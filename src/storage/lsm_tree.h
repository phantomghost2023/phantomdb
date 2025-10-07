#ifndef PHANTOMDB_LSM_TREE_H
#define PHANTOMDB_LSM_TREE_H

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <deque>
#include <mutex>

namespace phantomdb {
namespace storage {

template<typename Key, typename Value>
class LSMTREE {
public:
    static const int DEFAULT_MEMTABLE_SIZE = 1000;
    
    explicit LSMTREE(int memtableSize = DEFAULT_MEMTABLE_SIZE);
    ~LSMTREE();
    
    void insert(const Key& key, const Value& value);
    bool search(const Key& key, Value& value) const;
    bool remove(const Key& key);
    int getSize() const;
    int getCount() const;
    
private:
    struct SSTable {
        std::map<Key, Value> data;
        int level;
    };
    
    mutable std::mutex mtx_;
    std::map<Key, Value> memtable_;  // In-memory table
    std::deque<std::shared_ptr<SSTable>> sstables_;  // Sorted String Tables on disk
    int memtableSize_;
    int count_;
    mutable int size_;
    
    void flushMemtable();
    std::shared_ptr<SSTable> mergeSSTables(const std::shared_ptr<SSTable>& sst1, 
                                           const std::shared_ptr<SSTable>& sst2) const;
    void compact();
};

// Implementation
template<typename Key, typename Value>
LSMTREE<Key, Value>::LSMTREE(int memtableSize) : memtableSize_(memtableSize), count_(0), size_(0) {}

template<typename Key, typename Value>
LSMTREE<Key, Value>::~LSMTREE() {}

template<typename Key, typename Value>
void LSMTREE<Key, Value>::insert(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(mtx_);
    memtable_[key] = value;
    count_++;
    size_++;
    
    // Check if memtable needs to be flushed
    if (memtable_.size() >= memtableSize_) {
        flushMemtable();
    }
}

template<typename Key, typename Value>
bool LSMTREE<Key, Value>::search(const Key& key, Value& value) const {
    std::lock_guard<std::mutex> lock(mtx_);
    
    // First check memtable
    auto it = memtable_.find(key);
    if (it != memtable_.end()) {
        value = it->second;
        return true;
    }
    
    // Then check SSTables in reverse order (newest first)
    for (auto rit = sstables_.rbegin(); rit != sstables_.rend(); ++rit) {
        auto sst_it = (*rit)->data.find(key);
        if (sst_it != (*rit)->data.end()) {
            value = sst_it->second;
            return true;
        }
    }
    
    return false;
}

template<typename Key, typename Value>
bool LSMTREE<Key, Value>::remove(const Key& key) {
    std::lock_guard<std::mutex> lock(mtx_);
    bool removed = false;
    
    // Remove from memtable
    auto it = memtable_.find(key);
    if (it != memtable_.end()) {
        memtable_.erase(it);
        removed = true;
        count_--;
    }
    
    // Mark as deleted in SSTables (in a real implementation, we would add tombstones)
    for (auto& sst : sstables_) {
        auto sst_it = sst->data.find(key);
        if (sst_it != sst->data.end()) {
            // In a real implementation, we would mark with a tombstone
            // For simplicity, we'll just note that it was found
            removed = true;
        }
    }
    
    return removed;
}

template<typename Key, typename Value>
int LSMTREE<Key, Value>::getSize() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return size_;
}

template<typename Key, typename Value>
int LSMTREE<Key, Value>::getCount() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return count_;
}

template<typename Key, typename Value>
void LSMTREE<Key, Value>::flushMemtable() {
    // Create a new SSTable from current memtable
    auto sstable = std::make_shared<SSTable>();
    sstable->data = memtable_;
    sstable->level = 0;
    
    // Clear memtable
    memtable_.clear();
    
    // Add to SSTables
    sstables_.push_back(sstable);
    
    // Trigger compaction if needed
    compact();
}

template<typename Key, typename Value>
std::shared_ptr<typename LSMTREE<Key, Value>::SSTable> 
LSMTREE<Key, Value>::mergeSSTables(const std::shared_ptr<SSTable>& sst1, 
                                  const std::shared_ptr<SSTable>& sst2) const {
    auto merged = std::make_shared<SSTable>();
    merged->level = std::max(sst1->level, sst2->level) + 1;
    
    // Merge the two SSTables
    auto it1 = sst1->data.begin();
    auto it2 = sst2->data.begin();
    
    while (it1 != sst1->data.end() && it2 != sst2->data.end()) {
        if (it1->first <= it2->first) {
            merged->data[it1->first] = it1->second;
            ++it1;
        } else {
            merged->data[it2->first] = it2->second;
            ++it2;
        }
    }
    
    // Add remaining elements
    while (it1 != sst1->data.end()) {
        merged->data[it1->first] = it1->second;
        ++it1;
    }
    
    while (it2 != sst2->data.end()) {
        merged->data[it2->first] = it2->second;
        ++it2;
    }
    
    return merged;
}

template<typename Key, typename Value>
void LSMTREE<Key, Value>::compact() {
    // Simple compaction strategy: merge SSTables when we have too many
    const int COMPACTION_THRESHOLD = 4;
    
    if (sstables_.size() >= COMPACTION_THRESHOLD) {
        // Merge the oldest SSTables
        auto sst1 = sstables_.front();
        sstables_.pop_front();
        auto sst2 = sstables_.front();
        sstables_.pop_front();
        
        auto merged = mergeSSTables(sst1, sst2);
        sstables_.push_back(merged);
    }
}

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_LSM_TREE_H