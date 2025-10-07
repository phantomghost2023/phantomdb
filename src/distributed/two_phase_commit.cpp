#include "two_phase_commit.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

namespace phantomdb {
namespace distributed {

TwoPhaseCommitCoordinator::TwoPhaseCommitCoordinator()
    : prepareTimeout_(std::chrono::milliseconds(5000)), // 5 seconds
      commitAbortTimeout_(std::chrono::milliseconds(3000)), // 3 seconds
      running_(false) {
    std::cout << "Creating TwoPhaseCommitCoordinator" << std::endl;
}

TwoPhaseCommitCoordinator::~TwoPhaseCommitCoordinator() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying TwoPhaseCommitCoordinator" << std::endl;
}

bool TwoPhaseCommitCoordinator::initialize() {
    std::cout << "Initializing TwoPhaseCommitCoordinator" << std::endl;
    
    running_ = true;
    timeoutThread_ = std::make_unique<std::thread>(&TwoPhaseCommitCoordinator::timeoutHandler, this);
    
    std::cout << "TwoPhaseCommitCoordinator initialized successfully" << std::endl;
    return true;
}

void TwoPhaseCommitCoordinator::shutdown() {
    std::cout << "Shutting down TwoPhaseCommitCoordinator" << std::endl;
    
    running_ = false;
    
    if (timeoutThread_ && timeoutThread_->joinable()) {
        timeoutThread_->join();
        timeoutThread_.reset();
    }
    
    // Clear transactions
    {
        std::lock_guard<std::mutex> lock(coordinatorMutex_);
        transactions_.clear();
    }
    
    std::cout << "TwoPhaseCommitCoordinator shutdown completed" << std::endl;
}

bool TwoPhaseCommitCoordinator::beginTransaction(const std::string& transactionId, 
                                               const std::vector<ParticipantInfo>& participants) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    // Check if transaction already exists
    if (transactions_.find(transactionId) != transactions_.end()) {
        std::cout << "Transaction " << transactionId << " already exists" << std::endl;
        return false;
    }
    
    // Create new transaction
    auto transaction = std::make_unique<TransactionInfo>(transactionId);
    transaction->participants = participants;
    
    transactions_[transactionId] = std::move(transaction);
    
    std::cout << "Started transaction " << transactionId << " with " << participants.size() << " participants" << std::endl;
    return true;
}

bool TwoPhaseCommitCoordinator::addParticipant(const std::string& transactionId, 
                                              const ParticipantInfo& participant) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        std::cout << "Transaction " << transactionId << " not found" << std::endl;
        return false;
    }
    
    auto& transaction = it->second;
    transaction->participants.push_back(participant);
    
    std::cout << "Added participant " << participant.id << " to transaction " << transactionId << std::endl;
    return true;
}

bool TwoPhaseCommitCoordinator::executeTwoPhaseCommit(const std::string& transactionId) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        std::cout << "Transaction " << transactionId << " not found" << std::endl;
        return false;
    }
    
    auto& transaction = it->second;
    
    // Check if transaction is in active state
    if (transaction->state.load() != TransactionState::ACTIVE) {
        std::cout << "Transaction " << transactionId << " is not in active state" << std::endl;
        return false;
    }
    
    std::cout << "Executing two-phase commit for transaction " << transactionId << std::endl;
    
    // Phase 1: Prepare
    std::cout << "Phase 1: Sending prepare requests for transaction " << transactionId << std::endl;
    std::vector<ParticipantResponse> prepareResponses = 
        collectResponses(transactionId, transaction->participants, TwoPhaseCommitPhase::PREPARE);
    
    // Check if all participants voted YES
    bool allPrepared = true;
    for (const auto& response : prepareResponses) {
        if (!response.vote) {
            allPrepared = false;
            std::cout << "Participant " << response.participantId << " voted NO in prepare phase for transaction " << transactionId << ": " << response.errorMessage << std::endl;
        }
    }
    
    if (allPrepared) {
        // All participants prepared successfully, proceed to commit
        transaction->state.store(TransactionState::PREPARED);
        std::cout << "All participants prepared successfully for transaction " << transactionId << std::endl;
        
        // Phase 2: Commit
        std::cout << "Phase 2: Sending commit requests for transaction " << transactionId << std::endl;
        std::vector<ParticipantResponse> commitResponses = 
            collectResponses(transactionId, transaction->participants, TwoPhaseCommitPhase::COMMIT);
        
        // Check if all participants committed successfully
        bool allCommitted = true;
        for (const auto& response : commitResponses) {
            if (!response.vote) {
                allCommitted = false;
                std::cout << "Participant " << response.participantId << " failed to commit transaction " << transactionId << ": " << response.errorMessage << std::endl;
            }
        }
        
        if (allCommitted) {
            transaction->state.store(TransactionState::COMMITTED);
            std::cout << "Transaction " << transactionId << " committed successfully" << std::endl;
            return true;
        } else {
            transaction->state.store(TransactionState::ABORTED);
            std::cout << "Transaction " << transactionId << " failed to commit, marked as aborted" << std::endl;
            return false;
        }
    } else {
        // At least one participant voted NO, abort the transaction
        transaction->state.store(TransactionState::ABORTED);
        std::cout << "Transaction " << transactionId << " aborted due to prepare phase failure" << std::endl;
        
        // Send abort requests to all participants
        std::cout << "Sending abort requests for transaction " << transactionId << std::endl;
        collectResponses(transactionId, transaction->participants, TwoPhaseCommitPhase::ABORT);
        
        return false;
    }
}

TransactionState TwoPhaseCommitCoordinator::getTransactionState(const std::string& transactionId) const {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        return TransactionState::ABORTED; // Transaction not found, consider it aborted
    }
    
    return it->second->state.load();
}

std::vector<ParticipantInfo> TwoPhaseCommitCoordinator::getParticipants(const std::string& transactionId) const {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        return {}; // Return empty vector if transaction not found
    }
    
    return it->second->participants;
}

void TwoPhaseCommitCoordinator::registerPrepareCallback(const PrepareCallback& callback) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    prepareCallback_ = callback;
    std::cout << "Prepare callback registered" << std::endl;
}

void TwoPhaseCommitCoordinator::registerCommitCallback(const CommitCallback& callback) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    commitCallback_ = callback;
    std::cout << "Commit callback registered" << std::endl;
}

void TwoPhaseCommitCoordinator::registerAbortCallback(const AbortCallback& callback) {
    std::lock_guard<std::mutex> lock(coordinatorMutex_);
    abortCallback_ = callback;
    std::cout << "Abort callback registered" << std::endl;
}

void TwoPhaseCommitCoordinator::setPrepareTimeout(const std::chrono::milliseconds& timeout) {
    prepareTimeout_.store(timeout);
    std::cout << "Prepare timeout set to " << timeout.count() << " ms" << std::endl;
}

void TwoPhaseCommitCoordinator::setCommitAbortTimeout(const std::chrono::milliseconds& timeout) {
    commitAbortTimeout_.store(timeout);
    std::cout << "Commit/Abort timeout set to " << timeout.count() << " ms" << std::endl;
}

std::chrono::milliseconds TwoPhaseCommitCoordinator::getPrepareTimeout() const {
    return prepareTimeout_.load();
}

std::chrono::milliseconds TwoPhaseCommitCoordinator::getCommitAbortTimeout() const {
    return commitAbortTimeout_.load();
}

bool TwoPhaseCommitCoordinator::sendPrepareRequests(const std::string& transactionId, 
                                                   const std::vector<ParticipantInfo>& participants) {
    // Send prepare requests to all participants
    // In a real implementation, this would send network requests
    // For now, we'll simulate successful responses
    
    std::cout << "Sending prepare requests for transaction " << transactionId << " to " << participants.size() << " participants" << std::endl;
    
    return true;
}

bool TwoPhaseCommitCoordinator::sendCommitRequests(const std::string& transactionId, 
                                                  const std::vector<ParticipantInfo>& participants) {
    // Send commit requests to all participants
    // In a real implementation, this would send network requests
    // For now, we'll simulate successful responses
    
    std::cout << "Sending commit requests for transaction " << transactionId << " to " << participants.size() << " participants" << std::endl;
    
    return true;
}

bool TwoPhaseCommitCoordinator::sendAbortRequests(const std::string& transactionId, 
                                                 const std::vector<ParticipantInfo>& participants) {
    // Send abort requests to all participants
    // In a real implementation, this would send network requests
    // For now, we'll simulate successful responses
    
    std::cout << "Sending abort requests for transaction " << transactionId << " to " << participants.size() << " participants" << std::endl;
    
    return true;
}

std::vector<ParticipantResponse> TwoPhaseCommitCoordinator::collectResponses(
    const std::string& transactionId, 
    const std::vector<ParticipantInfo>& participants,
    TwoPhaseCommitPhase phase) {
    
    std::vector<ParticipantResponse> responses;
    responses.reserve(participants.size());
    
    // Simulate collecting responses from participants
    for (const auto& participant : participants) {
        bool success = false;
        std::string errorMessage;
        
        // Call appropriate callback based on phase
        switch (phase) {
            case TwoPhaseCommitPhase::PREPARE:
                if (prepareCallback_) {
                    success = prepareCallback_(participant, transactionId);
                } else {
                    success = true; // Default to success if no callback registered
                }
                break;
            case TwoPhaseCommitPhase::COMMIT:
                if (commitCallback_) {
                    success = commitCallback_(participant, transactionId);
                } else {
                    success = true; // Default to success if no callback registered
                }
                break;
            case TwoPhaseCommitPhase::ABORT:
                if (abortCallback_) {
                    success = abortCallback_(participant, transactionId);
                } else {
                    success = true; // Default to success if no callback registered
                }
                break;
        }
        
        if (!success) {
            errorMessage = "Participant failed to respond";
        }
        
        responses.emplace_back(participant.id, success, errorMessage);
        
        if (success) {
            std::cout << "Participant " << participant.id << " responded successfully in " << 
                        ((phase == TwoPhaseCommitPhase::PREPARE) ? "PREPARE" :
                         (phase == TwoPhaseCommitPhase::COMMIT) ? "COMMIT" : "ABORT") << 
                        " phase for transaction " << transactionId << std::endl;
        } else {
            std::cout << "Participant " << participant.id << " failed in " << 
                        ((phase == TwoPhaseCommitPhase::PREPARE) ? "PREPARE" :
                         (phase == TwoPhaseCommitPhase::COMMIT) ? "COMMIT" : "ABORT") << 
                        " phase for transaction " << transactionId << ": " << errorMessage << std::endl;
        }
    }
    
    return responses;
}

void TwoPhaseCommitCoordinator::timeoutHandler() {
    std::cout << "Starting timeout handler thread" << std::endl;
    
    while (running_) {
        try {
            // Check for timed-out transactions
            {
                std::lock_guard<std::mutex> lock(coordinatorMutex_);
                
                auto it = transactions_.begin();
                while (it != transactions_.end()) {
                    auto& transaction = it->second;
                    if (isTransactionTimedOut(*transaction)) {
                        std::cout << "Transaction " << transaction->transactionId << " timed out, aborting" << std::endl;
                        abortTimedOutTransaction(transaction->transactionId);
                        // Remove the transaction after aborting
                        it = transactions_.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            
            // Sleep for a short interval
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (const std::exception& e) {
            std::cout << "Exception in timeout handler: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Timeout handler thread ended" << std::endl;
}

bool TwoPhaseCommitCoordinator::isTransactionTimedOut(const TransactionInfo& transaction) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - transaction.startTime);
    
    // Use prepare timeout for active transactions, commit/abort timeout for prepared transactions
    std::chrono::milliseconds timeout;
    if (transaction.state.load() == TransactionState::ACTIVE) {
        timeout = prepareTimeout_.load();
    } else {
        timeout = commitAbortTimeout_.load();
    }
    
    return elapsed > timeout;
}

void TwoPhaseCommitCoordinator::abortTimedOutTransaction(const std::string& transactionId) {
    // In a real implementation, we would send abort requests to all participants
    // For now, we'll just update the transaction state
    
    auto it = transactions_.find(transactionId);
    if (it != transactions_.end()) {
        it->second->state.store(TransactionState::ABORTED);
        std::cout << "Transaction " << transactionId << " marked as aborted due to timeout" << std::endl;
    }
}

} // namespace distributed
} // namespace phantomdb