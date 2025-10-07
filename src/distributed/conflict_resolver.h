#ifndef PHANTOMDB_CONFLICT_RESOLVER_H
#define PHANTOMDB_CONFLICT_RESOLVER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <chrono>

namespace phantomdb {
namespace distributed {

// Conflict resolution strategies
enum class ConflictResolutionStrategy {
    LATEST_TIMESTAMP,
    LWW_REGISTER,
    MULTI_VALUE,
    CUSTOM
};

// Conflict information
struct ConflictInfo {
    std::string key;
    std::vector<std::string> conflictingValues;
    std::vector<std::chrono::system_clock::time_point> timestamps;
    std::vector<std::string> sources;
    
    ConflictInfo(const std::string& k) : key(k) {}
};

// Resolved conflict result
struct ResolvedConflict {
    std::string key;
    std::string resolvedValue;
    std::chrono::system_clock::time_point resolutionTime;
    std::string resolutionStrategy;
    
    ResolvedConflict(const std::string& k, const std::string& value, 
                     const std::string& strategy)
        : key(k), resolvedValue(value), 
          resolutionTime(std::chrono::system_clock::now()),
          resolutionStrategy(strategy) {}
};

// Custom conflict resolution function type
using CustomConflictResolver = std::function<std::string(const ConflictInfo&)>;

class ConflictResolver {
public:
    ConflictResolver(ConflictResolutionStrategy strategy = ConflictResolutionStrategy::LATEST_TIMESTAMP);
    ~ConflictResolver();
    
    // Initialize the conflict resolver
    bool initialize();
    
    // Shutdown the conflict resolver
    void shutdown();
    
    // Set conflict resolution strategy
    void setResolutionStrategy(ConflictResolutionStrategy strategy);
    
    // Get conflict resolution strategy
    ConflictResolutionStrategy getResolutionStrategy() const;
    
    // Register custom conflict resolver
    void registerCustomResolver(const CustomConflictResolver& resolver);
    
    // Resolve a conflict
    ResolvedConflict resolveConflict(const ConflictInfo& conflict);
    
    // Resolve multiple conflicts
    std::vector<ResolvedConflict> resolveConflicts(const std::vector<ConflictInfo>& conflicts);
    
    // Get conflict resolution statistics
    struct ResolutionStats {
        uint64_t totalConflicts;
        uint64_t resolvedByTimestamp;
        uint64_t resolvedByLWW;
        uint64_t resolvedByMultiValue;
        uint64_t resolvedByCustom;
        uint64_t unresolvedConflicts;
        
        ResolutionStats() : totalConflicts(0), resolvedByTimestamp(0), resolvedByLWW(0), 
                           resolvedByMultiValue(0), resolvedByCustom(0), unresolvedConflicts(0) {}
    };
    
    ResolutionStats getResolutionStats() const;

private:
    // Conflict resolution strategy
    ConflictResolutionStrategy strategy_;
    
    // Custom conflict resolver
    CustomConflictResolver customResolver_;
    
    // Resolution statistics
    mutable ResolutionStats stats_;
    mutable std::mutex statsMutex_;
    
    // Mutex for thread safety
    mutable std::mutex resolverMutex_;
    
    // Internal conflict resolution methods
    ResolvedConflict resolveByLatestTimestamp(const ConflictInfo& conflict);
    ResolvedConflict resolveByLWWRegister(const ConflictInfo& conflict);
    ResolvedConflict resolveByMultiValue(const ConflictInfo& conflict);
    ResolvedConflict resolveByCustom(const ConflictInfo& conflict);
    
    // Helper methods
    std::chrono::system_clock::time_point getLatestTimestamp(const ConflictInfo& conflict) const;
    std::string getLatestValueByTimestamp(const ConflictInfo& conflict) const;
    std::string createMultiValue(const std::vector<std::string>& values) const;
    
    // Update statistics
    void updateStats(const std::string& resolutionStrategy);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_CONFLICT_RESOLVER_H