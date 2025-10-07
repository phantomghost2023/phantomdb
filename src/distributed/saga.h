#ifndef PHANTOMDB_SAGA_H
#define PHANTOMDB_SAGA_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>

namespace phantomdb {
namespace distributed {

// Saga step types
enum class SagaStepType {
    ACTION,
    COMPENSATION
};

// Saga step status
enum class SagaStepStatus {
    PENDING,
    EXECUTING,
    COMPLETED,
    FAILED,
    COMPENSATED
};

// Saga status
enum class SagaStatus {
    ACTIVE,
    COMPLETED,
    FAILED,
    COMPENSATED
};

// Saga step information
struct SagaStep {
    std::string id;
    SagaStepType type;
    std::atomic<SagaStepStatus> status;
    std::string action;
    std::string compensation;
    std::string participantId;
    std::string data;
    
    SagaStep(const std::string& stepId, const std::string& actionCmd, const std::string& compCmd, 
             const std::string& participant, const std::string& stepData = "")
        : id(stepId), type(SagaStepType::ACTION), status(SagaStepStatus::PENDING),
          action(actionCmd), compensation(compCmd), participantId(participant), data(stepData) {}
};

// Callback function types
using SagaActionCallback = std::function<bool(const std::string& participantId, 
                                             const std::string& action, 
                                             const std::string& data)>;
using SagaCompensationCallback = std::function<bool(const std::string& participantId, 
                                                   const std::string& compensation, 
                                                   const std::string& data)>;

class SagaCoordinator {
public:
    SagaCoordinator();
    ~SagaCoordinator();
    
    // Initialize the Saga coordinator
    bool initialize();
    
    // Shutdown the Saga coordinator
    void shutdown();
    
    // Begin a new saga
    bool beginSaga(const std::string& sagaId);
    
    // Add a step to the saga
    bool addStep(const std::string& sagaId, const SagaStep& step);
    
    // Execute the saga
    bool executeSaga(const std::string& sagaId);
    
    // Get saga status
    SagaStatus getSagaStatus(const std::string& sagaId) const;
    
    // Get all steps for a saga
    std::vector<SagaStep> getSteps(const std::string& sagaId) const;
    
    // Register callbacks
    void registerActionCallback(const SagaActionCallback& callback);
    void registerCompensationCallback(const SagaCompensationCallback& callback);
    
    // Set timeout for saga execution
    void setSagaTimeout(const std::chrono::milliseconds& timeout);
    
    // Get timeout value
    std::chrono::milliseconds getSagaTimeout() const;

private:
    // Saga information
    struct SagaInfo {
        std::string sagaId;
        std::vector<SagaStep> steps;
        std::atomic<SagaStatus> status;
        std::chrono::steady_clock::time_point startTime;
        
        SagaInfo(const std::string& id) 
            : sagaId(id), status(SagaStatus::ACTIVE) {
            startTime = std::chrono::steady_clock::now();
        }
    };
    
    // Active sagas
    std::unordered_map<std::string, std::unique_ptr<SagaInfo>> sagas_;
    
    // Callbacks
    SagaActionCallback actionCallback_;
    SagaCompensationCallback compensationCallback_;
    
    // Timeout value
    std::atomic<std::chrono::milliseconds> sagaTimeout_;
    
    // Mutex for thread safety
    mutable std::mutex coordinatorMutex_;
    
    // Internal methods
    bool executeStep(SagaStep& step);
    bool compensateStep(const SagaStep& step);
    bool compensatePreviousSteps(SagaInfo& saga, size_t failedStepIndex);
    
    // Check if saga has timed out
    bool isSagaTimedOut(const SagaInfo& saga) const;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_SAGA_H