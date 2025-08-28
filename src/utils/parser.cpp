#include "parser.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

/**
 * Parse Single Line from Games Data
 * 
 * Parses a single tab-separated line from the games.txt file and converts
 * it to a Record structure. The line format is:
 * date\tt team_id\tt pts\tt fg_pct\tt ft_pct\tt fg3_pct\tt ast\tt reb\tt wins
 * 
 * @param line Tab-separated text line to parse
 * @return Record structure containing the parsed data, or empty record if parsing fails
 */
Record Parser::parseLine(const std::string& line) {
    // Split the line by tab delimiter
    std::vector<std::string> tokens = split(line, '\t');
    
    // Check if we have enough tokens (need at least 9 fields)
    if (tokens.size() < 9) {
        return Record(); // Return empty record if insufficient data
    }
    
    try {
        // Extract each field from the tokens
        std::string game_date = tokens[0];    // Game date (DD/MM/YYYY format)
        int team_id = stringToInt(tokens[1]); // Home team identifier
        int pts = stringToInt(tokens[2]);     // Points scored by home team
        float fg_pct = stringToFloat(tokens[3]); // Field goal percentage
        float ft_pct = stringToFloat(tokens[4]); // Free throw percentage (key for indexing)
        float fg3_pct = stringToFloat(tokens[5]); // 3-point field goal percentage
        int ast = stringToInt(tokens[6]);     // Assists
        int reb = stringToInt(tokens[7]);     // Rebounds
        int wins = stringToInt(tokens[8]);    // Win indicator (1=win, 0=loss)
        
        // Create and return the record
        return Record(game_date, team_id, pts, fg_pct, ft_pct, fg3_pct, ast, reb, wins);
    } catch (...) {
        return Record(); // Return empty record on parsing error
    }
}

std::vector<Record> Parser::parseFile(const std::string& filename) {
    std::vector<Record> records;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return records;
    }
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        if (first_line) {
            first_line = false; // Skip header line
            continue;
        }
        
        Record record = parseLine(line);
        if (isValidRecord(record)) {
            records.push_back(record);
        }
    }
    
    file.close();
    return records;
}

bool Parser::isValidRecord(const Record& record) {
    // Check if record has valid data
    return (record.team_id_home > 0 && 
            record.pts_home >= 0 && 
            record.ft_pct_home >= 0.0f && 
            record.ft_pct_home <= 1.0f &&
            strlen(record.game_date) > 0);
}

void Parser::printRecordStats(const std::vector<Record>& records) {
    if (records.empty()) {
        std::cout << "No valid records found." << std::endl;
        return;
    }
    
    std::cout << "\n=== RECORD STATISTICS ===" << std::endl;
    std::cout << "Total records: " << records.size() << std::endl;
    
    // Calculate statistics for FT_PCT_home
    float min_ft = records[0].ft_pct_home;
    float max_ft = records[0].ft_pct_home;
    float sum_ft = 0.0f;
    
    for (const Record& record : records) {
        if (record.ft_pct_home < min_ft) min_ft = record.ft_pct_home;
        if (record.ft_pct_home > max_ft) max_ft = record.ft_pct_home;
        sum_ft += record.ft_pct_home;
    }
    
    float avg_ft = sum_ft / records.size();
    
    std::cout << "FT_PCT_home - Min: " << min_ft 
              << ", Max: " << max_ft 
              << ", Average: " << avg_ft << std::endl;
    
    // Count records with FT_PCT_home > 0.9
    int count_above_09 = 0;
    for (const Record& record : records) {
        if (record.ft_pct_home > 0.9f) {
            count_above_09++;
        }
    }
    
    std::cout << "Records with FT_PCT_home > 0.9: " << count_above_09 << std::endl;
    std::cout << "==========================\n" << std::endl;
}

std::vector<std::string> Parser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

float Parser::stringToFloat(const std::string& str) {
    try {
        return std::stof(str);
    } catch (...) {
        return 0.0f;
    }
}

int Parser::stringToInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return 0;
    }
}
