#ifndef PHANTOMDB_BTREE_H
#define PHANTOMDB_BTREE_H

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace storage {

template<typename Key, typename Value>
class BTree {
public:
    // Default degree of B-tree (minimum degree)
    static const int DEFAULT_DEGREE = 10;
    
    BTree(int degree = DEFAULT_DEGREE);
    ~BTree();
    
    // Insert a key-value pair
    void insert(const Key& key, const Value& value);
    
    // Search for a key
    bool search(const Key& key, Value& value) const;
    
    // Remove a key
    bool remove(const Key& key);
    
    // Print the tree (for debugging)
    void print() const;
    
private:
    struct Node {
        bool isLeaf;
        std::vector<Key> keys;
        std::vector<Value> values;
        std::vector<std::shared_ptr<Node>> children;
        int keyCount;
        
        Node(bool leaf);
    };
    
    std::shared_ptr<Node> root;
    int degree;  // Minimum degree (defines the range for number of keys)
    
    // Private helper methods
    std::shared_ptr<Node> createNode(bool isLeaf);
    void splitChild(std::shared_ptr<Node> parent, int childIndex, std::shared_ptr<Node> child);
    void insertNonFull(std::shared_ptr<Node> node, const Key& key, const Value& value);
    bool searchRecursive(std::shared_ptr<Node> node, const Key& key, Value& value) const;
    bool removeRecursive(std::shared_ptr<Node> node, const Key& key);
    void removeFromNode(std::shared_ptr<Node> node, int keyIndex);
    void mergeNodes(std::shared_ptr<Node> parent, int childIndex);
    void borrowFromPrev(std::shared_ptr<Node> parent, int childIndex);
    void borrowFromNext(std::shared_ptr<Node> parent, int childIndex);
    Key getPredecessor(std::shared_ptr<Node> node);
    Key getSuccessor(std::shared_ptr<Node> node);
    void printRecursive(std::shared_ptr<Node> node, int depth) const;
};

// Implementation
template<typename Key, typename Value>
BTree<Key, Value>::Node::Node(bool leaf) : isLeaf(leaf), keyCount(0) {
    // Reserve space for efficiency
    keys.reserve(2 * DEFAULT_DEGREE - 1);
    values.reserve(2 * DEFAULT_DEGREE - 1);
    children.reserve(2 * DEFAULT_DEGREE);
}

template<typename Key, typename Value>
BTree<Key, Value>::BTree(int degree) : degree(degree) {
    root = createNode(true);
}

template<typename Key, typename Value>
BTree<Key, Value>::~BTree() {
    // Destructor - smart pointers will handle cleanup
}

template<typename Key, typename Value>
std::shared_ptr<typename BTree<Key, Value>::Node> BTree<Key, Value>::createNode(bool isLeaf) {
    return std::make_shared<Node>(isLeaf);
}

template<typename Key, typename Value>
void BTree<Key, Value>::insert(const Key& key, const Value& value) {
    // If root is full, then tree grows in height
    if (root->keyCount == 2 * degree - 1) {
        std::shared_ptr<Node> newRoot = createNode(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0, root);
        root = newRoot;
    }
    
    insertNonFull(root, key, value);
}

template<typename Key, typename Value>
void BTree<Key, Value>::splitChild(std::shared_ptr<Node> parent, int childIndex, std::shared_ptr<Node> child) {
    // Create a new node which is going to store (degree-1) keys of child
    std::shared_ptr<Node> newNode = createNode(child->isLeaf);
    newNode->keyCount = degree - 1;
    
    // Copy the last (degree-1) keys and values of child to newNode
    newNode->keys.resize(degree - 1);
    newNode->values.resize(degree - 1);
    for (int i = 0; i < degree - 1; i++) {
        newNode->keys[i] = child->keys[i + degree];
        newNode->values[i] = child->values[i + degree];
    }
    
    // Copy the last degree children of child to newNode
    if (!child->isLeaf) {
        newNode->children.resize(degree);
        for (int i = 0; i < degree; i++) {
            newNode->children[i] = child->children[i + degree];
        }
    }
    
    // Reduce the number of keys in child
    child->keyCount = degree - 1;
    child->keys.resize(degree - 1);
    child->values.resize(degree - 1);
    if (!child->isLeaf) {
        child->children.resize(degree);
    }
    
    // Since this node is going to have a new child,
    // create space of new child
    parent->children.resize(parent->children.size() + 1);
    for (int i = parent->keyCount; i >= childIndex + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }
    
    // Link the new child to this node
    parent->children[childIndex + 1] = newNode;
    
    // A key of child will move to this node. Find the location of
    // new key and move all greater keys one space ahead
    parent->keys.resize(parent->keyCount + 1);
    parent->values.resize(parent->keyCount + 1);
    for (int i = parent->keyCount - 1; i >= childIndex; i--) {
        parent->keys[i + 1] = parent->keys[i];
        parent->values[i + 1] = parent->values[i];
    }
    
    // Copy the middle key of child to this node
    parent->keys[childIndex] = child->keys[degree - 1];
    parent->values[childIndex] = child->values[degree - 1];
    parent->keyCount++;
}

template<typename Key, typename Value>
void BTree<Key, Value>::insertNonFull(std::shared_ptr<Node> node, const Key& key, const Value& value) {
    // Initialize index as index of rightmost element
    int i = node->keyCount - 1;
    
    // If this is a leaf node
    if (node->isLeaf) {
        // Make room for new key and value
        node->keys.resize(node->keyCount + 1);
        node->values.resize(node->keyCount + 1);
        
        // Find the location of new key and move all greater keys one space ahead
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        
        // Insert the new key and value at found location
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
        node->keyCount++;
    } else {
        // If this node is not leaf, then find the child which is going to have the new key
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        
        // See if the found child is full
        if (node->children[i + 1]->keyCount == 2 * degree - 1) {
            // If the child is full, then split it
            splitChild(node, i + 1, node->children[i + 1]);
            
            // After split, the middle key of children[i] goes up and
            // children[i] is split into two. See which of the two
            // is going to have the new key
            if (node->keys[i + 1] < key) {
                i++;
            }
        }
        
        insertNonFull(node->children[i + 1], key, value);
    }
}

template<typename Key, typename Value>
bool BTree<Key, Value>::search(const Key& key, Value& value) const {
    return searchRecursive(root, key, value);
}

template<typename Key, typename Value>
bool BTree<Key, Value>::searchRecursive(std::shared_ptr<Node> node, const Key& key, Value& value) const {
    // Find the first key greater than or equal to key
    int i = 0;
    while (i < node->keyCount && key > node->keys[i]) {
        i++;
    }
    
    // If the found key is equal to key, return it
    if (i < node->keyCount && node->keys[i] == key) {
        value = node->values[i];
        return true;
    }
    
    // If key is not found here and this is a leaf node
    if (node->isLeaf) {
        return false;
    }
    
    // Go to the appropriate child
    return searchRecursive(node->children[i], key, value);
}

template<typename Key, typename Value>
bool BTree<Key, Value>::remove(const Key& key) {
    if (!root) {
        return false;
    }
    
    bool result = removeRecursive(root, key);
    
    // If root node has 0 keys, make its first child as the new root
    // if it has a child, otherwise set root as NULL
    if (root->keyCount == 0) {
        if (root->isLeaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
    }
    
    return result;
}

template<typename Key, typename Value>
bool BTree<Key, Value>::removeRecursive(std::shared_ptr<Node> node, const Key& key) {
    int keyIndex = 0;
    while (keyIndex < node->keyCount && node->keys[keyIndex] < key) {
        keyIndex++;
    }
    
    // The key to be removed is present in this node
    if (keyIndex < node->keyCount && node->keys[keyIndex] == key) {
        // If the key is in this node, remove it
        removeFromNode(node, keyIndex);
        return true;
    }
    
    // If this node is a leaf, then the key is not present in tree
    if (node->isLeaf) {
        return false;
    }
    
    // The key is present in the subtree rooted with this child
    bool lastChild = (keyIndex == node->keyCount);
    
    // If the child where the key is supposed to exist has less than degree keys,
    // we fill that child
    if (node->children[keyIndex]->keyCount < degree) {
        // Borrow a key from the left sibling if possible
        if (keyIndex != 0 && node->children[keyIndex - 1]->keyCount >= degree) {
            borrowFromPrev(node, keyIndex);
        }
        // Borrow a key from the right sibling if possible
        else if (keyIndex != node->keyCount && node->children[keyIndex + 1]->keyCount >= degree) {
            borrowFromNext(node, keyIndex);
        }
        // Merge with a sibling if both have less than degree keys
        else {
            if (keyIndex != node->keyCount) {
                mergeNodes(node, keyIndex);
            } else {
                mergeNodes(node, keyIndex - 1);
            }
        }
    }
    
    // Recursively remove from the subtree
    return removeRecursive(lastChild ? node->children[keyIndex - 1] : node->children[keyIndex], key);
}

template<typename Key, typename Value>
void BTree<Key, Value>::removeFromNode(std::shared_ptr<Node> node, int keyIndex) {
    // If the key is in a leaf node
    if (node->isLeaf) {
        // Move all keys after keyIndex one position backward
        for (int i = keyIndex + 1; i < node->keyCount; ++i) {
            node->keys[i - 1] = node->keys[i];
            node->values[i - 1] = node->values[i];
        }
        node->keyCount--;
        node->keys.resize(node->keyCount);
        node->values.resize(node->keyCount);
    } else {
        // If the key is in an internal node
        Key key = node->keys[keyIndex];
        Value value = node->values[keyIndex];
        
        // If the child that precedes key has at least degree keys,
        // find the predecessor of key in the subtree rooted at
        // children[keyIndex]. Replace key by its predecessor and
        // recursively delete the predecessor in children[keyIndex]
        if (node->children[keyIndex]->keyCount >= degree) {
            Key pred = getPredecessor(node->children[keyIndex]);
            node->keys[keyIndex] = pred;
            removeRecursive(node->children[keyIndex], pred);
        }
        // If the child that succeeds key has at least degree keys,
        // find the successor of key in the subtree rooted at
        // children[keyIndex+1]. Replace key by its successor and
        // recursively delete the successor in children[keyIndex+1]
        else if (node->children[keyIndex + 1]->keyCount >= degree) {
            Key succ = getSuccessor(node->children[keyIndex + 1]);
            node->keys[keyIndex] = succ;
            removeRecursive(node->children[keyIndex + 1], succ);
        }
        // If both children[keyIndex] and children[keyIndex+1] have less than degree keys,
        // merge key and all of children[keyIndex+1] into children[keyIndex]
        else {
            mergeNodes(node, keyIndex);
            removeRecursive(node->children[keyIndex], key);
        }
    }
}

template<typename Key, typename Value>
void BTree<Key, Value>::mergeNodes(std::shared_ptr<Node> parent, int childIndex) {
    std::shared_ptr<Node> child = parent->children[childIndex];
    std::shared_ptr<Node> sibling = parent->children[childIndex + 1];
    
    // Pulling a key from the parent for the child
    child->keys.resize(child->keyCount + 1);
    child->values.resize(child->keyCount + 1);
    child->keys[child->keyCount] = parent->keys[childIndex];
    child->values[child->keyCount] = parent->values[childIndex];
    
    // Copying the keys and values from sibling to child
    for (int i = 0; i < sibling->keyCount; ++i) {
        child->keys.resize(child->keyCount + 1);
        child->values.resize(child->keyCount + 1);
        child->keys[child->keyCount] = sibling->keys[i];
        child->values[child->keyCount] = sibling->values[i];
        child->keyCount++;
    }
    
    // Copying the children from sibling to child if it is not a leaf
    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->keyCount; ++i) {
            child->children.push_back(sibling->children[i]);
        }
    }
    
    // Moving all keys after childIndex in the parent one step before
    for (int i = childIndex + 1; i < parent->keyCount; ++i) {
        parent->keys[i - 1] = parent->keys[i];
        parent->values[i - 1] = parent->values[i];
    }
    
    // Moving the child pointers after (childIndex+1) in the parent one step before
    for (int i = childIndex + 2; i <= parent->keyCount; ++i) {
        parent->children[i - 1] = parent->children[i];
    }
    
    // Update key count of child and parent
    child->keyCount += sibling->keyCount + 1;
    parent->keyCount--;
    
    // Resize vectors to match key count
    parent->keys.resize(parent->keyCount);
    parent->values.resize(parent->keyCount);
    parent->children.resize(parent->keyCount + 1);
}

template<typename Key, typename Value>
void BTree<Key, Value>::borrowFromPrev(std::shared_ptr<Node> parent, int childIndex) {
    std::shared_ptr<Node> child = parent->children[childIndex];
    std::shared_ptr<Node> sibling = parent->children[childIndex - 1];
    
    // Shift all keys and values in child one position forward
    child->keys.resize(child->keyCount + 1);
    child->values.resize(child->keyCount + 1);
    for (int i = child->keyCount - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
        child->values[i + 1] = child->values[i];
    }
    
    // If child is not a leaf, shift all children one position forward
    if (!child->isLeaf) {
        child->children.resize(child->children.size() + 1);
        for (int i = child->children.size() - 2; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }
    
    // Set the first key and value of child to the key and value from parent
    child->keys[0] = parent->keys[childIndex - 1];
    child->values[0] = parent->values[childIndex - 1];
    
    // Move the last key and value of sibling to parent
    parent->keys[childIndex - 1] = sibling->keys[sibling->keyCount - 1];
    parent->values[childIndex - 1] = sibling->values[sibling->keyCount - 1];
    
    // Move the last child of sibling to child if sibling is not a leaf
    if (!child->isLeaf) {
        child->children[0] = sibling->children[sibling->keyCount];
    }
    
    // Increase key count of child and decrease key count of sibling
    child->keyCount++;
    sibling->keyCount--;
    
    // Resize vectors to match key count
    sibling->keys.resize(sibling->keyCount);
    sibling->values.resize(sibling->keyCount);
    if (!sibling->isLeaf) {
        sibling->children.resize(sibling->keyCount + 1);
    }
}

template<typename Key, typename Value>
void BTree<Key, Value>::borrowFromNext(std::shared_ptr<Node> parent, int childIndex) {
    std::shared_ptr<Node> child = parent->children[childIndex];
    std::shared_ptr<Node> sibling = parent->children[childIndex + 1];
    
    // Set the last key and value of child to the key and value from parent
    child->keys.resize(child->keyCount + 1);
    child->values.resize(child->keyCount + 1);
    child->keys[child->keyCount] = parent->keys[childIndex];
    child->values[child->keyCount] = parent->values[childIndex];
    
    // Move the first key and value of sibling to parent
    parent->keys[childIndex] = sibling->keys[0];
    parent->values[childIndex] = sibling->values[0];
    
    // Move the first child of sibling to child if sibling is not a leaf
    if (!child->isLeaf) {
        child->children.resize(child->children.size() + 1);
        child->children[child->keyCount + 1] = sibling->children[0];
    }
    
    // Move all keys and values of sibling one step backward
    for (int i = 1; i < sibling->keyCount; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->values[i - 1] = sibling->values[i];
    }
    
    // Move the children of sibling one step backward if sibling is not a leaf
    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->keyCount; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }
    
    // Increase key count of child and decrease key count of sibling
    child->keyCount++;
    sibling->keyCount--;
    
    // Resize vectors to match key count
    sibling->keys.resize(sibling->keyCount);
    sibling->values.resize(sibling->keyCount);
    if (!sibling->isLeaf) {
        sibling->children.resize(sibling->keyCount + 1);
    }
}

template<typename Key, typename Value>
Key BTree<Key, Value>::getPredecessor(std::shared_ptr<Node> node) {
    // Keep moving to the rightmost node until we reach a leaf
    while (!node->isLeaf) {
        node = node->children[node->keyCount];
    }
    
    // Return the last key of the leaf
    return node->keys[node->keyCount - 1];
}

template<typename Key, typename Value>
Key BTree<Key, Value>::getSuccessor(std::shared_ptr<Node> node) {
    // Keep moving to the leftmost node until we reach a leaf
    while (!node->isLeaf) {
        node = node->children[0];
    }
    
    // Return the first key of the leaf
    return node->keys[0];
}

template<typename Key, typename Value>
void BTree<Key, Value>::print() const {
    if (root) {
        printRecursive(root, 0);
    }
}

template<typename Key, typename Value>
void BTree<Key, Value>::printRecursive(std::shared_ptr<Node> node, int depth) const {
    if (node) {
        // Print the current node
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        
        std::cout << "[";
        for (int i = 0; i < node->keyCount; i++) {
            std::cout << node->keys[i];
            if (i < node->keyCount - 1) {
                std::cout << " ";
            }
        }
        std::cout << "]" << std::endl;
        
        // Print children if not a leaf
        if (!node->isLeaf) {
            for (int i = 0; i <= node->keyCount; i++) {
                printRecursive(node->children[i], depth + 1);
            }
        }
    }
}

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_BTREE_H