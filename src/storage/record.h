/**
 * SC3020 Database Management System
 * Record Structure Definition
 * 
 * This file defines the Record structure that represents a single NBA game record.
 * The structure is designed for efficient storage and retrieval with fixed-length fields.
 * 
 */

#ifndef RECORD_H
#define RECORD_H

// Standard C++ libraries
#include <string>    // For string operations
#include <cstring>   // For memory operations (memset, strncpy)
#include <iostream>  // For console output

/**
 * NBA Game Record Structure
 * 
 * This structure represents a single NBA game record with all required fields.
 * All fields are fixed-length to ensure efficient storage and retrieval.
 * 
 * Memory Layout:
 * - Total size: 44 bytes
 * - All fields are packed tightly for storage efficiency
 * - Fixed-length strings prevent fragmentation
 */
struct Record {
    char game_date[11];        // Game date in DD/MM/YYYY format (10 chars + null terminator)
    int team_id_home;          // Home team identifier (4 bytes)
    int pts_home;              // Home team points scored (4 bytes)
    float fg_pct_home;         // Field goal percentage (4 bytes)
    float ft_pct_home;         // Free throw percentage - KEY ATTRIBUTE for indexing (4 bytes)
    float fg3_pct_home;        // 3-point field goal percentage (4 bytes)
    int ast_home;              // Home team assists (4 bytes)
    int reb_home;              // Home team rebounds (4 bytes)
    int home_team_wins;        // Win indicator (1 = win, 0 = loss) (4 bytes)
    
    /**
     * Default Constructor
     * 
     * Initializes all fields to zero/null values.
     * Uses memset for efficient initialization.
     */
    Record() {
        memset(this, 0, sizeof(Record));
    }
    
    /**
     * Parameterized Constructor
     * 
     * Creates a record with specific values for all fields.
     * 
     * @param date Game date string
     * @param team_id Home team ID
     * @param pts Points scored by home team
     * @param fg_pct Field goal percentage
     * @param ft_pct Free throw percentage (key attribute)
     * @param fg3_pct 3-point percentage
     * @param ast Assists
     * @param reb Rebounds
     * @param wins Win indicator
     */
    Record(const std::string& date, int team_id, int pts, 
           float fg_pct, float ft_pct, float fg3_pct, 
           int ast, int reb, int wins) {
        // Copy date string with bounds checking
        strncpy(game_date, date.c_str(), 10);
        game_date[10] = '\0';  // Ensure null termination
        
        // Assign all other fields
        team_id_home = team_id;
        pts_home = pts;
        fg_pct_home = fg_pct;
        ft_pct_home = ft_pct;      // This is the key attribute for indexing
        fg3_pct_home = fg3_pct;
        ast_home = ast;
        reb_home = reb;
        home_team_wins = wins;
    }
    
    /**
     * Get Record Size
     * 
     * Returns the size of a Record structure in bytes.
     * This is used for calculating storage requirements.
     * 
     * @return Size of Record structure in bytes
     */
    static size_t getSize() {
        return sizeof(Record);
    }
    
    /**
     * Print Record for Debugging
     * 
     * Displays the record contents in a readable format.
     * Useful for debugging and verification purposes.
     */
    void print() const {
        std::cout << "Date: " << game_date 
                  << ", Team: " << team_id_home 
                  << ", Points: " << pts_home 
                  << ", FT%: " << ft_pct_home 
                  << ", Wins: " << home_team_wins << std::endl;
    }
};

// Validate record size remains fixed across platforms
static_assert(sizeof(Record) == 44, "Record must be 44 bytes");

#endif // RECORD_H
