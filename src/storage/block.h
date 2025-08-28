/**
 * SC3020 Database Management System
 * Block Structure Definition
 * 
 * This file defines the Block structure that represents a storage unit
 * for the database. Each block contains a header and data area for records.
 * 
 */

#ifndef BLOCK_H
#define BLOCK_H

// Include record structure
#include "record.h"

// Standard C++ libraries
#include <vector>   // For dynamic arrays
#include <cstring>  // For memory operations

/**
 * Block Header Structure
 * 
 * Contains metadata about the block including block ID, record count,
 * and pointer to next block. The header is padded to ensure proper alignment.
 */
struct BlockHeader {
    int block_id;              // Unique identifier for this block
    int num_records;           // Number of records currently stored in this block
    int next_block;            // Pointer to next block (-1 if this is the last block)
    char padding[4];           // Padding to make header exactly 16 bytes
    
    /**
     * Default Constructor
     * 
     * Initializes header fields with default values.
     */
    BlockHeader() : block_id(0), num_records(0), next_block(-1) {
        memset(padding, 0, sizeof(padding));
    }
};

/**
 * Block Structure
 * 
 * Represents a 4096-byte storage unit that can hold multiple records.
 * The block consists of a header followed by a data area for records.
 * 
 * Block Layout:
 * - Header: 16 bytes (BlockHeader)
 * - Data Area: 4080 bytes (for storing records)
 * - Total Size: 4096 bytes (standard disk block size)
 */
struct Block {
    static const int BLOCK_SIZE = 4096;                    // Total block size in bytes
    static const int HEADER_SIZE = sizeof(BlockHeader);    // Size of block header
    static const int DATA_SIZE = BLOCK_SIZE - HEADER_SIZE; // Size of data area
    static const int MAX_RECORDS = DATA_SIZE / sizeof(Record); // Maximum records per block
    
    BlockHeader header;        // Block metadata
    char data[DATA_SIZE];      // Data area for storing records
    
    /**
     * Default Constructor
     * 
     * Initializes the block with zero values.
     */
    Block() {
        memset(this, 0, sizeof(Block));
    }
    
    /**
     * Add Record to Block
     * 
     * Attempts to add a record to the block's data area.
     * Records are stored sequentially in the data area.
     * 
     * @param record The record to add
     * @return true if record was added successfully, false if block is full
     */
    bool addRecord(const Record& record) {
        // Check if block has space for another record
        if (header.num_records >= MAX_RECORDS) {
            return false; // Block is full
        }
        
        // Calculate offset for the new record
        size_t offset = header.num_records * sizeof(Record);
        
        // Copy record to the data area
        memcpy(data + offset, &record, sizeof(Record));
        
        // Increment record count
        header.num_records++;
        
        return true;
    }
    
    /**
     * Get Record from Block
     * 
     * Retrieves a record from the specified index in the block.
     * 
     * @param index Index of the record to retrieve
     * @return Record at the specified index, or empty record if index is invalid
     */
    Record getRecord(int index) const {
        // Check if index is valid
        if (index >= header.num_records) {
            return Record(); // Return empty record if index out of bounds
        }
        
        // Calculate offset for the requested record
        size_t offset = index * sizeof(Record);
        
        // Create record and copy data from block
        Record record;
        memcpy(&record, data + offset, sizeof(Record));
        
        return record;
    }
    
    /**
     * Check if Block is Full
     * 
     * Determines whether the block can hold more records.
     * 
     * @return true if block is full, false otherwise
     */
    bool isFull() const {
        return header.num_records >= MAX_RECORDS;
    }
    
    /**
     * Get Number of Records in Block
     * 
     * Returns the current number of records stored in this block.
     * 
     * @return Number of records in the block
     */
    int getNumRecords() const {
        return header.num_records;
    }
    
    /**
     * Clear Block
     * 
     * Resets the block to its initial state by zeroing all data.
     */
    void clear() {
        memset(this, 0, sizeof(Block));
    }
    
    /**
     * Print Block Information
     * 
     * Displays block metadata for debugging purposes.
     */
    void printInfo() const {
        std::cout << "Block " << header.block_id 
                  << ": " << header.num_records 
                  << " records, Next: " << header.next_block << std::endl;
    }
};

// Enforce expected structure sizes to avoid platform-dependent padding surprises
static_assert(sizeof(BlockHeader) == 16, "BlockHeader must be 16 bytes");

#endif // BLOCK_H
