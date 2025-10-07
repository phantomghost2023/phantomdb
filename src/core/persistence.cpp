#include "persistence.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace phantomdb {
namespace core {

PersistenceManager::PersistenceManager() : dataDirectory_("./data") {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories(dataDirectory_);
}

void PersistenceManager::setDataDirectory(const std::string& directory) {
    dataDirectory_ = directory;
    std::filesystem::create_directories(dataDirectory_);
}

std::string PersistenceManager::getDataDirectory() const {
    return dataDirectory_;
}

bool PersistenceManager::saveDatabase(const std::string& databaseName,
                                     const std::unordered_map<std::string, 
                                         std::pair<std::vector<std::pair<std::string, std::string>>, 
                                                  std::vector<std::unordered_map<std::string, std::string>>>>& tables,
                                     const std::string& filename) {
    try {
        std::string filePath = getDatabaseFilePath(databaseName, filename);
        
        // Create directory structure if needed
        std::filesystem::path pathObj(filePath);
        std::filesystem::create_directories(pathObj.parent_path());
        
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        
        // Write database header
        file << "# PhantomDB Database File\n";
        file << "# Database: " << databaseName << "\n";
        file << "# Format: CSV\n";
        file << "# Generated: " << __DATE__ << " " << __TIME__ << "\n\n";
        
        // Write each table
        for (const auto& tablePair : tables) {
            const std::string& tableName = tablePair.first;
            const auto& tableData = tablePair.second;
            const auto& columnDefinitions = tableData.first;
            const auto& rowData = tableData.second;
            
            // Write table header
            file << "[TABLE:" << tableName << "]\n";
            
            // Write column definitions
            file << "COLUMNS:";
            for (size_t i = 0; i < columnDefinitions.size(); ++i) {
                if (i > 0) file << ",";
                file << columnDefinitions[i].first << ":" << columnDefinitions[i].second;
            }
            file << "\n";
            
            // Write row data
            if (!rowData.empty()) {
                // Write header row
                file << "DATA:";
                bool firstCol = true;
                for (const auto& column : columnDefinitions) {
                    if (!firstCol) file << ",";
                    file << column.first;
                    firstCol = false;
                }
                file << "\n";
                
                // Write data rows
                for (const auto& row : rowData) {
                    file << "ROW:";
                    firstCol = true;
                    for (const auto& column : columnDefinitions) {
                        if (!firstCol) file << ",";
                        auto it = row.find(column.first);
                        if (it != row.end()) {
                            file << escapeCSV(it->second);
                        }
                        firstCol = false;
                    }
                    file << "\n";
                }
            }
            
            file << "\n";
        }
        
        file.close();
        std::cout << "Database '" << databaseName << "' saved to " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving database: " << e.what() << std::endl;
        return false;
    }
}

bool PersistenceManager::loadDatabase(const std::string& databaseName,
                                     std::unordered_map<std::string, 
                                         std::pair<std::vector<std::pair<std::string, std::string>>, 
                                                  std::vector<std::unordered_map<std::string, std::string>>>>& tables,
                                     const std::string& filename) {
    try {
        std::string filePath = getDatabaseFilePath(databaseName, filename);
        
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for reading: " << filePath << std::endl;
            return false;
        }
        
        std::string line;
        std::string currentTable;
        
        // Clear existing data
        tables.clear();
        
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Check for table header
            if (line.substr(0, 7) == "[TABLE:") {
                currentTable = line.substr(7, line.length() - 8); // Remove [TABLE: and ]
                tables[currentTable] = std::make_pair(
                    std::vector<std::pair<std::string, std::string>>(), 
                    std::vector<std::unordered_map<std::string, std::string>>()
                );
                continue;
            }
            
            // Check for columns
            if (line.substr(0, 8) == "COLUMNS:" && !currentTable.empty()) {
                std::string columnsStr = line.substr(8);
                std::stringstream ss(columnsStr);
                std::string columnPair;
                
                while (std::getline(ss, columnPair, ',')) {
                    size_t colonPos = columnPair.find(':');
                    if (colonPos != std::string::npos) {
                        std::string columnName = columnPair.substr(0, colonPos);
                        std::string columnType = columnPair.substr(colonPos + 1);
                        tables[currentTable].first.emplace_back(columnName, columnType);
                    }
                }
                continue;
            }
            
            // Check for data header
            if (line.substr(0, 5) == "DATA:" && !currentTable.empty()) {
                // We don't need to process the header line, just continue
                continue;
            }
            
            // Check for data rows
            if (line.substr(0, 4) == "ROW:" && !currentTable.empty()) {
                std::string rowStr = line.substr(4);
                std::stringstream ss(rowStr);
                std::string cell;
                std::vector<std::string> cells;
                
                while (std::getline(ss, cell, ',')) {
                    cells.push_back(unescapeCSV(cell));
                }
                
                // Create row map
                std::unordered_map<std::string, std::string> row;
                const auto& columnDefinitions = tables[currentTable].first;
                
                for (size_t i = 0; i < std::min(cells.size(), columnDefinitions.size()); ++i) {
                    row[columnDefinitions[i].first] = cells[i];
                }
                
                tables[currentTable].second.push_back(row);
                continue;
            }
        }
        
        file.close();
        std::cout << "Database '" << databaseName << "' loaded from " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading database: " << e.what() << std::endl;
        return false;
    }
}

std::string PersistenceManager::getDatabaseFilePath(const std::string& databaseName, const std::string& filename) const {
    if (!filename.empty()) {
        return dataDirectory_ + "/" + filename;
    }
    return dataDirectory_ + "/" + databaseName + ".db";
}

std::string PersistenceManager::escapeCSV(const std::string& str) const {
    // If string contains commas, quotes, or newlines, wrap in quotes and escape quotes
    if (str.find_first_of(",\"\n") != std::string::npos) {
        std::string escaped = str;
        // Replace all quotes with double quotes
        size_t pos = 0;
        while ((pos = escaped.find('"', pos)) != std::string::npos) {
            escaped.replace(pos, 1, "\"\"");
            pos += 2;
        }
        return "\"" + escaped + "\"";
    }
    return str;
}

std::string PersistenceManager::unescapeCSV(const std::string& str) const {
    if (str.length() >= 2 && str.front() == '"' && str.back() == '"') {
        std::string unescaped = str.substr(1, str.length() - 2);
        // Replace all double quotes with single quotes
        size_t pos = 0;
        while ((pos = unescaped.find("\"\"", pos)) != std::string::npos) {
            unescaped.replace(pos, 2, "\"");
            pos += 1;
        }
        return unescaped;
    }
    return str;
}

} // namespace core
} // namespace phantomdb