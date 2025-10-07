#include "saga.h"
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace distributed {

SagaCoordinator::SagaCoordinator()
    : sagaTimeout_(std::chrono::milliseconds(30000)) { // 30 seconds
    std::cout << "Creating SagaCoordinator" << std::endl;
}

SagaCoordinator::~SagaCoordinator() {
    std::cout << "Destroying SagaCoordinator" << std::endl;
}

bool SagaCoordinator::initialize() {
    std::cout << "Initializing SagaCoordinator" << std::endl;
    
    // No background threads needed for Saga coordinator
    std::cout << "SagaCoordinator initialized successfully" << std::endl;
    return true;
}

void SagaCoordinator::shutdown() {
    std::cout << "Shutting down SagaCoordinator" << std::endl;
    
    // Clear sagas
    {
        std::lock_guard<std::mutex> lock(coordinatorMutex_);
        sagas_.clear();
    }
    
    std::cout << "SagaCoordinator shutdown completed" << std::endl;
}

bool SagaCoordinator::beginSaga(const std::string& sagaId) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    // Check if saga already exists
    if (sagas_.find(sagaId) != sagas_.end()) {
        std::cout << "Saga " << sagaId << " already exists" << std::endl;
        return false;
    }
    
    // Create new saga
    auto saga = std::make_unique<SagaInfo>(sagaId);
    sagas_[sagaId] = std::move(saga);
    
    std::cout << "Started saga " << sagaId << std::endl;
    return true;
}

bool SagaCoordinator::addStep(const std::string& sagaId, const SagaStep& step) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = sagas_.find(sagaId);
    if (it == sagas_.end()) {
        std::cout << "Saga " << sagaId << " not found" << std::endl;
        return false;
    }
    
    auto& saga = it->second;
    saga->steps.push_back(step);
    
    std::cout << "Added step " << step.id << " to saga " << sagaId << std::endl;
    return true;
}

bool SagaCoordinator::executeSaga(const std::string& sagaId) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = sagas_.find(sagaId);
    if (it == sagas_.end()) {
        std::cout << "Saga " << sagaId << " not found" << std::endl;
        return false;
    }
    
    auto& saga = it->second;
    
    // Check if saga is in active state
    if (saga->status.load() != SagaStatus::ACTIVE) {
        std::cout << "Saga " << sagaId << " is not in active state" << std::endl;
        return false;
    }
    
    std::cout << "Executing saga " << sagaId << std::endl;
    
    // Execute each step in order
    for (size_t i = 0; i < saga->steps.size(); i++) {
        auto& step = saga->steps[i];
        
        // Check if saga has timed out
        if (isSagaTimedOut(*saga)) {
            std::cout << "Saga " << sagaId << " timed out during execution" << std::endl;
            saga->status.store(SagaStatus::FAILED);
            compensatePreviousSteps(*saga, i);
            return false;
        }
        
        // Execute the step
        if (!executeStep(step)) {
            std::cout << "Step " << step.id << " failed in saga " << sagaId << ", compensating previous steps" << std::endl;
            saga->status.store(SagaStatus::FAILED);
            compensatePreviousSteps(*saga, i);
            return false;
        }
    }
    
    // All steps completed successfully
    saga->status.store(SagaStatus::COMPLETED);
    std::cout << "Saga " << sagaId << " completed successfully" << std::endl;
    return true;
}

// Get saga status
SagaStatus SagaCoordinator::getSagaStatus(const std::string& sagaId) const {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    auto it = sagas_.find(sagaId);
    if (it != sagas_.end()) {
        return it->second->status.load();
    }
    return SagaStatus::FAILED;
}

std::vector<SagaStep> SagaCoordinator::getSteps(const std::string& sagaId) const {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = sagas_.find(sagaId);
    if (it == sagas_.end()) {
        return {}; // Return empty vector if saga not found
    }
    
    return it->second->steps;
}

void SagaCoordinator::registerActionCallback(const SagaActionCallback& callback) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    actionCallback_ = callback;
    std::cout << "Action callback registered" << std::endl;
}

void SagaCoordinator::registerCompensationCallback(const SagaCompensationCallback& callback) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    compensationCallback_ = callback;
    std::cout << "Compensation callback registered" << std::endl;
}

void SagaCoordinator::setSagaTimeout(const std::chrono::milliseconds& timeout) {
    sagaTimeout_.store(timeout);
    std::cout << "Saga timeout set to " << timeout.count() << " ms" << std::endl;
}

std::chrono::milliseconds SagaCoordinator::getSagaTimeout() const {
    return sagaTimeout_.load();
}

bool SagaCoordinator::executeStep(SagaStep& step) {
    std::cout << "Executing step " << step.id << " with action " << step.action << " on participant " << step.participantId << std::endl;
    
    step.status = SagaStepStatus::EXECUTING;
    
    // Call action callback
    bool success = false;
    if (actionCallback_) {
        success = actionCallback_(step.participantId, step.action, step.data);
    } else {
        success = true; // Default to success if no callback registered
    }
    
    if (success) {
        step.status = SagaStepStatus::COMPLETED;
        std::cout << "Step " << step.id << " completed successfully" << std::endl;
    } else {
        step.status = SagaStepStatus::FAILED;
        std::cout << "Step " << step.id << " failed" << std::endl;
    }
    
    return success;
}

bool SagaCoordinator::compensateStep(const SagaStep& step) {
    std::cout << "Compensating step " << step.id << " with compensation " << step.compensation << " on participant " << step.participantId << std::endl;
    
    // Call compensation callback
    bool success = false;
    if (compensationCallback_) {
        success = compensationCallback_(step.participantId, step.compensation, step.data);
    } else {
        success = true; // Default to success if no callback registered
    }
    
    if (success) {
        std::cout << "Step " << step.id << " compensated successfully" << std::endl;
    } else {
        std::cout << "Step " << step.id << " compensation failed" << std::endl;
    }
    
    return success;
}

bool SagaCoordinator::compensatePreviousSteps(SagaInfo& saga, size_t failedStepIndex) {
    std::cout << "Compensating previous " << failedStepIndex << " steps in saga " << saga.sagaId << std::endl;
    
    bool allCompensated = true;
    
    // Compensate steps in reverse order
    for (size_t i = failedStepIndex; i > 0; i--) {
        const auto& step = saga.steps[i - 1];
        
        // Check if saga has timed out
        if (isSagaTimedOut(saga)) {
            std::cout << "Saga " << saga.sagaId << " timed out during compensation" << std::endl;
            allCompensated = false;
            break;
        }
        
        if (!compensateStep(step)) {
            std::cout << "Failed to compensate step " << step.id << " in saga " << saga.sagaId << std::endl;
            allCompensated = false;
        }
    }
    
    if (allCompensated) {
        saga.status.store(SagaStatus::COMPENSATED);
        std::cout << "Saga " << saga.sagaId << " compensated successfully" << std::endl;
    } else {
        saga.status.store(SagaStatus::FAILED);
        std::cout << "Saga " << saga.sagaId << " compensation failed" << std::endl;
    }
    
    return allCompensated;
}

bool SagaCoordinator::isSagaTimedOut(const SagaInfo& saga) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - saga.startTime);
    return elapsed > sagaTimeout_.load();
}

} // namespace distributed
} // namespace phantomdb