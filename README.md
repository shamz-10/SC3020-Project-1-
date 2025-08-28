# SC3020 Database Management System - Code Documentation

## Project Overview
This project implements a complete database management system with storage and indexing components for NBA games data analysis. The system demonstrates fundamental database concepts including block-based storage, B+ tree indexing, and query optimization.

## Key Features

- **Storage Component**: Efficient block-based storage with 4096-byte blocks
- **Indexing Component**: B+ tree implementation with bulk loading
- **Query Processing**: Range queries with performance analysis
- **Performance Measurement**: Real I/O counting and timing analysis
- **Data Validation**: Robust parsing and error handling
- **Cross-Platform Safety**: Static assertions to ensure on-disk layout consistency
- **IO Profiling**: Unique and total block I/O counters for fine-grained performance insight
- **Optimized Querying**: Batched block reads for improved cache performance in queries
- **Improved Reporting**: Rich runtime metrics and consistent result generation


## System Architecture

The system is organized into three main components:

1. **Storage Layer** (`src/storage/`):
   - Handles binary file I/O and block management.
   - Includes cross-platform safety via static_assert checks on critical structures like Record and BlockHeader to prevent data corruption due to padding.
3. **Indexing Layer** (`src/indexing/`):
    - Implements B+ tree for efficient queries.
    - Tracks both unique and total I/Os to help distinguish between high-locality access patterns and broad scans.
5. **Utility Layer** (`src/utils/`):
   - Provides data parsing and validation

## Project Structure
```
SC3020_Code_Project/
├── src/                    # Source code directory
│   ├── main.cpp           # Main program entry point
│   ├── storage/           # Storage component
│   │   ├── record.h       # Record structure definition
│   │   ├── block.h        # Block structure definition
│   │   ├── database.h     # Database class header
│   │   └── storage.cpp    # Database implementation
│   ├── indexing/          # Indexing component
│   │   ├── bptree.h       # B+ tree header
│   │   ├── bptree.cpp     # B+ tree implementation
│   │   └── record_pointer.h # Record pointer structure
│   └── utils/             # Utility functions
│       ├── parser.h       # Data parsing header
│       └── parser.cpp     # Data parsing implementation
├── docs/                  # Documentation
│   ├── DESIGN.md          # System design documentation
│   └── API.md             # API documentation
├── data/                  # Data files
│   └── games.txt          # NBA games dataset
├── output/                # Generated output files
├── Makefile               # Build configuration
└── README.md              # This file
```

## Components

### 1. Storage Component
- **Purpose**: Manages disk-based storage of NBA game records
- **Key Features**:
    - Block-based I/O, fixed-length records, efficient storage
    - Static size checks (static_assert) on Record (44 bytes) and BlockHeader (16 bytes) to ensure layout consistency across compilers and platforms
    - Safer block writes: Replaced const_cast with reinterpret_cast<const char*> to avoid undefined behavior during write operations
    - I/O Counters: Tracks both total and unique data block accesses for performance analysis
- **Files**: `src/storage/`

### 2. Indexing Component  
- **Purpose**: Implements B+ tree for efficient range queries
- **Key Features**:
    - Balanced tree structure, bulk loading, disk persistence
    - Bulk loading for fast index creation
    - Dual I/O Counters: Separately tracks total and unique index node accesses for granular performance profiling
    - Insightful Metrics: Enables analysis of index traversal cost independent from data fetch cost
- **Files**: `src/indexing/`

### 3. Utilities
- **Purpose**: Data parsing and validation functions
- **Key Features**:
    - Text-to-binary conversion
    - Input validation and error detection
    - Supports consistent and safe data transformation
- **Files**: `src/utils/`

## How to Compile and Run

### Prerequisites
- C++ compiler (g++ recommended)
- Make utility

### Build Instructions
```bash
cd ~/Desktop/SC3020_Code_Project
make
./database_system
```

### Clean Build
```bash
make clean
```

## Project Tasks

### Task 1: Storage Component
- Parse NBA games data from text file
- Store records in binary database with block structure
- Ensure record layout safety with static size assertions
- Track total and unique data block accesses
- Report storage statistics

### Task 2: Indexing Component
- Build B+ tree index on FT_PCT_home attribute
- Implement bulk loading for efficiency
- Track total and unique I/Os on index nodes
- Report tree statistics

### Task 3: Query Processing
- Perform range queries using B+ tree
- Compare with brute force linear scan
- Optimize data reads by batching pointers by block ID to reduce redundant I/O
- Report performance metrics

## Expected Output
- Database statistics (record size, block count, etc.)
- B+ tree statistics (order, levels, nodes)
- Performance comparison (speedup, block access reduction)
- Detailed I/O statistics: unique vs total I/Os for both data and index blocks
- Improved runtime reports using execution-time counters (instead of recomputation)

## Code Documentation

### Enhanced Comments
All source code files have been extensively commented to explain:
- **Purpose and functionality** of each class and method
- **Algorithm descriptions** with time/space complexity
- **Data structure layouts** and memory organization
- **Key optimizations** and performance improvements
- **File format specifications** and metadata handling

### Key Commented Sections:
- **Storage Component**: Block structure, metadata persistence, optimized record insertion
- **B+ Tree Implementation**: Bulk loading algorithm, range search, node management
- **Data Parsing**: Text-to-binary conversion, validation, error handling
- **Performance Analysis**: Timing measurements, statistics reporting

## Author
SC3020 Database Management Systems.
