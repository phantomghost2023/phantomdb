#ifndef PHANTOMDB_HASH_TABLE_H
#define PHANTOMDB_HASH_TABLE_H

#include <vector>
#include <memory>
#include <iostream>
#include <functional>

namespace phantomdb {
namespace storage {

template<typename Key, typename Value>
class HashTable {
public:
    // Default size of hash table
    static const int DEFAULT_SIZE = 100;
    
    HashTable(int size = DEFAULT_SIZE);
    ~HashTable();
    
    // Insert a key-value pair
    void insert(const Key& key, const Value& value);
    
    // Search for a key
    bool search(const Key& key, Value& value) const;
    
    // Remove a key
    bool remove(const Key& key);
    
    // Get the size of the hash table
    int getSize() const;
    
    // Get the number of elements in the hash table
    int getCount() const;
    
private:
    struct HashNode {
        Key key;
        Value value;
        std::shared_ptr<HashNode> next;
        
        HashNode(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };
    
    std::vector<std::shared_ptr<HashNode>> buckets;
    int size;
    int count;
    
    // Hash function
    int hash(const Key& key) const;
};

// Implementation
template<typename Key, typename Value>
HashTable<Key, Value>::HashTable(int size) : size(size), count(0) {
    buckets.resize(size);
}

template<typename Key, typename Value>
HashTable<Key, Value>::~HashTable() {
    // Destructor - smart pointers will handle cleanup
}

template<typename Key, typename Value>
int HashTable<Key, Value>::hash(const Key& key) const {
    // Simple hash function using std::hash
    return std::hash<Key>{}(key) % size;
}

template<typename Key, typename Value>
void HashTable<Key, Value>::insert(const Key& key, const Value& value) {
    int index = hash(key);
    
    // Check if key already exists
    std::shared_ptr<HashNode> current = buckets[index];
    while (current) {
        if (current->key == key) {
            // Update existing value
            current->value = value;
            return;
        }
        current = current->next;
    }
    
    // Key doesn't exist, create new node
    std::shared_ptr<HashNode> newNode = std::make_shared<HashNode>(key, value);
    newNode->next = buckets[index];
    buckets[index] = newNode;
    count++;
}

template<typename Key, typename Value>
bool HashTable<Key, Value>::search(const Key& key, Value& value) const {
    int index = hash(key);
    
    std::shared_ptr<HashNode> current = buckets[index];
    while (current) {
        if (current->key == key) {
            value = current->value;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

template<typename Key, typename Value>
bool HashTable<Key, Value>::remove(const Key& key) {
    int index = hash(key);
    
    std::shared_ptr<HashNode> current = buckets[index];
    std::shared_ptr<HashNode> prev = nullptr;
    
    while (current) {
        if (current->key == key) {
            if (prev) {
                prev->next = current->next;
            } else {
                buckets[index] = current->next;
            }
            count--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

template<typename Key, typename Value>
int HashTable<Key, Value>::getSize() const {
    return size;
}

template<typename Key, typename Value>
int HashTable<Key, Value>::getCount() const {
    return count;
}

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_HASH_TABLE_H