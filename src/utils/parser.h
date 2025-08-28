/**
 * SC3020 Database Management System
 * Parser Class Header
 * 
 * This file defines the Parser class that handles data parsing
 * from text format to binary records for the database.
 * 
 * The Parser class provides:
 * - Tab-separated value (TSV) parsing for NBA games data
 * - Data validation and error handling for robust processing
 * - Type conversion from string to appropriate data types
 * - Statistics generation for data analysis
 * - Batch processing capabilities for large datasets
 * 
 * Data Format:
 * - Tab-separated values with 9 columns
 * - Header row is automatically skipped
 * - Supports NBA games data from 2003-2022
 * - Handles missing or malformed data gracefully
 * 
 * Columns:
 * 1. GAME_DATE_EST - Game date (DD/MM/YYYY)
 * 2. TEAM_ID_home - Home team identifier
 * 3. PTS_home - Points scored by home team
 * 4. FG_PCT_home - Field goal percentage
 * 5. FT_PCT_home - Free throw percentage (key for indexing)
 * 6. FG3_PCT_home - 3-point field goal percentage
 * 7. AST_home - Assists
 * 8. REB_home - Rebounds
 * 9. HOME_TEAM_WINS - Win indicator (1=win, 0=loss)
 */

#ifndef PARSER_H
#define PARSER_H

// Include record structure
#include "../storage/record.h"

// Standard C++ libraries
#include <string>    // For string operations
#include <vector>    // For dynamic arrays
#include <fstream>   // For file I/O

/**
 * Parser Class
 * 
 * Provides static methods for parsing NBA games data from text files
 * and converting them to binary Record structures.
 * 
 * Key Features:
 * - Text to binary conversion
 * - Data validation
 * - Error handling
 * - Statistics generation
 */
class Parser {
public:
    /**
     * Parse Single Line
     * 
     * Parses a single line from the games.txt file and converts it
     * to a Record structure.
     * 
     * @param line Text line to parse
     * @return Record structure containing the parsed data
     */
    static Record parseLine(const std::string& line);
    
    /**
     * Parse Entire File
     * 
     * Parses the entire games.txt file and returns a vector of records.
     * Skips the header line and validates each record.
     * 
     * @param filename Path to the text file to parse
     * @return Vector of valid Record structures
     */
    static std::vector<Record> parseFile(const std::string& filename);
    
    /**
     * Validate Record
     * 
     * Checks if a record contains valid data.
     * 
     * @param record Record to validate
     * @return true if record is valid, false otherwise
     */
    static bool isValidRecord(const Record& record);
    
    /**
     * Print Record Statistics
     * 
     * Displays statistics about the parsed records including
     * counts, ranges, and distributions.
     * 
     * @param records Vector of records to analyze
     */
    static void printRecordStats(const std::vector<Record>& records);
    
private:
    /**
     * Split String by Delimiter
     * 
     * Splits a string into tokens based on a delimiter character.
     * 
     * @param str String to split
     * @param delimiter Character to use as delimiter
     * @return Vector of string tokens
     */
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    /**
     * Convert String to Float
     * 
     * Safely converts a string to a float value.
     * Returns 0.0f if conversion fails.
     * 
     * @param str String to convert
     * @return Float value
     */
    static float stringToFloat(const std::string& str);
    
    /**
     * Convert String to Integer
     * 
     * Safely converts a string to an integer value.
     * Returns 0 if conversion fails.
     * 
     * @param str String to convert
     * @return Integer value
     */
    static int stringToInt(const std::string& str);
};

#endif // PARSER_H
