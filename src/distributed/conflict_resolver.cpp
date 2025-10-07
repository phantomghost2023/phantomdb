#include "conflict_resolver.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace phantomdb {
namespace distributed {

ConflictResolver::ConflictResolver(ConflictResolutionStrategy strategy)
    : strategy_(strategy) {
    std::cout << "Creating ConflictResolver with strategy " << static_cast<int>(strategy) << std::endl;
}

ConflictResolver::~ConflictResolver() {
    std::cout << "Destroying ConflictResolver" << std::endl;
}

bool ConflictResolver::initialize() {
    std::cout << "Initializing ConflictResolver" << std::endl;
    
    // Reset statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_ = ResolutionStats();
    }
    
    std::cout << "ConflictResolver initialized successfully" << std::endl;
    return true;
}

void ConflictResolver::shutdown() {
    std::cout << "Shutting down ConflictResolver" << std::endl;
    
    // Clear custom resolver
    {
        std::lock_guard<std::mutex> lock(resolverMutex_);
        customResolver_ = nullptr;
    }
    
    std::cout << "ConflictResolver shutdown completed" << std::endl;
}

void ConflictResolver::setResolutionStrategy(ConflictResolutionStrategy strategy) {
    std::lock_guard<std::mutex> lock(resolverMutex_);
    strategy_ = strategy;
    std::cout << "Conflict resolution strategy set to " << static_cast<int>(strategy) << std::endl;
}

ConflictResolutionStrategy ConflictResolver::getResolutionStrategy() const {
    std::lock_guard<std::mutex> lock(resolverMutex_);
    return strategy_;
}

void ConflictResolver::registerCustomResolver(const CustomConflictResolver& resolver) {
    std::lock_guard<std::mutex> lock(resolverMutex_);
    customResolver_ = resolver;
    std::cout << "Custom conflict resolver registered" << std::endl;
}

ResolvedConflict ConflictResolver::resolveConflict(const ConflictInfo& conflict) {
    std::lock_guard<std::mutex> lock(resolverMutex_);
    
    std::cout << "Resolving conflict for key " << conflict.key << std::endl;
    
    switch (strategy_) {
        case ConflictResolutionStrategy::LATEST_TIMESTAMP:
            return resolveByLatestTimestamp(conflict);
        case ConflictResolutionStrategy::LWW_REGISTER:
            return resolveByLWWRegister(conflict);
        case ConflictResolutionStrategy::MULTI_VALUE:
            return resolveByMultiValue(conflict);
        case ConflictResolutionStrategy::CUSTOM:
            return resolveByCustom(conflict);
        default:
            return resolveByLatestTimestamp(conflict);
    }
}

std::vector<ResolvedConflict> ConflictResolver::resolveConflicts(const std::vector<ConflictInfo>& conflicts) {
    std::vector<ResolvedConflict> resolvedConflicts;
    resolvedConflicts.reserve(conflicts.size());
    
    for (const auto& conflict : conflicts) {
        resolvedConflicts.push_back(resolveConflict(conflict));
    }
    
    return resolvedConflicts;
}

ConflictResolver::ResolutionStats ConflictResolver::getResolutionStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

ResolvedConflict ConflictResolver::resolveByLatestTimestamp(const ConflictInfo& conflict) {
    std::cout << "Resolving conflict for key " << conflict.key << " using latest timestamp strategy" << std::endl;
    
    updateStats("LATEST_TIMESTAMP");
    
    if (conflict.conflictingValues.empty()) {
        return ResolvedConflict(conflict.key, "", "LATEST_TIMESTAMP");
    }
    
    if (conflict.conflictingValues.size() == 1) {
        return ResolvedConflict(conflict.key, conflict.conflictingValues[0], "LATEST_TIMESTAMP");
    }
    
    std::string resolvedValue = getLatestValueByTimestamp(conflict);
    return ResolvedConflict(conflict.key, resolvedValue, "LATEST_TIMESTAMP");
}

ResolvedConflict ConflictResolver::resolveByLWWRegister(const ConflictInfo& conflict) {
    std::cout << "Resolving conflict for key " << conflict.key << " using LWW register strategy" << std::endl;
    
    updateStats("LWW_REGISTER");
    
    if (conflict.conflictingValues.empty()) {
        return ResolvedConflict(conflict.key, "", "LWW_REGISTER");
    }
    
    if (conflict.conflictingValues.size() == 1) {
        return ResolvedConflict(conflict.key, conflict.conflictingValues[0], "LWW_REGISTER");
    }
    
    // For LWW (Last Writer Wins), we use the latest timestamp
    std::string resolvedValue = getLatestValueByTimestamp(conflict);
    return ResolvedConflict(conflict.key, resolvedValue, "LWW_REGISTER");
}

ResolvedConflict ConflictResolver::resolveByMultiValue(const ConflictInfo& conflict) {
    std::cout << "Resolving conflict for key " << conflict.key << " using multi-value strategy" << std::endl;
    
    updateStats("MULTI_VALUE");
    
    if (conflict.conflictingValues.empty()) {
        return ResolvedConflict(conflict.key, "", "MULTI_VALUE");
    }
    
    if (conflict.conflictingValues.size() == 1) {
        return ResolvedConflict(conflict.key, conflict.conflictingValues[0], "MULTI_VALUE");
    }
    
    // Create a multi-value representation
    std::string resolvedValue = createMultiValue(conflict.conflictingValues);
    return ResolvedConflict(conflict.key, resolvedValue, "MULTI_VALUE");
}

ResolvedConflict ConflictResolver::resolveByCustom(const ConflictInfo& conflict) {
    std::cout << "Resolving conflict for key " << conflict.key << " using custom strategy" << std::endl;
    
    updateStats("CUSTOM");
    
    // Check if custom resolver is registered
    if (!customResolver_) {
        std::cout << "No custom resolver registered, falling back to latest timestamp strategy" << std::endl;
        return resolveByLatestTimestamp(conflict);
    }
    
    try {
        std::string resolvedValue = customResolver_(conflict);
        return ResolvedConflict(conflict.key, resolvedValue, "CUSTOM");
    } catch (const std::exception& e) {
        std::cout << "Exception in custom conflict resolver: " << e.what() << std::endl;
        std::cout << "Falling back to latest timestamp strategy" << std::endl;
        return resolveByLatestTimestamp(conflict);
    }
}

std::chrono::system_clock::time_point ConflictResolver::getLatestTimestamp(const ConflictInfo& conflict) const {
    if (conflict.timestamps.empty()) {
        return std::chrono::system_clock::now();
    }
    
    return *std::max_element(conflict.timestamps.begin(), conflict.timestamps.end());
}

std::string ConflictResolver::getLatestValueByTimestamp(const ConflictInfo& conflict) const {
    if (conflict.conflictingValues.empty()) {
        return "";
    }
    
    if (conflict.conflictingValues.size() == 1) {
        return conflict.conflictingValues[0];
    }
    
    if (conflict.timestamps.empty() || conflict.timestamps.size() != conflict.conflictingValues.size()) {
        // If timestamps are not available or don't match values, return the first value
        return conflict.conflictingValues[0];
    }
    
    // Find the value with the latest timestamp
    auto maxTimestampIt = std::max_element(conflict.timestamps.begin(), conflict.timestamps.end());
    size_t index = std::distance(conflict.timestamps.begin(), maxTimestampIt);
    
    return conflict.conflictingValues[index];
}

std::string ConflictResolver::createMultiValue(const std::vector<std::string>& values) const {
    if (values.empty()) {
        return "";
    }
    
    if (values.size() == 1) {
        return values[0];
    }
    
    // Create a JSON-like multi-value representation
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            oss << ",";
        }
        oss << "\"" << values[i] << "\"";
    }
    
    oss << "]";
    
    return oss.str();
}

void ConflictResolver::updateStats(const std::string& resolutionStrategy) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    stats_.totalConflicts++;
    
    if (resolutionStrategy == "LATEST_TIMESTAMP") {
        stats_.resolvedByTimestamp++;
    } else if (resolutionStrategy == "LWW_REGISTER") {
        stats_.resolvedByLWW++;
    } else if (resolutionStrategy == "MULTI_VALUE") {
        stats_.resolvedByMultiValue++;
    } else if (resolutionStrategy == "CUSTOM") {
        stats_.resolvedByCustom++;
    }
}

} // namespace distributed
} // namespace phantomdb