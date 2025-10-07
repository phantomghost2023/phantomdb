#ifndef PHANTOMDB_RAFT_H
#define PHANTOMDB_RAFT_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>
#include <functional>

namespace phantomdb {
namespace distributed {

// Raft node states
enum class RaftState {
    FOLLOWER,
    CANDIDATE,
    LEADER
};

// Log entry structure
struct LogEntry {
    uint64_t index;
    uint64_t term;
    std::string command;
    std::string data;
    
    LogEntry() : index(0), term(0) {}
    LogEntry(uint64_t idx, uint64_t t, const std::string& cmd, const std::string& d)
        : index(idx), term(t), command(cmd), data(d) {}
};

// Raft node information
struct RaftNodeInfo {
    std::string id;
    std::string address;
    int port;
    
    RaftNodeInfo(const std::string& nodeId, const std::string& addr, int p)
        : id(nodeId), address(addr), port(p) {}
};

// RPC request/response structures
struct RequestVoteRequest {
    uint64_t term;
    std::string candidateId;
    uint64_t lastLogIndex;
    uint64_t lastLogTerm;
};

struct RequestVoteResponse {
    uint64_t term;
    bool voteGranted;
};

struct AppendEntriesRequest {
    uint64_t term;
    std::string leaderId;
    uint64_t prevLogIndex;
    uint64_t prevLogTerm;
    std::vector<LogEntry> entries;
    uint64_t leaderCommit;
};

struct AppendEntriesResponse {
    uint64_t term;
    bool success;
    uint64_t conflictIndex;
    uint64_t conflictTerm;
};

// Callback function types
using ApplyCommandCallback = std::function<void(const std::string&, const std::string&)>;
using SendRequestVoteCallback = std::function<bool(const RaftNodeInfo&, const RequestVoteRequest&, RequestVoteResponse&)>;
using SendAppendEntriesCallback = std::function<bool(const RaftNodeInfo&, const AppendEntriesRequest&, AppendEntriesResponse&)>;

class RaftConsensus {
public:
    RaftConsensus(const std::string& nodeId, const std::vector<RaftNodeInfo>& nodes);
    ~RaftConsensus();
    
    // Initialize the Raft consensus module
    bool initialize();
    
    // Shutdown the Raft consensus module
    void shutdown();
    
    // Submit a command to be replicated
    bool submitCommand(const std::string& command, const std::string& data);
    
    // Register callback for applying commands
    void registerApplyCommandCallback(const ApplyCommandCallback& callback);
    
    // Register callback for sending RequestVote RPCs
    void registerSendRequestVoteCallback(const SendRequestVoteCallback& callback);
    
    // Register callback for sending AppendEntries RPCs
    void registerSendAppendEntriesCallback(const SendAppendEntriesCallback& callback);
    
    // Handle incoming RequestVote RPC
    bool handleRequestVote(const RequestVoteRequest& request, RequestVoteResponse& response);
    
    // Handle incoming AppendEntries RPC
    bool handleAppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& response);
    
    // Get current leader
    std::string getLeader() const;
    
    // Get current term
    uint64_t getCurrentTerm() const;
    
    // Get node state
    RaftState getState() const;
    
    // Get commit index
    uint64_t getCommitIndex() const;
    
    // Get last applied index
    uint64_t getLastApplied() const;

private:
    // Node ID
    std::string nodeId_;
    
    // Cluster nodes
    std::vector<RaftNodeInfo> nodes_;
    
    // Current state
    std::atomic<RaftState> state_;
    
    // Current term
    std::atomic<uint64_t> currentTerm_;
    
    // Candidate that received vote in current term (or empty if none)
    std::string votedFor_;
    
    // Log entries
    std::vector<LogEntry> log_;
    
    // Index of highest log entry known to be committed
    std::atomic<uint64_t> commitIndex_;
    
    // Index of highest log entry applied to state machine
    std::atomic<uint64_t> lastApplied_;
    
    // For each server, index of the next log entry to send to that server
    std::unordered_map<std::string, uint64_t> nextIndex_;
    
    // For each server, index of highest log entry known to be replicated on server
    std::unordered_map<std::string, uint64_t> matchIndex_;
    
    // Leader ID
    std::string leaderId_;
    
    // Election timer
    std::chrono::steady_clock::time_point electionTimeout_;
    std::chrono::milliseconds electionTimeoutDuration_;
    
    // Heartbeat timer
    std::chrono::steady_clock::time_point heartbeatTimeout_;
    std::chrono::milliseconds heartbeatTimeoutDuration_;
    
    // Background thread for consensus operations
    std::unique_ptr<std::thread> consensusThread_;
    std::atomic<bool> running_;
    
    // Mutex for thread safety
    mutable std::mutex raftMutex_;
    
    // Callbacks
    ApplyCommandCallback applyCommandCallback_;
    SendRequestVoteCallback sendRequestVoteCallback_;
    SendAppendEntriesCallback sendAppendEntriesCallback_;
    
    // Initialize timers
    void initializeTimers();
    
    // Reset election timeout
    void resetElectionTimeout();
    
    // Reset heartbeat timeout
    void resetHeartbeatTimeout();
    
    // Check if election timeout has expired
    bool isElectionTimeoutExpired() const;
    
    // Check if heartbeat timeout has expired
    bool isHeartbeatTimeoutExpired() const;
    
    // Convert to candidate
    void becomeCandidate();
    
    // Convert to leader
    void becomeLeader();
    
    // Convert to follower
    void becomeFollower(uint64_t term);
    
    // Start election
    void startElection();
    
    // Send heartbeat
    void sendHeartbeat();
    
    // Replicate log entries to followers
    void replicateLog();
    
    // Apply committed entries
    void applyCommittedEntries();
    
    // Background consensus loop
    void consensusLoop();
    
    // Get last log index
    uint64_t getLastLogIndex() const;
    
    // Get last log term
    uint64_t getLastLogTerm() const;
    
    // Check if log is up-to-date
    bool isLogUpToDate(uint64_t lastLogIndex, uint64_t lastLogTerm) const;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_RAFT_H