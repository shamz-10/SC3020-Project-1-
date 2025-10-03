/**
 * SC3020 Database Management System
 * B+ Tree Header
 * 
 * This file defines the B+ tree data structure and class interface
 * for efficient indexing and range queries on NBA game records.
 * 
 * The B+ Tree implementation provides:
 * - Balanced tree structure for logarithmic search complexity O(log n)
 * - Bulk loading algorithm for efficient construction O(n log n)
 * - Range search capabilities for finding records in key ranges
 * - Disk persistence for large datasets that don't fit in memory
 * - Automatic node splitting and rebalancing during insertion
 * - Metadata persistence for proper file reopening
 * - I/O operation tracking for performance analysis
 * 
 * Tree Structure:
 * - Internal nodes: contain keys and child pointers
 * - Leaf nodes: contain keys and record pointers
 * - All leaf nodes are linked for efficient range queries
 * - Order: maximum number of keys per node (101 in this implementation)
 * 
 * File Format:
 * - Header: 8 bytes (root_id, next_node_id)
 * - Data: Sequential nodes of fixed size
 * - Each node contains keys, pointers, and metadata
 */

#ifndef BPTREE_H
#define BPTREE_H

// Include required headers
#include "record_pointer.h"           // Record pointer structure
#include "../storage/record.h"        // Record structure
#include <vector>                     // For dynamic arrays
#include <fstream>                    // For file I/O
#include <set>                        // For tracking unique nodes accessed
#include <string>                     // For file paths

/**
 * B+ Tree Node Structure
 * 
 * Represents a single node in the B+ tree.
 * Each node can be either an internal node or a leaf node.
 * 
 * Node Layout:
 * - is_leaf: indicates node type
 * - num_keys: current number of keys stored
 * - keys: array of key values (FT_PCT_home values)
 * - children: array of child pointers or record pointers
 * - next_leaf: pointer to next leaf (for leaf nodes)
 * - parent: pointer to parent node
 */
struct BPTreeNode {
    static const int MAX_KEYS = 101;  // Maximum number of keys per node
    
    bool is_leaf;                     // True if this is a leaf node, false if internal
    int num_keys;                     // Number of keys currently stored in this node
    float keys[MAX_KEYS];             // Array of key values (FT_PCT_home percentages)
    int children[MAX_KEYS + 1];       // Array of child pointers (block IDs for internal nodes, encoded record pointers for leaf nodes)
    int next_leaf;                    // Pointer to next leaf node (for leaf nodes only)
    int parent;                       // Pointer to parent node
    
    /**
     * Default Constructor
     * 
     * Initializes a new B+ tree node with default values.
     * By default, nodes are created as leaf nodes.
     */
    BPTreeNode() : is_leaf(true), num_keys(0), next_leaf(-1), parent(-1) {
        // Initialize all keys and children to default values
        for (int i = 0; i < MAX_KEYS; i++) {
            keys[i] = 0.0f;
            children[i] = -1;
        }
        children[MAX_KEYS] = -1;
    }
};

/**
 * B+ Tree Class
 * 
 * Implements a B+ tree data structure for efficient indexing
 * and range queries on NBA game records. The B+ tree provides
 * logarithmic search complexity and supports range queries efficiently.
 * 
 * Key Features:
 * - Balanced tree structure for logarithmic search complexity
 * - Bulk loading for efficient construction (O(n log n) vs O(n²))
 * - Range search capabilities for finding records in key ranges
 * - Disk persistence for large datasets that don't fit in memory
 * - Automatic node splitting and rebalancing during insertion
 * - Metadata persistence for proper file reopening
 * 
 * Tree Structure:
 * - Internal nodes: contain keys and child pointers
 * - Leaf nodes: contain keys and record pointers
 * - All leaf nodes are linked for efficient range queries
 * - Order: maximum number of keys per node (101 in this implementation)
 */
class BPTree {
private:
    std::string filename;             // Path to the B+ tree file
    mutable std::fstream file;        // File stream for I/O operations (mutable for const methods)
    int root_id;                      // ID of the root node
    int next_node_id;                 // Next available node ID
    int order;                        // B+ tree order (maximum keys per node)
    
    // I/O counters for performance measurement
    mutable int index_nodes_accessed;       // Backward-compat total ops
    mutable int total_index_node_ios;       // Total node I/O operations
    mutable std::set<int> unique_index_nodes; // Unique node IDs accessed
    
    // Node Operations
    
    /**
     * Write Node to File
     * 
     * Writes a B+ tree node to the disk file.
     * 
     * @param node_id ID of the node to write
     * @param node Node data to write
     * @return true if write was successful
     */
    bool writeNode(int node_id, const BPTreeNode& node);
    
    /**
     * Read Node from File
     * 
     * Reads a B+ tree node from the disk file.
     * 
     * @param node_id ID of the node to read
     * @param node Node object to store the read data
     * @return true if read was successful
     */
    bool readNode(int node_id, BPTreeNode& node) const;
    
    /**
     * Create New Node
     * 
     * Creates a new node in the B+ tree file.
     * 
     * @param node Node data to create
     * @return ID of the newly created node, or -1 if failed
     */
    int createNode(const BPTreeNode& node);
    
    // Tree Operations
    
    /**
     * Find Leaf Node for Key
     * 
     * Traverses the tree to find the leaf node that should contain
     * the specified key value.
     * 
     * @param key Key value to search for
     * @return ID of the leaf node that should contain the key
     */
    int findLeaf(float key);
    
    /**
     * Insert into Leaf Node
     * 
     * Inserts a key-value pair into a leaf node.
     * 
     * @param leaf_id ID of the leaf node
     * @param key Key value to insert
     * @param ptr Record pointer to insert
     */
    void insertIntoLeaf(int leaf_id, float key, const RecordPointer& ptr);
    
    /**
     * Split Leaf Node
     * 
     * Splits a full leaf node into two nodes and updates the tree structure.
     * 
     * @param leaf_id ID of the leaf node to split
     */
    void splitLeaf(int leaf_id);
    
    /**
     * Insert into Parent Node
     * 
     * Inserts a key and child pointer into a parent node.
     * 
     * @param left_id ID of the left child
     * @param key Key value to promote
     * @param right_id ID of the right child
     */
    void insertIntoParent(int left_id, float key, int right_id);
    
    /**
     * Split Internal Node
     * 
     * Splits a full internal node into two nodes.
     * 
     * @param node_id ID of the internal node to split
     */
    void splitInternal(int node_id);
    
    // Deletion helper functions
    void removeFromLeaf(int leaf_id, float key);
    void handleUnderflow(int node_id);
    void deleteNode(int node_id);
    
    // Utility Functions
    

    
    /**
     * Update Root Node
     * 
     * Updates the root node pointer when the tree structure changes.
     * 
     * @param new_root ID of the new root node
     */
    void updateRoot(int new_root);
    
    /**
     * Write Metadata
     * 
     * Writes B+ tree metadata (root_id, next_node_id) to disk.
     */
    void writeMetadata();
    
    /**
     * Read Metadata
     * 
     * Reads B+ tree metadata (root_id, next_node_id) from disk.
     */
    void readMetadata();
    
public:
    /**
     * Constructor
     * 
     * Creates a B+ tree with the specified filename.
     * Calculates the optimal order based on node size.
     * 
     * @param fname Path to the B+ tree file
     */
    BPTree(const std::string& fname);
    
    /**
     * Destructor
     * 
     * Ensures proper cleanup by closing the B+ tree file.
     */
    ~BPTree();
    
    // File Operations
    
    /**
     * Open B+ Tree File
     * 
     * Opens the B+ tree file for reading and writing.
     * 
     * @return true if file was opened successfully
     */
    bool open();
    
    /**
     * Close B+ Tree File
     * 
     * Closes the B+ tree file.
     */
    void close();
    
    /**
     * Check if B+ Tree is Open
     * 
     * Determines whether the B+ tree file is currently open.
     * 
     * @return true if file is open
     */
    bool isOpen() const;
    
    // Tree Operations
    
    /**
     * Insert Key-Value Pair
     * 
     * Inserts a key-value pair into the B+ tree.
     * 
     * @param key Key value to insert
     * @param ptr Record pointer to insert
     * @return true if insertion was successful
     */
    bool insert(float key, const RecordPointer& ptr);
    
    /**
     * Search for Key
     * 
     * Searches for records with the specified key value.
     * 
     * @param key Key value to search for
     * @return Vector of record pointers matching the key
     */
    std::vector<RecordPointer> search(float key);
    
    /**
     * Range Search
     * 
     * Searches for records with key values in the specified range.
     * 
     * @param min_key Minimum key value (inclusive)
     * @param max_key Maximum key value (inclusive)
     * @return Vector of record pointers in the range
     */
    std::vector<RecordPointer> rangeSearch(float min_key, float max_key);
    
    /**
     * Remove Key
     * 
     * Removes a key-value pair from the B+ tree.
     * 
     * @param key Key value to remove
     * @return true if removal was successful
     */
    bool remove(float key);
    
    /**
     * Remove Range
     * 
     * Removes all key-value pairs within the specified range.
     * 
     * @param min_key Minimum key value (inclusive)
     * @param max_key Maximum key value (inclusive)
     * @return Number of records removed
     */
    int removeRange(float min_key, float max_key);
    
    // Bulk Loading
    
    /**
     * Bulk Load B+ Tree
     * 
     * Efficiently constructs a B+ tree from sorted data.
     * This method is much faster than individual insertions.
     * 
     * @param data Vector of key-value pairs to insert
     * @return true if bulk loading was successful
     */
    bool bulkLoad(const std::vector<std::pair<float, RecordPointer>>& data);
    
    // Statistics and Information
    
    /**
     * Get Number of Nodes
     * 
     * Returns the total number of nodes in the B+ tree.
     * 
     * @return Number of nodes
     */
    int getNumNodes() const { return next_node_id; }
    
    /**
     * Get Number of Levels
     * 
     * Returns the height of the B+ tree.
     * 
     * @return Number of levels in the tree
     */
    int getNumLevels() const;
    
    /**
     * Get Root Keys
     * 
     * Returns the keys stored in the root node.
     * 
     * @return Vector of root node keys
     */
    std::vector<float> getRootKeys() const;
    
    // Utility Functions
    
    /**
     * Print Tree Structure
     * 
     * Displays the B+ tree structure for debugging purposes.
     */
    void printTree() const;
    
    /**
     * Print Tree Statistics
     * 
     * Displays comprehensive statistics about the B+ tree.
     */
    void printStatistics() const;
    
    /**
     * Get Index Nodes Accessed Count
     * 
     * Returns the number of index nodes accessed during operations.
     * 
     * @return Number of index nodes accessed
     */
    int getIndexNodesAccessed() const { return index_nodes_accessed; }
    int getIndexNodeIOsTotal() const { return total_index_node_ios; }
    int getIndexNodesAccessedUnique() const { return static_cast<int>(unique_index_nodes.size()); }
    
    /**
     * Reset I/O Counters
     * 
     * Resets the I/O counters for performance measurement.
     */
    void resetIOCounters() { index_nodes_accessed = 0; total_index_node_ios = 0; unique_index_nodes.clear(); }
    
    /**
     * Get B+ Tree Order
     * 
     * Returns the order of the B+ tree.
     * 
     * @return B+ tree order
     */
    int getOrder() const { return order; }
    
    /**
     * Get Root Node ID
     * 
     * Returns the ID of the root node.
     * 
     * @return Root node ID
     */
    int getRootId() const { return root_id; }
    
    /**
     * Get Root Node Keys
     * 
     * Returns the keys stored in the root node.
     * 
     * @return Vector of root node keys
     */
    std::vector<float> getRootNodeKeys() const;
    
    /**
     * Print Node Information
     * 
     * Displays information about a specific node.
     * 
     * @param node_id ID of the node to display
     */
    void printNode(int node_id) const;
};

#endif // BPTREE_H
