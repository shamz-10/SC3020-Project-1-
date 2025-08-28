# SC3020 Database Management System - Installation Guide

## Overview

This guide provides comprehensive instructions for setting up and running the SC3020 Database Management System. The system implements a complete database with storage and indexing components for NBA games data analysis.

## System Requirements

### Operating System
- **macOS** (tested on macOS 24.6.0)
- **Linux** (Ubuntu 20.04+ recommended)
- **Windows** (with WSL or MinGW)

### Required Software
- **C++ Compiler**: GCC 7.0+ or Clang 6.0+
- **Make Utility**: GNU Make 4.0+
- **Git** (for cloning the repository)

## Installation Steps

### 1. Clone the Repository
```bash
git clone <repository-url>
cd SC3020_Code_Project
```

### 2. Verify Prerequisites
```bash
# Check C++ compiler
g++ --version

# Check Make utility
make --version

# Check Git
git --version
```

### 3. Download Dataset
The project requires the NBA games dataset. Place the `games.txt` file in the `data/` directory:

```bash
# Create data directory if it doesn't exist
mkdir -p data

# Download or copy games.txt to data/games.txt
# The file should contain tab-separated NBA game statistics
```

**Dataset Format**: Tab-separated values with columns:
- GAME_DATE_EST
- TEAM_ID_home
- PTS_home
- FG_PCT_home
- FT_PCT_home (key attribute for indexing)
- FG3_PCT_home
- AST_home
- REB_home
- HOME_TEAM_WINS

### 4. Build the Project
```bash
# Clean any previous builds
make clean

# Build the project
make
```

**Expected Output**: `database_system` executable should be created.

### 5. Run the System
```bash
# Run all tasks (storage, indexing, query processing)
./database_system

# Or use the provided script
./run_project.sh
```

## Project Structure

```
SC3020_Code_Project/
├── data/
│   └── games.txt              # NBA games dataset
├── src/
│   ├── main.cpp               # Main program entry point
│   ├── storage/               # Storage component
│   │   ├── database.h         # Database class declaration
│   │   ├── database.cpp       # Database implementation
│   │   ├── block.h           # Block structure
│   │   └── record.h          # Record structure
│   ├── indexing/              # Indexing component
│   │   ├── bptree.h          # B+ tree class declaration
│   │   ├── bptree.cpp        # B+ tree implementation
│   │   └── record_pointer.h  # Record pointer structure
│   └── utils/                 # Utilities
│       ├── parser.h          # Parser class declaration
│       └── parser.cpp        # Parser implementation
├── output/                    # Generated files
│   ├── database.bin          # Binary database file
│   ├── bptree.bin           # B+ tree index file
│   ├── task1_results.txt    # Task 1 results
│   ├── task2_results.txt    # Task 2 results
│   ├── task3_results.txt    # Task 3 results
│   ├── summary_results.txt  # Summary results
│   └── results_tables.md    # Formatted results tables
├── docs/                     # Documentation
│   ├── API.md               # API documentation
│   └── DESIGN.md            # Design documentation
├── Makefile                 # Build configuration
├── run_project.sh           # Run script
├── README.md                # Project overview
├── INSTALLATION.md          # This file
└── results_tables.md        # Results tables for report
```

## Running Individual Tasks

### Task 1: Storage Component
The system automatically runs Task 1 when executed. It:
- Parses the NBA games data from `data/games.txt`
- Stores records in binary format in `output/database.bin`
- Reports storage statistics

### Task 2: Indexing Component
The system automatically runs Task 2 after Task 1. It:
- Builds a B+ tree index on the FT_PCT_home attribute
- Stores the index in `output/bptree.bin`
- Reports B+ tree statistics

### Task 3: Query Processing
The system automatically runs Task 3 after Task 2. It:
- Performs range queries using the B+ tree
- Compares with brute force linear scan
- Deletes records with FT_PCT_home > 0.9
- Reports performance metrics

## Expected Outputs

### Console Output
The program will display:
- Task execution progress
- Performance statistics
- Comparison results
- File generation confirmations

### Generated Files
After successful execution, check the `output/` directory for:

1. **Binary Files**:
   - `database.bin`: Binary database file
   - `bptree.bin`: B+ tree index file

2. **Results Files**:
   - `task1_results.txt`: Storage component results
   - `task2_results.txt`: Indexing component results
   - `task3_results.txt`: Query processing results
   - `summary_results.txt`: Complete project summary
   - `results_tables.md`: Formatted tables for report

## Troubleshooting

### Common Issues

1. **Compilation Errors**:
   ```bash
   # Ensure you have the required compiler
   g++ --version
   
   # Clean and rebuild
   make clean && make
   ```

2. **Missing Dataset**:
   ```bash
   # Verify games.txt exists
   ls -la data/games.txt
   
   # Check file format
   head -5 data/games.txt
   ```

3. **Permission Issues**:
   ```bash
   # Make run script executable
   chmod +x run_project.sh
   
   # Make database_system executable
   chmod +x database_system
   ```

4. **Memory Issues**:
   - Ensure sufficient RAM (2GB+ recommended)
   - The dataset contains ~26K records

### Performance Notes
- **Storage**: Takes ~90 seconds for 26K records
- **Indexing**: Takes ~0.6 seconds for B+ tree construction
- **Query**: Takes ~0.0001 seconds for B+ tree queries

## Verification

To verify the installation is working correctly:

1. **Check Build Success**:
   ```bash
   make clean && make
   # Should complete without errors
   ```

2. **Check Execution**:
   ```bash
   ./database_system
   # Should complete all tasks and generate output files
   ```

3. **Check Output Files**:
   ```bash
   ls -la output/
   # Should contain all expected files
   ```

4. **Verify Results**:
   ```bash
   cat output/summary_results.txt
   # Should show complete project statistics
   ```

## Support

For issues or questions:
1. Check this installation guide
2. Review the README.md file
3. Check the documentation in docs/
4. Verify your system meets the requirements

## License

This project is for educational purposes as part of SC3020 Database Management Systems course.
