#ifndef PHANTOMDB_TWO_PHASE_COMMIT_H
#define PHANTOMDB_TWO_PHASE_COMMIT_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

namespace phantomdb {
namespace distributed {

// 2PC participant information
struct ParticipantInfo {
    std::string id;
    std::string address;
    int port;
    
    ParticipantInfo(const std::string& participantId, const std::string& addr, int p)
        : id(participantId), address(addr), port(p) {}
};

// Transaction states
enum class TransactionState {
    ACTIVE,
    PREPARED,
    COMMITTED,
    ABORTED,
    TIMEOUT
};

// 2PC phases
enum class TwoPhaseCommitPhase {
    PREPARE,
    COMMIT,
    ABORT
};

// Participant response
struct ParticipantResponse {
    std::string participantId;
    bool vote; // true for YES, false for NO
    std::string errorMessage;
    
    ParticipantResponse(const std::string& id, bool v, const std::string& error = "")
        : participantId(id), vote(v), errorMessage(error) {}
};

// Callback function types
using PrepareCallback = std::function<bool(const ParticipantInfo&, const std::string& transactionId)>;
using CommitCallback = std::function<bool(const ParticipantInfo&, const std::string& transactionId)>;
using AbortCallback = std::function<bool(const ParticipantInfo&, const std::string& transactionId)>;

class TwoPhaseCommitCoordinator {
public:
    TwoPhaseCommitCoordinator();
    ~TwoPhaseCommitCoordinator();
    
    // Initialize the 2PC coordinator
    bool initialize();
    
    // Shutdown the 2PC coordinator
    void shutdown();
    
    // Begin a new distributed transaction
    bool beginTransaction(const std::string& transactionId, const std::vector<ParticipantInfo>& participants);
    
    // Add a participant to an existing transaction
    bool addParticipant(const std::string& transactionId, const ParticipantInfo& participant);
    
    // Execute the two-phase commit protocol
    bool executeTwoPhaseCommit(const std::string& transactionId);
    
    // Get transaction state
    TransactionState getTransactionState(const std::string& transactionId) const;
    
    // Get all participants for a transaction
    std::vector<ParticipantInfo> getParticipants(const std::string& transactionId) const;
    
    // Register callbacks
    void registerPrepareCallback(const PrepareCallback& callback);
    void registerCommitCallback(const CommitCallback& callback);
    void registerAbortCallback(const AbortCallback& callback);
    
    // Set timeout for prepare phase
    void setPrepareTimeout(const std::chrono::milliseconds& timeout);
    
    // Set timeout for commit/abort phase
    void setCommitAbortTimeout(const std::chrono::milliseconds& timeout);
    
    // Get timeout values
    std::chrono::milliseconds getPrepareTimeout() const;
    std::chrono::milliseconds getCommitAbortTimeout() const;

private:
    // Transaction information
    struct TransactionInfo {
        std::string transactionId;
        std::vector<ParticipantInfo> participants;
        std::atomic<TransactionState> state;
        std::chrono::steady_clock::time_point startTime;
        
        TransactionInfo(const std::string& id) 
            : transactionId(id), state(TransactionState::ACTIVE) {
            startTime = std::chrono::steady_clock::now();
        }
    };
    
    // Active transactions
    std::unordered_map<std::string, std::unique_ptr<TransactionInfo>> transactions_;
    
    // Callbacks
    PrepareCallback prepareCallback_;
    CommitCallback commitCallback_;
    AbortCallback abortCallback_;
    
    // Timeout values
    std::atomic<std::chrono::milliseconds> prepareTimeout_;
    std::atomic<std::chrono::milliseconds> commitAbortTimeout_;
    
    // Mutex for thread safety
    mutable std::mutex coordinatorMutex_;
    
    // Background thread for timeout handling
    std::unique_ptr<std::thread> timeoutThread_;
    std::atomic<bool> running_;
    
    // Internal methods
    bool sendPrepareRequests(const std::string& transactionId, const std::vector<ParticipantInfo>& participants);
    bool sendCommitRequests(const std::string& transactionId, const std::vector<ParticipantInfo>& participants);
    bool sendAbortRequests(const std::string& transactionId, const std::vector<ParticipantInfo>& participants);
    
    // Collect responses from participants
    std::vector<ParticipantResponse> collectResponses(const std::string& transactionId, 
                                                     const std::vector<ParticipantInfo>& participants,
                                                     TwoPhaseCommitPhase phase);
    
    // Timeout handling
    void timeoutHandler();
    
    // Check if transaction has timed out
    bool isTransactionTimedOut(const TransactionInfo& transaction) const;
    
    // Abort a timed-out transaction
    void abortTimedOutTransaction(const std::string& transactionId);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_TWO_PHASE_COMMIT_H