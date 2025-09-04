/**
 * SC3020 Database Management System
 * Database Class Header
 * 
 * This file defines the Database class that manages the binary database file
 * and provides high-level operations for storing and retrieving records.
 * 
 * The Database class implements:
 * - Block-based storage with 4096-byte blocks for efficient I/O
 * - Fixed-length record storage for predictable access patterns
 * - Metadata persistence for database state management
 * - I/O operation tracking for performance analysis
 * - Comprehensive statistics generation for analysis
 * 
 * File Format:
 * - Header: 8 bytes (num_blocks, num_records)
 * - Data: Sequential blocks of 4096 bytes each
 * - Each block contains a header and variable number of records
 */

#ifndef DATABASE_H
#define DATABASE_H

// Include block structure
#include "block.h"

// Standard C++ libraries
#include <string>    // For file path strings
#include <fstream>   // For file I/O operations
#include <vector>    // For dynamic arrays
#include <set>       // For tracking unique blocks accessed

/**
 * Database Class
 * 
 * Manages a binary database file that stores NBA game records in blocks.
 * Provides high-level operations for adding records, reading blocks,
 * and generating statistics.
 * 
 * Key Features:
 * - Block-based storage with 4096-byte blocks
 * - Automatic block management and allocation
 * - File I/O operations with error handling
 * - Statistics generation for analysis
 */
static const size_t MAX_DATABASE_SIZE = 100 * 1024 * 1024; // Set capacity of database to 100 MB

class Database {
private:
    std::string filename;      // Path to the binary database file
    std::fstream file;         // File stream for I/O operations
    int num_blocks;            // Total number of blocks in the database
    int num_records;           // Total number of records in the database
    
    // I/O counters for performance measurement
    mutable int data_blocks_accessed;           // Backward-compat (kept as total ops before change)
    mutable int total_data_block_ios;           // Total block I/O operations (reads + writes)
    mutable std::set<int> unique_data_blocks;   // Unique data block IDs accessed since last reset
    
public:
    /**
     * Constructor
     * 
     * Creates a Database object with the specified filename.
     * 
     * @param fname Path to the database file
     */
    Database(const std::string& fname);
    
    /**
     * Destructor
     * 
     * Ensures proper cleanup by closing the database file.
     */
    ~Database();
    
    // File Operations
    
    /**
     * Open Database File
     * 
     * Opens the database file for reading and writing.
     * Creates the file if it doesn't exist.
     * 
     * @return true if file was opened successfully, false otherwise
     */
    bool open();
    
    /**
     * Close Database File
     * 
     * Closes the database file and flushes any pending writes.
     */
    void close();
    
    /**
     * Check if Database is Open
     * 
     * Determines whether the database file is currently open.
     * 
     * @return true if file is open, false otherwise
     */
    bool isOpen() const;
    
    // Block Operations
    
    /**
     * Write Block to File
     * 
     * Writes a block to the database file at the specified position.
     * 
     * @param block_id ID of the block to write
     * @param block Block data to write
     * @return true if write was successful, false otherwise
     */
    bool writeBlock(int block_id, const Block& block);
    
    /**
     * Read Block from File
     * 
     * Reads a block from the database file at the specified position.
     * 
     * @param block_id ID of the block to read
     * @param block Block object to store the read data
     * @return true if read was successful, false otherwise
     */
    bool readBlock(int block_id, Block& block);
    
    /**
     * Add New Block
     * 
     * Adds a new block to the database file.
     * 
     * @param block Block to add
     * @return ID of the newly added block, or -1 if failed
     */
    int addBlock(const Block& block);
    
    // Record Operations
    
    /**
     * Add Record to Database
     * 
     * Adds a record to the database by finding an available block
     * or creating a new block if necessary.
     * 
     * @param record Record to add
     * @return true if record was added successfully, false otherwise
     */
    bool addRecord(const Record& record);
    
    /**
     * Get Record from Database
     * 
     * Retrieves a record from the specified block and record index.
     * 
     * @param block_id ID of the block containing the record
     * @param record_index Index of the record within the block
     * @return Record at the specified location
     */
    Record getRecord(int block_id, int record_index);
    
    /**
     * Delete Record from Database
     * 
     * Marks a record as deleted by setting it to empty values.
     * This implements a simple deletion strategy.
     * 
     * @param block_id ID of the block containing the record
     * @param record_index Index of the record within the block
     * @return true if record was deleted successfully
     */
    bool deleteRecord(int block_id, int record_index);
    
    // Statistics and Information
    
    /**
     * Get Number of Blocks
     * 
     * Returns the total number of blocks in the database.
     * 
     * @return Number of blocks
     */
    int getNumBlocks() const { return num_blocks; }
    
    /**
     * Get Number of Records
     * 
     * Returns the total number of records in the database.
     * 
     * @return Number of records
     */
    int getNumRecords() const { return num_records; }
    
    /**
     * Get Records Per Block
     * 
     * Returns the maximum number of records that can be stored in a block.
     * 
     * @return Maximum records per block
     */
    int getRecordsPerBlock() const { return Block::MAX_RECORDS; }
    
    /**
     * Get Record Size
     * 
     * Returns the size of a single record in bytes.
     * 
     * @return Record size in bytes
     */
    size_t getRecordSize() const { return Record::getSize(); }
    
    // Utility Functions
    
    /**
     * Print Database Statistics
     * 
     * Displays comprehensive statistics about the database
     * including record counts, block counts, and file information.
     */
    void printStatistics() const;
    
    /**
     * Get All Records
     * 
     * Retrieves all records from the database and returns them as a vector.
     * This is useful for analysis and verification purposes.
     * 
     * @return Vector containing all records in the database
     */
    std::vector<Record> getAllRecords();
    
    /**
     * Get Data Blocks Accessed Count
     * 
     * Returns the number of unique data blocks accessed during operations.
     * 
     * @return Number of data blocks accessed
     */
    int getDataBlocksAccessed() const { return data_blocks_accessed; }
    int getDataBlockIOsTotal() const { return total_data_block_ios; }
    int getDataBlocksAccessedUnique() const { return static_cast<int>(unique_data_blocks.size()); }
    
    /**
     * Reset I/O Counters
     * 
     * Resets the I/O counters for performance measurement.
     */
    void resetIOCounters() { data_blocks_accessed = 0; total_data_block_ios = 0; unique_data_blocks.clear(); }
    
private:
    /**
     * Write Metadata
     * 
     * Writes database metadata (num_blocks, num_records) to disk.
     */
    void writeMetadata();
    
    /**
     * Read Metadata
     * 
     * Reads database metadata (num_blocks, num_records) from disk.
     */
    void readMetadata();
};

#endif // DATABASE_H
