/**
 * SC3020 Database Management System
 * Record Pointer Structure
 * 
 * This file defines the RecordPointer structure that represents a pointer
 * to a record stored in the database. It's used in B+ tree leaf nodes
 * to reference actual records.
 * 
 */

#ifndef RECORD_POINTER_H
#define RECORD_POINTER_H

/**
 * Record Pointer Structure
 * 
 * Represents a pointer to a record in the database.
 * Contains block ID and record index within that block.
 * 
 * This structure is used in B+ tree leaf nodes to store
 * references to actual records in the database.
 */
struct RecordPointer {
    int block_id;                     // ID of the block containing the record
    int record_index;                 // Index of the record within the block
    
    /**
     * Default Constructor
     * 
     * Initializes pointer with invalid values (-1).
     */
    RecordPointer() : block_id(-1), record_index(-1) {}
    
    /**
     * Parameterized Constructor
     * 
     * Creates a record pointer with specific block and record indices.
     * 
     * @param block ID of the block containing the record
     * @param index Index of the record within the block
     */
    RecordPointer(int block, int index) : block_id(block), record_index(index) {}
    
    /**
     * Less Than Comparison Operator
     * 
     * Compares two record pointers for sorting purposes.
     * First compares block IDs, then record indices.
     * 
     * @param other Record pointer to compare with
     * @return true if this pointer is less than the other
     */
    bool operator<(const RecordPointer& other) const {
        if (block_id != other.block_id) {
            return block_id < other.block_id;
        }
        return record_index < other.record_index;
    }
    
    /**
     * Equality Comparison Operator
     * 
     * Checks if two record pointers point to the same record.
     * 
     * @param other Record pointer to compare with
     * @return true if pointers are equal
     */
    bool operator==(const RecordPointer& other) const {
        return block_id == other.block_id && record_index == other.record_index;
    }
};

#endif // RECORD_POINTER_H
