/**
 * SC3020 Database Management System
 * Database Implementation
 * 
 * This file contains the implementation of the Database class methods
 * for managing binary database files and block-based storage operations.
 * 
 */

#include "database.h"
#include <iostream>  // For console output
#include <cstring>   // For memory operations

/**
 * Database Constructor
 * 
 * Initializes a Database object with the specified filename.
 * Sets initial values for block and record counts.
 * 
 * @param fname Path to the database file
 */
Database::Database(const std::string& fname) : filename(fname), num_blocks(0), num_records(0), data_blocks_accessed(0), total_data_block_ios(0) {
    // Constructor initializes member variables
    // filename: stores the path to the database file
    // num_blocks: tracks total number of blocks (starts at 0)
    // num_records: tracks total number of records (starts at 0)
    // data_blocks_accessed: tracks I/O operations for performance measurement
}

/**
 * Database Destructor
 * 
 * Ensures proper cleanup by closing the database file
 * when the Database object is destroyed.
 */
Database::~Database() {
    close(); // Close the file to ensure all data is written
}

/**
 * Open Database File
 * 
 * Opens the database file for reading and writing.
 * If the file doesn't exist, it creates a new file.
 * 
 * @return true if file was opened successfully, false otherwise
 */
bool Database::open() {
    // Try to open file for both reading and writing
    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    
    if (!file.is_open()) {
        // If file doesn't exist, create it first
        file.open(filename, std::ios::binary | std::ios::out);
        file.close();
        // Then open it for both reading and writing
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
        
        // Initialize metadata for new file
        writeMetadata();
    } else {
        // Load existing metadata
        readMetadata();
    }
    
    return file.is_open();
}

/**
 * Close Database File
 * 
 * Closes the database file and flushes any pending writes.
 * This ensures all data is safely written to disk.
 */
void Database::close() {
    if (file.is_open()) {
        // Write metadata before closing
        writeMetadata();
        file.close();
    }
}

/**
 * Check if Database is Open
 * 
 * Determines whether the database file is currently open.
 * 
 * @return true if file is open, false otherwise
 */
bool Database::isOpen() const {
    return file.is_open();
}

/**
 * Write Block to File
 * 
 * Writes a block to the database file at the specified position.
 * Each block is written at a fixed offset based on its block ID.
 * 
 * @param block_id ID of the block to write
 * @param block Block data to write
 * @return true if write was successful, false otherwise
 */
bool Database::writeBlock(int block_id, const Block& block) {
    if (!file.is_open()) return false;
    
    // Calculate file position: metadata (8 bytes) + block_id * block_size
    file.seekp(8 + block_id * Block::BLOCK_SIZE);
    
    // Write the entire block to the file
    file.write(reinterpret_cast<const char*>(&block), Block::BLOCK_SIZE);
    
    // Increment I/O counters
    data_blocks_accessed++;            // kept for backward compat
    total_data_block_ios++;
    unique_data_blocks.insert(block_id);
    
    return file.good(); // Return true if write operation was successful
}

/**
 * Read Block from File
 * 
 * Reads a block from the database file at the specified position.
 * 
 * @param block_id ID of the block to read
 * @param block Block object to store the read data
 * @return true if read was successful, false otherwise
 */
bool Database::readBlock(int block_id, Block& block) {
    if (!file.is_open()) return false;
    
    // Calculate file position: metadata (8 bytes) + block_id * block_size
    file.seekg(8 + block_id * Block::BLOCK_SIZE);
    
    // Read the entire block from the file
    file.read(reinterpret_cast<char*>(&block), Block::BLOCK_SIZE);
    
    // Increment I/O counters
    data_blocks_accessed++;            // kept for backward compat
    total_data_block_ios++;
    unique_data_blocks.insert(block_id);
    
    return file.good(); // Return true if read operation was successful
}

/**
 * Add New Block
 * 
 * Adds a new block to the database file.
 * The block ID is automatically assigned based on the current block count.
 * 
 * @param block Block to add
 * @return ID of the newly added block, or -1 if failed
 */
int Database::addBlock(const Block& block) {
    if (!file.is_open()) return -1;
    
    // Assign the next available block ID
    int block_id = num_blocks;
    
    // Write the block to the file
    if (writeBlock(block_id, block)) {
        num_blocks++; // Increment block count
        return block_id;
    }
    
    return -1; // Return -1 if write failed
}

/**
 * Add Record to Database
 * 
 * Adds a record to the database by finding an available block
 * or creating a new block if necessary. This method implements
 * an optimized approach that avoids the O(n²) complexity of
 * scanning all blocks for each record.
 * 
 * Algorithm:
 * 1. Try to add to the current (last) block if it has space
 * 2. If current block is full, create a new block
 * 3. This ensures O(1) average case complexity
 * 
 * @param record Record to add
 * @return true if record was added successfully, false otherwise
 */
bool Database::addRecord(const Record& record) {
    // Step 1: Try to add to the current block if it exists and has space
    if (num_blocks > 0) {
        Block currentBlock;
        if (readBlock(num_blocks - 1, currentBlock)) {
            if (!currentBlock.isFull()) {
                currentBlock.addRecord(record);
                writeBlock(num_blocks - 1, currentBlock);
                num_records++;
                return true;
            }
        }
    }

    // Step 2: Capacity check before creating a new block
    static const size_t MAX_DATABASE_SIZE = 100 * 1024 * 1024; // 100 MB
    size_t current_size = (num_blocks * Block::BLOCK_SIZE) + sizeof(int) * 2; // 8-byte metadata
    if (current_size + Block::BLOCK_SIZE > MAX_DATABASE_SIZE) {
        std::cerr << "Error: Database capacity exceeded (100 MB limit)." << std::endl;
        return false;
    }

    // Step 3: Create new block if current block is full or doesn't exist
    Block newBlock;
    newBlock.header.block_id = num_blocks;
    newBlock.addRecord(record);

    if (addBlock(newBlock) != -1) {
        num_records++;
        return true;
    }

    return false; // Return false if all attempts failed
}


/**
 * Get Record from Database
 * 
 * Retrieves a record from the specified block and record index.
 * 
 * @param block_id ID of the block containing the record
 * @param record_index Index of the record within the block
 * @return Record at the specified location
 */
Record Database::getRecord(int block_id, int record_index) {
    Block block;
    if (readBlock(block_id, block)) {
        return block.getRecord(record_index);
    }
    return Record(); // Return empty record if read failed
}

bool Database::deleteRecord(int block_id, int record_index) {
    Block block;
    if (readBlock(block_id, block)) {
        // Create an empty record to mark as deleted
        Record empty_record;
        // Replace the record at the specified index
        size_t offset = record_index * sizeof(Record);
        memcpy(block.data + offset, &empty_record, sizeof(Record));
        // Write the updated block back to disk
        return writeBlock(block_id, block);
    }
    return false; // Return false if read failed
}

/**
 * Print Database Statistics
 * 
 * Displays comprehensive statistics about the database
 * including record counts, block counts, and file information.
 * This information is useful for analysis and verification.
 */
void Database::printStatistics() const {
    std::cout << "\n=== DATABASE STATISTICS ===" << std::endl;
    std::cout << "Record size: " << getRecordSize() << " bytes" << std::endl;
    std::cout << "Records per block: " << getRecordsPerBlock() << std::endl;
    std::cout << "Total records: " << num_records << std::endl;
    std::cout << "Total blocks: " << num_blocks << std::endl;
    std::cout << "Block size: " << Block::BLOCK_SIZE << " bytes" << std::endl;
    std::cout << "Database file: " << filename << std::endl;
    std::cout << "==========================\n" << std::endl;
}

/**
 * Get All Records
 * 
 * Retrieves all records from the database and returns them as a vector.
 * This function reads all blocks and extracts all records.
 * 
 * @return Vector containing all records in the database
 */
std::vector<Record> Database::getAllRecords() {
    std::vector<Record> records;
    
    // Iterate through all blocks
    for (int i = 0; i < num_blocks; i++) {
        Block block;
        if (readBlock(i, block)) {
            // Extract all records from this block
            for (int j = 0; j < block.getNumRecords(); j++) {
                records.push_back(block.getRecord(j));
            }
        }
    }
    
    return records;
}

/**
 * Write Metadata to Database File
 * 
 * Writes database metadata (num_blocks, num_records) to the beginning
 * of the database file. This ensures that when the database is reopened,
 * we can restore the correct state without scanning all blocks.
 * 
 * File Layout:
 * - Bytes 0-3: num_blocks (4 bytes)
 * - Bytes 4-7: num_records (4 bytes)
 * - Bytes 8+: Block data
 */
void Database::writeMetadata() {
    if (!file.is_open()) return;
    
    // Write metadata at the beginning of the file
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&num_blocks), sizeof(int));
    file.write(reinterpret_cast<const char*>(&num_records), sizeof(int));
}

/**
 * Read Metadata from Database File
 * 
 * Reads database metadata (num_blocks, num_records) from the beginning
 * of the database file. This is called when opening an existing database
 * to restore the correct state.
 * 
 * File Layout:
 * - Bytes 0-3: num_blocks (4 bytes)
 * - Bytes 4-7: num_records (4 bytes)
 * - Bytes 8+: Block data
 */
void Database::readMetadata() {
    if (!file.is_open()) return;
    
    // Read metadata from the beginning of the file
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&num_blocks), sizeof(int));
    file.read(reinterpret_cast<char*>(&num_records), sizeof(int));
}
