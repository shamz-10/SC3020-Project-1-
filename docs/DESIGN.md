# System Design Documentation

## Architecture Overview

### 1. Storage Layer
```
┌─────────────────┐
│   Text Data     │  ← games.txt (26,651 NBA records)
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Parser        │  ← Converts text to binary records
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Database      │  ← Manages binary file storage
│   (Blocks)      │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Disk File     │  ← database.bin (binary storage)
└─────────────────┘
```

### 2. Indexing Layer
```
┌─────────────────┐
│   Records       │  ← From database storage
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   B+ Tree       │  ← Index on FT_PCT_home
│   Builder       │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Index File    │  ← bptree.bin (tree structure)
└─────────────────┘
```

### 3. Query Layer
```
┌─────────────────┐
│   Query         │  ← "FT_PCT_home > 0.9"
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   B+ Tree       │  ← Efficient range search
│   Search        │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│   Results       │  ← Matching records
└─────────────────┘
```

## Data Structures

### Record Structure
```cpp
struct Record {
    char game_date[11];        // Fixed-length date string
    int team_id_home;          // Team identifier
    int pts_home;              // Points scored
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

### B+ Tree Node
```cpp
struct BPTreeNode {
    bool is_leaf;              // Node type indicator
    int num_keys;              // Current key count
    float keys[MAX_KEYS];      // Key values
    int children[MAX_KEYS+1];  // Child pointers
    int next_leaf;             // Next leaf pointer
    int parent;                // Parent node pointer
};
```

## Design Decisions

### 1. Block Size: 4096 bytes
- **Reason**: Standard disk block size
- **Benefits**: Efficient I/O operations
- **Trade-offs**: Memory usage vs. I/O efficiency

### 2. Fixed-Length Records
- **Reason**: Predictable storage layout
- **Benefits**: Fast random access
- **Trade-offs**: Storage overhead for variable data

### 3. B+ Tree Order Calculation
- **Formula**: `(node_size - overhead) / (key_size + pointer_size)`
- **Result**: ~50 keys per node
- **Benefits**: Balanced tree height

### 4. Bulk Loading
- **Method**: Sort data, then build tree bottom-up
- **Benefits**: Efficient construction, optimal tree shape
- **Complexity**: O(n log n) for sorting + O(n) for building

## Performance Characteristics

### Storage Performance
- **Record Size**: 44 bytes
- **Records per Block**: 93
- **Total Blocks**: ~287 for 26,651 records
- **Storage Efficiency**: ~95% (minimal fragmentation)

### Index Performance
- **Tree Height**: 3 levels
- **Node Count**: ~534 nodes
- **Search Complexity**: O(log n) = O(log 26,651) ≈ 15 comparisons

### Query Performance
- **B+ Tree Search**: O(log n) + O(k) where k = result count
- **Brute Force**: O(n) where n = total records
- **Expected Speedup**: 10-100x for selective queries

## File Organization

### Source Code Structure
```
src/
├── storage/          # Storage layer implementation
├── indexing/         # Indexing layer implementation  
└── utils/            # Utility functions
```

### Data Flow
1. **Input**: Text file (games.txt)
2. **Processing**: Parse → Store → Index → Query
3. **Output**: Binary files + performance statistics

## Error Handling

### File Operations
- Check file existence before operations
- Handle I/O errors gracefully
- Validate data integrity

### Memory Management
- Use stack allocation where possible
- Minimize dynamic allocation
- Proper cleanup in destructors

### Data Validation
- Validate record fields
- Check for data consistency
- Handle malformed input gracefully
