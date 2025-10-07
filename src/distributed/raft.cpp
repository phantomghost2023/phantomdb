#include "raft.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>

namespace phantomdb {
namespace distributed {

RaftConsensus::RaftConsensus(const std::string& nodeId, const std::vector<RaftNodeInfo>& nodes)
    : nodeId_(nodeId), nodes_(nodes), state_(RaftState::FOLLOWER), 
      currentTerm_(0), commitIndex_(0), lastApplied_(0), 
      electionTimeoutDuration_(std::chrono::milliseconds(150 + rand() % 150)), // 150-300ms
      heartbeatTimeoutDuration_(std::chrono::milliseconds(50)), // 50ms
      running_(false) {
    std::cout << "Creating RaftConsensus instance for node " << nodeId << std::endl;
    
    // Initialize nextIndex and matchIndex for each node
    for (const auto& node : nodes_) {
        if (node.id != nodeId_) {
            nextIndex_[node.id] = 1;
            matchIndex_[node.id] = 0;
        }
    }
    
    initializeTimers();
}

RaftConsensus::~RaftConsensus() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying RaftConsensus instance for node " << nodeId_ << std::endl;
}

bool RaftConsensus::initialize() {
    std::cout << "Initializing RaftConsensus for node " << nodeId_ << std::endl;
    
    running_ = true;
    consensusThread_ = std::make_unique<std::thread>(&RaftConsensus::consensusLoop, this);
    
    std::cout << "RaftConsensus initialized successfully for node " << nodeId_ << std::endl;
    return true;
}

void RaftConsensus::shutdown() {
    std::cout << "Shutting down RaftConsensus for node " << nodeId_ << std::endl;
    
    running_ = false;
    
    if (consensusThread_ && consensusThread_->joinable()) {
        consensusThread_->join();
        consensusThread_.reset();
    }
    
    // Clear state
    {
        std::lock_guard<std::mutex> lock(raftMutex_);
        log_.clear();
        nextIndex_.clear();
        matchIndex_.clear();
    }
    
    std::cout << "RaftConsensus shutdown completed for node " << nodeId_ << std::endl;
}

bool RaftConsensus::submitCommand(const std::string& command, const std::string& data) {
    if (state_ != RaftState::LEADER) {
        std::cout << "Node " << nodeId_ << " is not leader, cannot submit command" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    // Create new log entry
    LogEntry entry;
    entry.index = log_.empty() ? 1 : log_.back().index + 1;
    entry.term = currentTerm_.load();
    entry.command = command;
    entry.data = data;
    
    log_.push_back(entry);
    
    std::cout << "Submitted command " << command << " to log at index " << entry.index << " for node " << nodeId_ << std::endl;
    return true;
}

void RaftConsensus::registerApplyCommandCallback(const ApplyCommandCallback& callback) {
    applyCommandCallback_ = callback;
}

void RaftConsensus::registerSendRequestVoteCallback(const SendRequestVoteCallback& callback) {
    sendRequestVoteCallback_ = callback;
}

void RaftConsensus::registerSendAppendEntriesCallback(const SendAppendEntriesCallback& callback) {
    sendAppendEntriesCallback_ = callback;
}

bool RaftConsensus::handleRequestVote(const RequestVoteRequest& request, RequestVoteResponse& response) {
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    std::cout << "Node " << nodeId_ << " received RequestVote from candidate " << request.candidateId << " for term " << request.term << std::endl;
    
    // Reply false if term < currentTerm
    if (request.term < currentTerm_.load()) {
        response.term = currentTerm_.load();
        response.voteGranted = false;
        std::cout << "Node " << nodeId_ << " rejected vote for candidate " << request.candidateId << " - term too old" << std::endl;
        return true;
    }
    
    // If term > currentTerm, update currentTerm and convert to follower
    if (request.term > currentTerm_.load()) {
        becomeFollower(request.term);
    }
    
    // If votedFor is null or candidateId, and candidate's log is at least as up-to-date as receiver's log
    bool canVote = (votedFor_.empty() || votedFor_ == request.candidateId) && 
                   isLogUpToDate(request.lastLogIndex, request.lastLogTerm);
    
    if (canVote) {
        votedFor_ = request.candidateId;
        resetElectionTimeout();
        response.term = currentTerm_.load();
        response.voteGranted = true;
        std::cout << "Node " << nodeId_ << " granted vote to candidate " << request.candidateId << std::endl;
    } else {
        response.term = currentTerm_.load();
        response.voteGranted = false;
        std::cout << "Node " << nodeId_ << " rejected vote for candidate " << request.candidateId << " - already voted or log not up-to-date" << std::endl;
    }
    
    return true;
}

bool RaftConsensus::handleAppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& response) {
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    std::cout << "Node " << nodeId_ << " received AppendEntries from leader " << request.leaderId << " for term " << request.term << std::endl;
    
    // Reply false if term < currentTerm
    if (request.term < currentTerm_.load()) {
        response.term = currentTerm_.load();
        response.success = false;
        std::cout << "Node " << nodeId_ << " rejected AppendEntries from leader " << request.leaderId << " - term too old" << std::endl;
        return true;
    }
    
    // If term >= currentTerm, update currentTerm and convert to follower
    if (request.term > currentTerm_.load()) {
        becomeFollower(request.term);
    } else if (state_ == RaftState::CANDIDATE) {
        becomeFollower(currentTerm_.load());
    }
    
    // Reset election timeout
    resetElectionTimeout();
    leaderId_ = request.leaderId;
    
    // Reply false if log doesn't contain an entry at prevLogIndex whose term matches prevLogTerm
    if (request.prevLogIndex > 0) {
        if (request.prevLogIndex > getLastLogIndex()) {
            response.term = currentTerm_.load();
            response.success = false;
            response.conflictIndex = getLastLogIndex() + 1;
            response.conflictTerm = 0;
            std::cout << "Node " << nodeId_ << " rejected AppendEntries - prevLogIndex " << request.prevLogIndex << " out of range" << std::endl;
            return true;
        }
        
        if (log_.at(request.prevLogIndex - 1).term != request.prevLogTerm) {
            response.term = currentTerm_.load();
            response.success = false;
            response.conflictIndex = request.prevLogIndex;
            response.conflictTerm = log_.at(request.prevLogIndex - 1).term;
            std::cout << "Node " << nodeId_ << " rejected AppendEntries - prevLogTerm mismatch" << std::endl;
            return true;
        }
    }
    
    // If an existing entry conflicts with a new one, delete the existing entry and all that follow it
    uint64_t nextIndex = request.prevLogIndex + 1;
    for (size_t i = 0; i < request.entries.size(); i++) {
        uint64_t logIndex = request.prevLogIndex + 1 + i;
        if (logIndex <= getLastLogIndex() && 
            log_.at(logIndex - 1).term != request.entries[i].term) {
            // Delete conflicting entries and all that follow
            log_.erase(log_.begin() + logIndex - 1, log_.end());
            std::cout << "Node " << nodeId_ << " deleted conflicting entries from index " << logIndex << std::endl;
            break;
        }
    }
    
    // Append any new entries not already in the log
    for (const auto& entry : request.entries) {
        uint64_t logIndex = entry.index;
        if (logIndex > getLastLogIndex()) {
            log_.push_back(entry);
            std::cout << "Node " << nodeId_ << " appended entry at index " << logIndex << std::endl;
        }
    }
    
    // If leaderCommit > commitIndex, set commitIndex = min(leaderCommit, index of last new entry)
    if (request.leaderCommit > commitIndex_.load()) {
        uint64_t lastNewIndex = request.prevLogIndex + request.entries.size();
        commitIndex_.store(std::min(request.leaderCommit, lastNewIndex));
        std::cout << "Node " << nodeId_ << " updated commit index to " << commitIndex_.load() << std::endl;
    }
    
    response.term = currentTerm_.load();
    response.success = true;
    return true;
}

std::string RaftConsensus::getLeader() const {
    std::lock_guard<std::mutex> lock(raftMutex_);
    return leaderId_;
}

uint64_t RaftConsensus::getCurrentTerm() const {
    return currentTerm_.load();
}

RaftState RaftConsensus::getState() const {
    return state_.load();
}

uint64_t RaftConsensus::getCommitIndex() const {
    return commitIndex_.load();
}

uint64_t RaftConsensus::getLastApplied() const {
    return lastApplied_.load();
}

void RaftConsensus::initializeTimers() {
    resetElectionTimeout();
    resetHeartbeatTimeout();
}

void RaftConsensus::resetElectionTimeout() {
    auto now = std::chrono::steady_clock::now();
    electionTimeout_ = now + electionTimeoutDuration_;
}

void RaftConsensus::resetHeartbeatTimeout() {
    auto now = std::chrono::steady_clock::now();
    heartbeatTimeout_ = now + heartbeatTimeoutDuration_;
}

bool RaftConsensus::isElectionTimeoutExpired() const {
    return std::chrono::steady_clock::now() >= electionTimeout_;
}

bool RaftConsensus::isHeartbeatTimeoutExpired() const {
    return std::chrono::steady_clock::now() >= heartbeatTimeout_;
}

void RaftConsensus::becomeCandidate() {
    state_.store(RaftState::CANDIDATE);
    currentTerm_.fetch_add(1);
    votedFor_ = nodeId_;
    leaderId_.clear();
    
    std::cout << "Node " << nodeId_ << " became candidate for term " << currentTerm_.load() << std::endl;
}

void RaftConsensus::becomeLeader() {
    state_.store(RaftState::LEADER);
    leaderId_ = nodeId_;
    
    // Initialize nextIndex and matchIndex for each follower
    for (const auto& node : nodes_) {
        if (node.id != nodeId_) {
            nextIndex_[node.id] = getLastLogIndex() + 1;
            matchIndex_[node.id] = 0;
        }
    }
    
    std::cout << "Node " << nodeId_ << " became leader for term " << currentTerm_.load() << std::endl;
}

void RaftConsensus::becomeFollower(uint64_t term) {
    state_.store(RaftState::FOLLOWER);
    currentTerm_.store(term);
    votedFor_.clear();
    leaderId_.clear();
    
    std::cout << "Node " << nodeId_ << " became follower for term " << term << std::endl;
}

void RaftConsensus::startElection() {
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    becomeCandidate();
    
    // Send RequestVote RPCs to all other nodes
    uint64_t votesReceived = 1; // Vote for self
    uint64_t totalNodes = nodes_.size();
    
    RequestVoteRequest request;
    request.term = currentTerm_.load();
    request.candidateId = nodeId_;
    request.lastLogIndex = getLastLogIndex();
    request.lastLogTerm = getLastLogTerm();
    
    std::cout << "Node " << nodeId_ << " starting election for term " << request.term << " - requesting votes" << std::endl;
    
    // Send RequestVote RPCs in parallel
    for (const auto& node : nodes_) {
        if (node.id != nodeId_ && sendRequestVoteCallback_) {
            RequestVoteResponse response;
            if (sendRequestVoteCallback_(node, request, response)) {
                if (response.term > currentTerm_.load()) {
                    becomeFollower(response.term);
                    return;
                }
                
                if (response.voteGranted) {
                    votesReceived++;
                    std::cout << "Node " << nodeId_ << " received vote from " << node.id << std::endl;
                }
            }
        }
    }
    
    // If received votes from majority of nodes, become leader
    if (votesReceived > totalNodes / 2) {
        becomeLeader();
        // Send initial heartbeat
        sendHeartbeat();
    } else {
        std::cout << "Node " << nodeId_ << " did not receive majority votes (" << votesReceived << " of " << totalNodes << ")" << std::endl;
    }
}

void RaftConsensus::sendHeartbeat() {
    if (state_.load() != RaftState::LEADER) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    AppendEntriesRequest request;
    request.term = currentTerm_.load();
    request.leaderId = nodeId_;
    request.prevLogIndex = 0;
    request.prevLogTerm = 0;
    request.leaderCommit = commitIndex_.load();
    
    std::cout << "Node " << nodeId_ << " sending heartbeat for term " << request.term << std::endl;
    
    // Send AppendEntries RPCs to all other nodes
    for (const auto& node : nodes_) {
        if (node.id != nodeId_ && sendAppendEntriesCallback_) {
            AppendEntriesResponse response;
            if (sendAppendEntriesCallback_(node, request, response)) {
                if (response.term > currentTerm_.load()) {
                    becomeFollower(response.term);
                    return;
                }
            }
        }
    }
    
    resetHeartbeatTimeout();
}

void RaftConsensus::replicateLog() {
    if (state_.load() != RaftState::LEADER) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    // For each follower, send AppendEntries RPC with new log entries
    for (const auto& node : nodes_) {
        if (node.id != nodeId_ && sendAppendEntriesCallback_) {
            auto nextIndexIt = nextIndex_.find(node.id);
            auto matchIndexIt = matchIndex_.find(node.id);
            
            if (nextIndexIt != nextIndex_.end() && matchIndexIt != matchIndex_.end()) {
                uint64_t nextIndex = nextIndexIt->second;
                
                // Send entries starting from nextIndex
                if (nextIndex <= getLastLogIndex()) {
                    AppendEntriesRequest request;
                    request.term = currentTerm_.load();
                    request.leaderId = nodeId_;
                    request.leaderCommit = commitIndex_.load();
                    
                    // Set prevLogIndex and prevLogTerm
                    if (nextIndex > 1) {
                        request.prevLogIndex = nextIndex - 1;
                        request.prevLogTerm = log_.at(request.prevLogIndex - 1).term;
                    } else {
                        request.prevLogIndex = 0;
                        request.prevLogTerm = 0;
                    }
                    
                    // Add entries to send
                    for (uint64_t i = nextIndex; i <= getLastLogIndex(); i++) {
                        request.entries.push_back(log_.at(i - 1));
                    }
                    
                    AppendEntriesResponse response;
                    if (sendAppendEntriesCallback_(node, request, response)) {
                        if (response.term > currentTerm_.load()) {
                            becomeFollower(response.term);
                            return;
                        }
                        
                        if (response.success) {
                            // Update nextIndex and matchIndex
                            nextIndexIt->second = nextIndex + request.entries.size();
                            if (!request.entries.empty()) {
                                matchIndexIt->second = request.entries.back().index;
                            }
                            
                            std::cout << "Node " << nodeId_ << " successfully replicated entries to " << node.id << std::endl;
                        } else {
                            // Decrement nextIndex and retry
                            if (nextIndexIt->second > 1) {
                                nextIndexIt->second--;
                            }
                            std::cout << "Node " << nodeId_ << " failed to replicate entries to " << node.id << " - decrementing nextIndex" << std::endl;
                        }
                    }
                }
            }
        }
    }
    
    // Update commit index based on matchIndex values
    uint64_t newCommitIndex = commitIndex_.load();
    for (uint64_t i = commitIndex_.load() + 1; i <= getLastLogIndex(); i++) {
        uint64_t count = 0;
        for (const auto& node : nodes_) {
            if (node.id != nodeId_) {
                auto matchIndexIt = matchIndex_.find(node.id);
                if (matchIndexIt != matchIndex_.end() && matchIndexIt->second >= i) {
                    count++;
                }
            }
        }
        
        // If majority of nodes have this entry, update commit index
        if (count > nodes_.size() / 2) {
            if (i <= getLastLogIndex() && log_.at(i - 1).term == currentTerm_.load()) {
                newCommitIndex = i;
            }
        }
    }
    
    if (newCommitIndex > commitIndex_.load()) {
        commitIndex_.store(newCommitIndex);
        std::cout << "Node " << nodeId_ << " updated commit index to " << newCommitIndex << std::endl;
    }
}

void RaftConsensus::applyCommittedEntries() {
    std::lock_guard<std::mutex> lock(raftMutex_);
    
    uint64_t commitIndex = commitIndex_.load();
    uint64_t lastApplied = lastApplied_.load();
    
    if (commitIndex > lastApplied) {
        for (uint64_t i = lastApplied + 1; i <= commitIndex; i++) {
            if (i <= getLastLogIndex()) {
                const LogEntry& entry = log_.at(i - 1);
                
                // Apply the command to the state machine
                if (applyCommandCallback_) {
                    applyCommandCallback_(entry.command, entry.data);
                    std::cout << "Node " << nodeId_ << " applied command " << entry.command << " at index " << i << std::endl;
                }
                
                lastApplied_.store(i);
            }
        }
    }
}

void RaftConsensus::consensusLoop() {
    std::cout << "Node " << nodeId_ << " starting consensus loop" << std::endl;
    
    while (running_) {
        try {
            switch (state_.load()) {
                case RaftState::FOLLOWER:
                    if (isElectionTimeoutExpired()) {
                        startElection();
                    }
                    break;
                    
                case RaftState::CANDIDATE:
                    if (isElectionTimeoutExpired()) {
                        startElection();
                    }
                    break;
                    
                case RaftState::LEADER:
                    if (isHeartbeatTimeoutExpired()) {
                        sendHeartbeat();
                        replicateLog();
                    }
                    break;
            }
            
            // Apply committed entries
            applyCommittedEntries();
            
            // Small delay to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } catch (const std::exception& e) {
            std::cout << "Exception in consensus loop for node " << nodeId_ << ": " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Node " << nodeId_ << " consensus loop ended" << std::endl;
}

uint64_t RaftConsensus::getLastLogIndex() const {
    return log_.empty() ? 0 : log_.back().index;
}

uint64_t RaftConsensus::getLastLogTerm() const {
    return log_.empty() ? 0 : log_.back().term;
}

bool RaftConsensus::isLogUpToDate(uint64_t lastLogIndex, uint64_t lastLogTerm) const {
    uint64_t myLastLogIndex = getLastLogIndex();
    uint64_t myLastLogTerm = getLastLogTerm();
    
    return (lastLogTerm > myLastLogTerm) || 
           (lastLogTerm == myLastLogTerm && lastLogIndex >= myLastLogIndex);
}

} // namespace distributed
} // namespace phantomdb