# API Documentation

## Overview
This document describes the API for the SC3020 Database Management System.

## Core Classes

### Database Class
Manages binary database files and block-based storage operations.

#### Constructor
```cpp
Database(const std::string& filename)
```
Creates a Database object with the specified filename.

#### Key Methods
- `bool open()` - Opens the database file
- `void close()` - Closes the database file
- `bool addRecord(const Record& record)` - Adds a record to the database
- `Record getRecord(int block_id, int record_index)` - Retrieves a record
- `void printStatistics()` - Prints database statistics

### BPTree Class
Implements B+ tree indexing for efficient range queries.

#### Constructor
```cpp
BPTree(const std::string& filename)
```
Creates a B+ tree with the specified filename.

#### Key Methods
- `bool open()` - Opens the B+ tree file
- `bool bulkLoad(const std::vector<std::pair<float, RecordPointer>>& data)` - Bulk loads the tree
- `std::vector<RecordPointer> rangeSearch(float min_key, float max_key)` - Performs range search
- `void printStatistics()` - Prints tree statistics

### Parser Class
Handles data parsing from text to binary format.

#### Static Methods
- `static std::vector<Record> parseFile(const std::string& filename)` - Parses entire file
- `static Record parseLine(const std::string& line)` - Parses single line
- `static void printRecordStats(const std::vector<Record>& records)` - Prints statistics

## Data Structures

### Record Structure
```cpp
struct Record {
    char game_date[11];        // Game date
    int team_id_home;          // Team ID
    int pts_home;              // Points
    float fg_pct_home;         // Field goal percentage
    float ft_pct_home;         // Free throw percentage (KEY)
    float fg3_pct_home;        // 3-point percentage
    int ast_home;              // Assists
    int reb_home;              // Rebounds
    int home_team_wins;        // Win indicator
};
```

### Block Structure
```cpp
struct Block {
    BlockHeader header;        // Block metadata
    char data[DATA_SIZE];      // Record storage area
};
```

### RecordPointer Structure
```cpp
struct RecordPointer {
    int block_id;              // Block ID
    int record_index;          // Record index within block
};
```

## Usage Examples

### Creating and Using Database
```cpp
// Create database
Database db("output/database.bin");
db.open();

// Add records
Record record = Parser::parseLine("22/12/2022\t1610612740\t126\t0.484\t0.926\t0.382\t25\t46\t1");
db.addRecord(record);

// Print statistics
db.printStatistics();
```

### Building and Using B+ Tree
```cpp
// Create B+ tree
BPTree bptree("output/bptree.bin");
bptree.open();

// Bulk load data
std::vector<std::pair<float, RecordPointer>> data;
// ... populate data ...
bptree.bulkLoad(data);

// Perform range search
std::vector<RecordPointer> results = bptree.rangeSearch(0.9f, 1.0f);
```

### Parsing Data
```cpp
// Parse entire file
std::vector<Record> records = Parser::parseFile("data/games.txt");

// Print statistics
Parser::printRecordStats(records);
```

## Error Handling
All methods return appropriate boolean values or throw exceptions to indicate success/failure. Check return values and handle exceptions appropriately in your code.

## Performance Notes
- Database operations are block-based for efficiency
- B+ tree provides O(log n) search complexity
- Bulk loading is much faster than individual insertions
- File I/O is buffered for better performance
