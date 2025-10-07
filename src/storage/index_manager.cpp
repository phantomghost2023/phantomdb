#include "index_manager.h"
#include "btree.h"
#include "hash_table.h"
#include "lsm_tree.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>

namespace phantomdb {
namespace storage {

class IndexManager::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    bool createIndex(const std::string& tableName, const std::string& columnName, IndexType type) {
        std::string indexName = tableName + "_" + columnName + "_idx";
        
        // Check if index already exists
        if (indexes.find(indexName) != indexes.end()) {
            std::cerr << "Index already exists: " << indexName << std::endl;
            return false;
        }
        
        // Create a new index based on the type
        switch (type) {
            case IndexType::B_TREE:
                {
                    // Create a B-tree index
                    auto btreeIndex = std::make_unique<BTree<int, std::string>>();
                    btreeIndexes[indexName] = std::move(btreeIndex);
                    indexes[indexName] = {tableName, columnName, type};
                    std::cout << "Created B-tree index: " << indexName << std::endl;
                }
                break;
            case IndexType::HASH:
                {
                    // Create a Hash table index
                    auto hashIndex = std::make_unique<HashTable<int, std::string>>();
                    hashIndexes[indexName] = std::move(hashIndex);
                    indexes[indexName] = {tableName, columnName, type};
                    std::cout << "Created Hash index: " << indexName << std::endl;
                }
                break;
            case IndexType::LSM_TREE:
                {
                    // Create an LSM-tree index
                    auto lsmTreeIndex = std::make_unique<LSMTREE<int, std::string>>();
                    lsmTreeIndexes[indexName] = std::move(lsmTreeIndex);
                    indexes[indexName] = {tableName, columnName, type};
                    std::cout << "Created LSM-tree index: " << indexName << std::endl;
                }
                break;
        }
        
        return true;
    }
    
    bool dropIndex(const std::string& indexName) {
        // Check if index exists
        if (indexes.find(indexName) == indexes.end()) {
            std::cerr << "Index not found: " << indexName << std::endl;
            return false;
        }
        
        // Remove from appropriate container based on type
        IndexInfo& info = indexes[indexName];
        switch (info.type) {
            case IndexType::B_TREE:
                btreeIndexes.erase(indexName);
                break;
            case IndexType::HASH:
                hashIndexes.erase(indexName);
                break;
            case IndexType::LSM_TREE:
                lsmTreeIndexes.erase(indexName);
                break;
        }
        
        // Remove from main index tracking
        indexes.erase(indexName);
        std::cout << "Dropped index: " << indexName << std::endl;
        return true;
    }
    
    IndexType getIndexType(const std::string& indexName) const {
        auto it = indexes.find(indexName);
        if (it != indexes.end()) {
            return it->second.type;
        }
        return IndexType::B_TREE; // Default type
    }
    
    bool insertIntoIndex(const std::string& indexName, int key, const std::string& value) {
        auto it = indexes.find(indexName);
        if (it == indexes.end()) {
            std::cerr << "Index not found: " << indexName << std::endl;
            return false;
        }
        
        // Insert into appropriate index based on type
        switch (it->second.type) {
            case IndexType::B_TREE:
                {
                    auto btreeIt = btreeIndexes.find(indexName);
                    if (btreeIt != btreeIndexes.end()) {
                        btreeIt->second->insert(key, value);
                        return true;
                    }
                }
                break;
            case IndexType::HASH:
                {
                    auto hashIt = hashIndexes.find(indexName);
                    if (hashIt != hashIndexes.end()) {
                        hashIt->second->insert(key, value);
                        return true;
                    }
                }
                break;
            case IndexType::LSM_TREE:
                {
                    auto lsmTreeIt = lsmTreeIndexes.find(indexName);
                    if (lsmTreeIt != lsmTreeIndexes.end()) {
                        lsmTreeIt->second->insert(key, value);
                        return true;
                    }
                }
                break;
        }
        
        std::cerr << "Index type does not support insertion or index not properly initialized: " << indexName << std::endl;
        return false;
    }
    
    bool searchInIndex(const std::string& indexName, int key, std::string& value) const {
        auto it = indexes.find(indexName);
        if (it == indexes.end()) {
            std::cerr << "Index not found: " << indexName << std::endl;
            return false;
        }
        
        // Search in appropriate index based on type
        switch (it->second.type) {
            case IndexType::B_TREE:
                {
                    auto btreeIt = btreeIndexes.find(indexName);
                    if (btreeIt != btreeIndexes.end()) {
                        return btreeIt->second->search(key, value);
                    }
                }
                break;
            case IndexType::HASH:
                {
                    auto hashIt = hashIndexes.find(indexName);
                    if (hashIt != hashIndexes.end()) {
                        return hashIt->second->search(key, value);
                    }
                }
                break;
            case IndexType::LSM_TREE:
                {
                    auto lsmTreeIt = lsmTreeIndexes.find(indexName);
                    if (lsmTreeIt != lsmTreeIndexes.end()) {
                        return lsmTreeIt->second->search(key, value);
                    }
                }
                break;
        }
        
        std::cerr << "Index type does not support search or index not properly initialized: " << indexName << std::endl;
        return false;
    }
    
    void listIndexes() const {
        std::cout << "Current indexes:" << std::endl;
        for (const auto& pair : indexes) {
            std::cout << "  " << pair.first << " (" << getIndexTypeName(pair.second.type) 
                      << ") on " << pair.second.tableName 
                      << "(" << pair.second.columnName << ")" << std::endl;
        }
        
        std::cout << "Auto-indexing enabled for tables:" << std::endl;
        for (const auto& pair : autoIndexConfig) {
            std::cout << "  " << pair.first << " (" << getIndexTypeName(pair.second.type) << ")" << std::endl;
            std::cout << "    Columns: ";
            for (const auto& col : pair.second.columns) {
                std::cout << col << " ";
            }
            std::cout << std::endl;
        }
    }
    
    void enableAutoIndexing(const std::string& tableName, const std::vector<std::string>& columns, IndexType type) {
        autoIndexConfig[tableName] = {columns, type};
        std::cout << "Auto-indexing enabled for table: " << tableName << std::endl;
    }
    
    void disableAutoIndexing(const std::string& tableName) {
        auto it = autoIndexConfig.find(tableName);
        if (it != autoIndexConfig.end()) {
            autoIndexConfig.erase(it);
            std::cout << "Auto-indexing disabled for table: " << tableName << std::endl;
        }
    }
    
    bool isAutoIndexingEnabled(const std::string& tableName) const {
        return autoIndexConfig.find(tableName) != autoIndexConfig.end();
    }
    
    IndexType getAutoIndexType(const std::string& tableName) const {
        auto it = autoIndexConfig.find(tableName);
        if (it != autoIndexConfig.end()) {
            return it->second.type;
        }
        return IndexType::B_TREE; // Default type
    }
    
    const std::vector<std::string>& getAutoIndexColumns(const std::string& tableName) const {
        static const std::vector<std::string> empty; // Return empty vector if not found
        auto it = autoIndexConfig.find(tableName);
        if (it != autoIndexConfig.end()) {
            return it->second.columns;
        }
        return empty;
    }
    
    // Create automatic indexes for a table based on its configuration
    void createAutoIndexes(const std::string& tableName) {
        auto it = autoIndexConfig.find(tableName);
        if (it != autoIndexConfig.end()) {
            const auto& config = it->second;
            for (const auto& column : config.columns) {
                createIndex(tableName, column, config.type);
            }
        }
    }
    
private:
    struct IndexInfo {
        std::string tableName;
        std::string columnName;
        IndexType type;
    };
    
    struct AutoIndexConfig {
        std::vector<std::string> columns;
        IndexType type;
    };
    
    std::string getIndexTypeName(IndexType type) const {
        switch (type) {
            case IndexType::B_TREE: return "B-tree";
            case IndexType::HASH: return "Hash";
            case IndexType::LSM_TREE: return "LSM-tree";
            default: return "Unknown";
        }
    }
    
    std::unordered_map<std::string, IndexInfo> indexes;
    std::unordered_map<std::string, std::unique_ptr<BTree<int, std::string>>> btreeIndexes;
    std::unordered_map<std::string, std::unique_ptr<HashTable<int, std::string>>> hashIndexes;
    std::unordered_map<std::string, std::unique_ptr<LSMTREE<int, std::string>>> lsmTreeIndexes;
    
    // Auto-indexing configuration
    std::unordered_map<std::string, AutoIndexConfig> autoIndexConfig;
};

IndexManager::IndexManager() : pImpl(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Index Manager initialized" << std::endl;
}

IndexManager::~IndexManager() {
    std::cout << "PhantomDB Index Manager shutdown" << std::endl;
}

bool IndexManager::initialize() {
    std::cout << "Initializing PhantomDB Index Manager" << std::endl;
    return true;
}

void IndexManager::shutdown() {
    std::cout << "Shutting down PhantomDB Index Manager" << std::endl;
}

bool IndexManager::createIndex(const std::string& tableName, const std::string& columnName, IndexType type) {
    return pImpl->createIndex(tableName, columnName, type);
}

bool IndexManager::dropIndex(const std::string& indexName) {
    return pImpl->dropIndex(indexName);
}

IndexType IndexManager::getIndexType(const std::string& indexName) const {
    return pImpl->getIndexType(indexName);
}

bool IndexManager::insertIntoIndex(const std::string& indexName, int key, const std::string& value) {
    return pImpl->insertIntoIndex(indexName, key, value);
}

bool IndexManager::searchInIndex(const std::string& indexName, int key, std::string& value) const {
    return pImpl->searchInIndex(indexName, key, value);
}

void IndexManager::listIndexes() const {
    pImpl->listIndexes();
}

void IndexManager::enableAutoIndexing(const std::string& tableName, const std::vector<std::string>& columns, IndexType type) {
    pImpl->enableAutoIndexing(tableName, columns, type);
}

void IndexManager::disableAutoIndexing(const std::string& tableName) {
    pImpl->disableAutoIndexing(tableName);
}

bool IndexManager::isAutoIndexingEnabled(const std::string& tableName) const {
    return pImpl->isAutoIndexingEnabled(tableName);
}

IndexType IndexManager::getAutoIndexType(const std::string& tableName) const {
    return pImpl->getAutoIndexType(tableName);
}

const std::vector<std::string>& IndexManager::getAutoIndexColumns(const std::string& tableName) const {
    return pImpl->getAutoIndexColumns(tableName);
}

} // namespace storage
} // namespace phantomdb