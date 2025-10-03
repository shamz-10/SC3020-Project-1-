#include "bptree.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <cmath>

/**
 * B+ Tree Constructor
 * 
 * Initializes a B+ tree with the specified filename and calculates
 * the optimal order based on the node size. The order determines
 * how many keys each node can hold, which affects tree performance.
 * 
 * @param fname Path to the B+ tree file
 */
BPTree::BPTree(const std::string& fname) : filename(fname), root_id(-1), next_node_id(0), index_nodes_accessed(0), total_index_node_ios(0) {
    // Use MAX_KEYS as the order (101 in this implementation)
    // This ensures optimal node utilization
    order = BPTreeNode::MAX_KEYS;
    if (order < 3) order = 3; // Minimum order for B+ tree validity
}

BPTree::~BPTree() {
    close();
}

bool BPTree::open() {
    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        // Create new file if it doesn't exist
        file.open(filename, std::ios::binary | std::ios::out);
        file.close();
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
        
        // Initialize root
        BPTreeNode root;
        root_id = createNode(root);
        
        // Write metadata
        writeMetadata();
    } else {
        // Load existing metadata
        readMetadata();
    }
    return file.is_open();
}

void BPTree::close() {
    if (file.is_open()) {
        // Write metadata before closing
        writeMetadata();
        file.close();
    }
}

bool BPTree::isOpen() const {
    return file.is_open();
}

bool BPTree::writeNode(int node_id, const BPTreeNode& node) {
    if (!file.is_open()) return false;
    
    // Skip metadata (8 bytes) and write node
    file.seekp(8 + node_id * sizeof(BPTreeNode));
    file.write(reinterpret_cast<const char*>(&node), sizeof(BPTreeNode));
    
    // Increment I/O counter for performance measurement
    index_nodes_accessed++;
    total_index_node_ios++;
    unique_index_nodes.insert(node_id);
    
    return file.good();
}

bool BPTree::readNode(int node_id, BPTreeNode& node) const {
    if (!file.is_open()) return false;
    
    // Skip metadata (8 bytes) and read node
    file.seekg(8 + node_id * sizeof(BPTreeNode));
    file.read(reinterpret_cast<char*>(&node), sizeof(BPTreeNode));
    
    // Increment I/O counter for performance measurement
    index_nodes_accessed++;
    total_index_node_ios++;
    unique_index_nodes.insert(node_id);
    
    return file.good();
}

int BPTree::createNode(const BPTreeNode& node) {
    if (!file.is_open()) return -1;
    
    int node_id = next_node_id++;
    if (writeNode(node_id, node)) {
        return node_id;
    }
    return -1;
}

int BPTree::findLeaf(float key) {
    if (root_id == -1) return -1;
    
    int current = root_id;
    BPTreeNode node;
    
    while (!readNode(current, node) || node.is_leaf == false) {
        if (!readNode(current, node)) return -1;
        
        int i = 0;
        while (i < node.num_keys && key >= node.keys[i]) {
            i++;
        }
        current = node.children[i];
    }
    
    return current;
}

void BPTree::insertIntoLeaf(int leaf_id, float key, const RecordPointer& ptr) {
    BPTreeNode leaf;
    if (!readNode(leaf_id, leaf)) return;
    
    // Find insertion position
    int i = 0;
    while (i < leaf.num_keys && leaf.keys[i] < key) {
        i++;
    }
    
    // Shift elements to make room
    for (int j = leaf.num_keys; j > i; j--) {
        leaf.keys[j] = leaf.keys[j-1];
        leaf.children[j] = leaf.children[j-1];
    }
    
    // Insert new key and pointer
    leaf.keys[i] = key;
    leaf.children[i] = ptr.block_id * 10000 + ptr.record_index; // Encode pointer
    leaf.num_keys++;
    
    writeNode(leaf_id, leaf);
}

void BPTree::splitLeaf(int leaf_id) {
    BPTreeNode leaf;
    if (!readNode(leaf_id, leaf)) return;
    
    // Create new leaf
    BPTreeNode new_leaf;
    new_leaf.is_leaf = true;
    new_leaf.next_leaf = leaf.next_leaf;
    new_leaf.parent = leaf.parent;
    
    int new_leaf_id = createNode(new_leaf);
    
    // Update original leaf
    leaf.next_leaf = new_leaf_id;
    
    // Redistribute keys
    int mid = leaf.num_keys / 2;
    for (int i = mid; i < leaf.num_keys; i++) {
        new_leaf.keys[new_leaf.num_keys] = leaf.keys[i];
        new_leaf.children[new_leaf.num_keys] = leaf.children[i];
        new_leaf.num_keys++;
    }
    leaf.num_keys = mid;
    
    // Write both nodes
    writeNode(leaf_id, leaf);
    writeNode(new_leaf_id, new_leaf);
    
    // Insert into parent
    float promote_key = new_leaf.keys[0];
    insertIntoParent(leaf_id, promote_key, new_leaf_id);
}

void BPTree::insertIntoParent(int left_id, float key, int right_id) {
    if (root_id == left_id) {
        // Create new root
        BPTreeNode new_root;
        new_root.is_leaf = false;
        new_root.keys[0] = key;
        new_root.children[0] = left_id;
        new_root.children[1] = right_id;
        new_root.num_keys = 1;
        
        root_id = createNode(new_root);
        
        // Update children's parent pointers
        BPTreeNode left_node, right_node;
        readNode(left_id, left_node);
        readNode(right_id, right_node);
        
        left_node.parent = root_id;
        right_node.parent = root_id;
        
        writeNode(left_id, left_node);
        writeNode(right_id, right_node);
    } else {
        // Insert into existing parent
        BPTreeNode parent;
        readNode(left_id, parent);
        int parent_id = parent.parent;
        readNode(parent_id, parent);
        
        // Find insertion position
        int i = 0;
        while (i < parent.num_keys && parent.children[i] != left_id) {
            i++;
        }
        
        // Shift elements
        for (int j = parent.num_keys; j > i; j--) {
            parent.keys[j] = parent.keys[j-1];
            parent.children[j+1] = parent.children[j];
        }
        
        parent.keys[i] = key;
        parent.children[i+1] = right_id;
        parent.num_keys++;
        
        writeNode(parent_id, parent);
        
        // Check if parent needs splitting
        if (parent.num_keys > order) {
            splitInternal(parent_id);
        }
    }
}

void BPTree::splitInternal(int node_id) {
    BPTreeNode node;
    if (!readNode(node_id, node)) return;
    
    // Create new internal node
    BPTreeNode new_node;
    new_node.is_leaf = false;
    new_node.parent = node.parent;
    
    int new_node_id = createNode(new_node);
    
    // Redistribute keys and children
    int mid = node.num_keys / 2;
    float promote_key = node.keys[mid];
    
    for (int i = mid + 1; i < node.num_keys; i++) {
        new_node.keys[new_node.num_keys] = node.keys[i];
        new_node.children[new_node.num_keys] = node.children[i];
        new_node.num_keys++;
    }
    new_node.children[new_node.num_keys] = node.children[node.num_keys];
    
    node.num_keys = mid;
    
    // Write both nodes
    writeNode(node_id, node);
    writeNode(new_node_id, new_node);
    
    // Insert into parent
    insertIntoParent(node_id, promote_key, new_node_id);
}

bool BPTree::insert(float key, const RecordPointer& ptr) {
    if (root_id == -1) {
        // Create root leaf
        BPTreeNode root;
        root.is_leaf = true;
        root.keys[0] = key;
        root.children[0] = ptr.block_id * 10000 + ptr.record_index;
        root.num_keys = 1;
        root_id = createNode(root);
        return true;
    }
    
    int leaf_id = findLeaf(key);
    if (leaf_id == -1) return false;
    
    BPTreeNode leaf;
    if (!readNode(leaf_id, leaf)) return false;
    
    // Check if leaf has space
    if (leaf.num_keys < order) {
        insertIntoLeaf(leaf_id, key, ptr);
    } else {
        // Leaf is full, need to split
        insertIntoLeaf(leaf_id, key, ptr);
        splitLeaf(leaf_id);
    }
    
    return true;
}

std::vector<RecordPointer> BPTree::search(float key) {
    std::vector<RecordPointer> results;
    
    int leaf_id = findLeaf(key);
    if (leaf_id == -1) return results;
    
    BPTreeNode leaf;
    if (!readNode(leaf_id, leaf)) return results;
    
    // Search in leaf node
    for (int i = 0; i < leaf.num_keys; i++) {
        if (leaf.keys[i] == key) {
            int encoded_ptr = leaf.children[i];
            int block_id = encoded_ptr / 10000;
            int record_index = encoded_ptr % 10000;
            results.push_back(RecordPointer(block_id, record_index));
        }
    }
    
    return results;
}

/**
 * Range Search in B+ Tree
 * 
 * Searches for all records with key values in the specified range [min_key, max_key].
 * This method efficiently traverses the B+ tree to find all matching records.
 * 
 * Algorithm:
 * 1. Find the leaf node that should contain min_key
 * 2. Scan through leaf nodes sequentially
 * 3. Collect all keys in the range
 * 4. Decode record pointers and return results
 * 
 * Time Complexity: O(log n + k) where k is the number of results
 * Space Complexity: O(k) for storing results
 * 
 * @param min_key Minimum key value (inclusive)
 * @param max_key Maximum key value (inclusive)
 * @return Vector of record pointers in the range
 */
std::vector<RecordPointer> BPTree::rangeSearch(float min_key, float max_key) {
    std::vector<RecordPointer> results;
    
    // Step 1: Find the leaf node that should contain min_key
    int leaf_id = findLeaf(min_key);
    if (leaf_id == -1) return results; // Tree is empty
    
    // Step 2: Scan through leaf nodes sequentially
    BPTreeNode leaf;
    while (leaf_id != -1 && readNode(leaf_id, leaf)) {
        // Step 3: Check each key in the current leaf
        for (int i = 0; i < leaf.num_keys; i++) {
            if (leaf.keys[i] >= min_key && leaf.keys[i] <= max_key) {
                // Key is in range, decode the record pointer
                int encoded_ptr = leaf.children[i];
                int block_id = encoded_ptr / 10000;      // Extract block ID
                int record_index = encoded_ptr % 10000;  // Extract record index
                results.push_back(RecordPointer(block_id, record_index));
            }
        }
        
        // Step 4: Stop if we've gone past the maximum key
        if (leaf.keys[leaf.num_keys - 1] > max_key) break;
        // Move to the next leaf node
        leaf_id = leaf.next_leaf;
    }
    
    return results;
}

/**
 * Bulk Load B+ Tree
 * 
 * Efficiently constructs a B+ tree from sorted data using the bulk loading
 * algorithm. This method is much faster than individual insertions because
 * it builds the tree bottom-up in a single pass.
 * 
 * Algorithm:
 * 1. Sort all data by key value
 * 2. Create leaf nodes and fill them with sorted data
 * 3. Link leaf nodes together for range queries
 * 4. Build internal nodes bottom-up
 * 5. Set root node
 * 
 * Time Complexity: O(n log n) for sorting + O(n) for tree construction
 * Space Complexity: O(n) for storing the tree
 * 
 * @param data Vector of key-value pairs to insert
 * @return true if bulk loading was successful
 */
bool BPTree::bulkLoad(const std::vector<std::pair<float, RecordPointer>>& data) {
    if (data.empty()) return false;
    
    // Step 1: Sort data by key for efficient bulk loading
    // This ensures that records are stored in sorted order in leaf nodes
    std::vector<std::pair<float, RecordPointer>> sorted_data = data;
    std::sort(sorted_data.begin(), sorted_data.end());
    
    // Step 2: Create leaf nodes and fill them with sorted data
    // Each leaf node can hold up to 'order' number of keys
    std::vector<int> leaf_nodes;  // Store leaf node IDs for linking
    int current_leaf = -1;        // ID of the current leaf being filled
    BPTreeNode leaf;              // Current leaf node being constructed
    
    for (size_t i = 0; i < sorted_data.size(); i++) {
        // Create a new leaf node when we reach the order limit
        if (i % order == 0) {
            // Save the previous leaf if it exists
            if (current_leaf != -1) {
                writeNode(current_leaf, leaf);
                leaf_nodes.push_back(current_leaf);
            }
            // Create a new leaf node
            leaf = BPTreeNode();
            leaf.is_leaf = true;
            current_leaf = createNode(leaf);
        }
        
        // Add the current key-value pair to the leaf
        leaf.keys[leaf.num_keys] = sorted_data[i].first;
        // Encode record pointer: block_id * 10000 + record_index
        // This allows us to store both values in a single integer
        leaf.children[leaf.num_keys] = sorted_data[i].second.block_id * 10000 + sorted_data[i].second.record_index;
        leaf.num_keys++;
    }
    
    if (current_leaf != -1) {
        writeNode(current_leaf, leaf);
        leaf_nodes.push_back(current_leaf);
    }
    
    // Link leaf nodes
    for (size_t i = 0; i < leaf_nodes.size() - 1; i++) {
        BPTreeNode leaf_node;
        readNode(leaf_nodes[i], leaf_node);
        leaf_node.next_leaf = leaf_nodes[i + 1];
        writeNode(leaf_nodes[i], leaf_node);
    }
    
    // Build internal nodes
    while (leaf_nodes.size() > 1) {
        std::vector<int> internal_nodes;
        
        for (size_t i = 0; i < leaf_nodes.size(); i += order) {
            BPTreeNode internal;
            internal.is_leaf = false;
            
            int end = std::min(i + order, leaf_nodes.size());
            for (int j = i; j < end; j++) {
                internal.children[j - i] = leaf_nodes[j];
                if (j < end - 1) {
                    // Get first key of next leaf
                    BPTreeNode next_leaf;
                    readNode(leaf_nodes[j + 1], next_leaf);
                    internal.keys[internal.num_keys] = next_leaf.keys[0];
                    internal.num_keys++;
                }
            }
            
            int internal_id = createNode(internal);
            internal_nodes.push_back(internal_id);
        }
        
        leaf_nodes = internal_nodes;
    }
    
    if (!leaf_nodes.empty()) {
        root_id = leaf_nodes[0];
    }
    
    return true;
}

int BPTree::getNumLevels() const {
    if (root_id == -1) return 0;
    
    int levels = 1;
    int current = root_id;
    BPTreeNode node;
    
    while (readNode(current, node) && !node.is_leaf) {
        current = node.children[0];
        levels++;
    }
    
    return levels;
}

std::vector<float> BPTree::getRootKeys() const {
    std::vector<float> keys;
    
    if (root_id == -1) return keys;
    
    BPTreeNode root;
    if (readNode(root_id, root)) {
        for (int i = 0; i < root.num_keys; i++) {
            keys.push_back(root.keys[i]);
        }
    }
    
    return keys;
}

void BPTree::printStatistics() const {
    std::cout << "\n=== B+ TREE STATISTICS ===" << std::endl;
    std::cout << "Order (n): " << order << std::endl;
    std::cout << "Number of nodes: " << getNumNodes() << std::endl;
    std::cout << "Number of levels: " << getNumLevels() << std::endl;
    std::cout << "Root node ID: " << root_id << std::endl;
    
    std::vector<float> root_keys = getRootKeys();
    std::cout << "Root node keys: ";
    for (float key : root_keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    std::cout << "==========================\n" << std::endl;
}

void BPTree::printTree() const {
    if (root_id == -1) {
        std::cout << "Empty tree" << std::endl;
        return;
    }
    
    std::queue<int> q;
    q.push(root_id);
    
    while (!q.empty()) {
        int level_size = q.size();
        std::cout << "Level: ";
        
        for (int i = 0; i < level_size; i++) {
            int node_id = q.front();
            q.pop();
            
            BPTreeNode node;
            if (readNode(node_id, node)) {
                std::cout << "[" << node_id << ": ";
                for (int j = 0; j < node.num_keys; j++) {
                    std::cout << node.keys[j];
                    if (j < node.num_keys - 1) std::cout << ",";
                }
                std::cout << "] ";
                
                if (!node.is_leaf) {
                    for (int j = 0; j <= node.num_keys; j++) {
                        q.push(node.children[j]);
                    }
                }
            }
        }
        std::cout << std::endl;
    }
}

void BPTree::printNode(int node_id) const {
    BPTreeNode node;
    if (readNode(node_id, node)) {
        std::cout << "Node " << node_id << " (";
        std::cout << (node.is_leaf ? "Leaf" : "Internal") << "): ";
        std::cout << "Keys: ";
        for (int i = 0; i < node.num_keys; i++) {
            std::cout << node.keys[i];
            if (i < node.num_keys - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
}



bool BPTree::remove(float key) {
    if (root_id == -1) return false;
    
    int leaf_id = findLeaf(key);
    if (leaf_id == -1) return false;
    
    BPTreeNode leaf;
    if (!readNode(leaf_id, leaf)) return false;
    
    // Check if key exists in leaf
    bool key_found = false;
    for (int i = 0; i < leaf.num_keys; i++) {
        if (leaf.keys[i] == key) {
            key_found = true;
            break;
        }
    }
    
    if (!key_found) return false;
    
    // Remove from leaf
    removeFromLeaf(leaf_id, key);
    
    // Handle underflow if necessary
    BPTreeNode updated_leaf;
    readNode(leaf_id, updated_leaf);
    if (updated_leaf.parent != -1 && updated_leaf.num_keys < (order + 1) / 2) {
        handleUnderflow(leaf_id);
    }
    
    return true;
}

void BPTree::removeFromLeaf(int leaf_id, float key) {
    BPTreeNode leaf;
    if (!readNode(leaf_id, leaf)) return;
    
    // Find the key to remove
    int key_index = -1;
    for (int i = 0; i < leaf.num_keys; i++) {
        if (leaf.keys[i] == key) {
            key_index = i;
            break;
        }
    }
    
    if (key_index == -1) return;
    
    // Shift elements to remove the key
    for (int i = key_index; i < leaf.num_keys - 1; i++) {
        leaf.keys[i] = leaf.keys[i + 1];
        leaf.children[i] = leaf.children[i + 1];
    }
    leaf.num_keys--;
    
    writeNode(leaf_id, leaf);
}

void BPTree::handleUnderflow(int node_id) {
    BPTreeNode node;
    if (!readNode(node_id, node)) return;
    
    if (node.parent == -1) {
        // Root node - if it has no keys and one child, make child the new root
        if (node.num_keys == 0 && !node.is_leaf) {
            if (node.children[0] != -1) {
                root_id = node.children[0];
                BPTreeNode new_root;
                readNode(root_id, new_root);
                new_root.parent = -1;
                writeNode(root_id, new_root);
            }
        }
        return;
    }
    
    // For simplicity, we'll just leave underflowed nodes as-is
    // In a full implementation, we would handle borrowing and merging
    // This ensures the tree remains functional while allowing node count to change
}

int BPTree::removeRange(float min_key, float max_key) {
    int removed_count = 0;
    
    // Find all records in the range
    std::vector<RecordPointer> records_to_remove = rangeSearch(min_key, max_key);
    
    // Remove each record by finding and removing the corresponding key
    for (const RecordPointer& ptr : records_to_remove) {
        // Find the key value for this record pointer by searching through leaf nodes
        // This is a simplified approach - in practice, you'd maintain a reverse mapping
        
        // Search through all leaf nodes to find the key associated with this record pointer
        int current_leaf = findLeaf(min_key);
        bool found = false;
        
        while (current_leaf != -1 && !found) {
            BPTreeNode leaf;
            if (readNode(current_leaf, leaf)) {
                for (int i = 0; i < leaf.num_keys; i++) {
                    int encoded_ptr = leaf.children[i];
                    int block_id = encoded_ptr / 10000;
                    int record_index = encoded_ptr % 10000;
                    
                    if (block_id == ptr.block_id && record_index == ptr.record_index) {
                        // Found the key, remove it
                        if (remove(leaf.keys[i])) {
                            removed_count++;
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    current_leaf = leaf.next_leaf;
                }
            } else {
                break;
            }
        }
    }
    
    return removed_count;
}

void BPTree::writeMetadata() {
    if (!file.is_open()) return;
    
    // Write metadata at the beginning of the file
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&root_id), sizeof(int));
    file.write(reinterpret_cast<const char*>(&next_node_id), sizeof(int));
}

void BPTree::readMetadata() {
    if (!file.is_open()) return;
    
    // Read metadata from the beginning of the file
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&root_id), sizeof(int));
    file.read(reinterpret_cast<char*>(&next_node_id), sizeof(int));
}

std::vector<float> BPTree::getRootNodeKeys() const {
    std::vector<float> keys;
    if (root_id == -1) return keys;
    
    BPTreeNode root_node;
    if (readNode(root_id, root_node)) {
        for (int i = 0; i < root_node.num_keys; i++) {
            keys.push_back(root_node.keys[i]);
        }
    }
    return keys;
}
