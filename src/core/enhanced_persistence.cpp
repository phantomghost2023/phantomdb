#include "enhanced_persistence.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace phantomdb {
namespace core {

EnhancedPersistenceManager::EnhancedPersistenceManager() 
    : dataDirectory_("./data"), 
      snapshotEnabled_(true), 
      snapshotInterval_(1000), 
      operationCount_(0) {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories(dataDirectory_);
}

void EnhancedPersistenceManager::setDataDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataDirectory_ = directory;
    std::filesystem::create_directories(dataDirectory_);
}

std::string EnhancedPersistenceManager::getDataDirectory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return dataDirectory_;
}

bool EnhancedPersistenceManager::saveDatabase(const std::string& databaseName,
                                            const std::unordered_map<std::string, TableData>& tables,
                                            const std::string& filename) {
    try {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string filePath = getDatabaseFilePath(databaseName, filename);
        
        // Create directory structure if needed
        std::filesystem::path pathObj(filePath);
        std::filesystem::create_directories(pathObj.parent_path());
        
        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            return false;
        }
        
        // Write database header with timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        file << "# PhantomDB Database File\n";
        file << "# Database: " << databaseName << "\n";
        file << "# Format: CSV\n";
        file << "# Generated: " << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S UTC") << "\n";
        file << "# Type: Snapshot\n\n";
        
        // Write each table
        for (const auto& tablePair : tables) {
            const std::string& tableName = tablePair.first;
            const auto& tableData = tablePair.second;
            const auto& columnDefinitions = tableData.columns;
            const auto& rowData = tableData.rows;
            
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
        
        // Increment operation count and check if snapshot is needed
        operationCount_++;
        if (isSnapshotNeeded()) {
            createSnapshot(databaseName, tables);
            operationCount_ = 0; // Reset counter after snapshot
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving database: " << e.what() << std::endl;
        return false;
    }
}

bool EnhancedPersistenceManager::loadDatabase(const std::string& databaseName,
                                            std::unordered_map<std::string, TableData>& tables,
                                            const std::string& filename) {
    try {
        std::lock_guard<std::mutex> lock(mutex_);
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
                tables[currentTable] = TableData();
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
                        tables[currentTable].columns.emplace_back(columnName, columnType);
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
                const auto& columnDefinitions = tables[currentTable].columns;
                
                for (size_t i = 0; i < std::min(cells.size(), columnDefinitions.size()); ++i) {
                    row[columnDefinitions[i].first] = cells[i];
                }
                
                tables[currentTable].rows.push_back(row);
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

bool EnhancedPersistenceManager::appendTransactionLog(const std::string& databaseName,
                                                     const std::string& operation,
                                                     const std::unordered_map<std::string, std::string>& data) {
    try {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string logPath = getTransactionLogPath(databaseName);
        
        // Create directory structure if needed
        std::filesystem::path pathObj(logPath);
        std::filesystem::create_directories(pathObj.parent_path());
        
        // Open file in append mode
        std::ofstream file(logPath, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Failed to open transaction log for appending: " << logPath << std::endl;
            return false;
        }
        
        // Write timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        file << "[" << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S UTC") << "] ";
        
        // Write operation
        file << operation << " ";
        
        // Write data
        file << "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) file << ", ";
            file << pair.first << ": " << escapeCSV(pair.second);
            first = false;
        }
        file << "}\n";
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error appending to transaction log: " << e.what() << std::endl;
        return false;
    }
}

bool EnhancedPersistenceManager::createSnapshot(const std::string& databaseName,
                                              const std::unordered_map<std::string, TableData>& tables) {
    if (!snapshotEnabled_) {
        return true; // Not an error, just not enabled
    }
    
    try {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string snapshotPath = getSnapshotPath(databaseName);
        
        // Create directory structure if needed
        std::filesystem::path pathObj(snapshotPath);
        std::filesystem::create_directories(pathObj.parent_path());
        
        // Create timestamped snapshot file
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream timestamp;
        timestamp << std::put_time(std::gmtime(&time_t), "%Y%m%d_%H%M%S");
        
        std::string timestampedSnapshotPath = snapshotPath + "_" + timestamp.str();
        
        std::ofstream file(timestampedSnapshotPath);
        if (!file.is_open()) {
            std::cerr << "Failed to create snapshot file: " << timestampedSnapshotPath << std::endl;
            return false;
        }
        
        // Write snapshot header
        file << "# PhantomDB Database Snapshot\n";
        file << "# Database: " << databaseName << "\n";
        file << "# Format: CSV\n";
        file << "# Timestamp: " << std::put_time(std::gmtime(&time_t), "%Y-%m-%d %H:%M:%S UTC") << "\n";
        file << "# Type: Snapshot\n\n";
        
        // Write each table (similar to saveDatabase)
        for (const auto& tablePair : tables) {
            const std::string& tableName = tablePair.first;
            const auto& tableData = tablePair.second;
            const auto& columnDefinitions = tableData.columns;
            const auto& rowData = tableData.rows;
            
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
        std::cout << "Snapshot created for database '" << databaseName << "' at " << timestampedSnapshotPath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating snapshot: " << e.what() << std::endl;
        return false;
    }
}

void EnhancedPersistenceManager::setSnapshotEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshotEnabled_ = enabled;
}

bool EnhancedPersistenceManager::isSnapshotEnabled() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return snapshotEnabled_;
}

void EnhancedPersistenceManager::setSnapshotInterval(size_t interval) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshotInterval_ = interval;
}

size_t EnhancedPersistenceManager::getSnapshotInterval() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return snapshotInterval_;
}

std::string EnhancedPersistenceManager::getDatabaseFilePath(const std::string& databaseName, const std::string& filename) const {
    if (!filename.empty()) {
        return dataDirectory_ + "/" + filename;
    }
    return dataDirectory_ + "/" + databaseName + ".db";
}

std::string EnhancedPersistenceManager::getTransactionLogPath(const std::string& databaseName) const {
    return dataDirectory_ + "/" + databaseName + ".log";
}

std::string EnhancedPersistenceManager::getSnapshotPath(const std::string& databaseName) const {
    return dataDirectory_ + "/" + databaseName + "_snapshot";
}

std::string EnhancedPersistenceManager::escapeCSV(const std::string& str) const {
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

std::string EnhancedPersistenceManager::unescapeCSV(const std::string& str) const {
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

bool EnhancedPersistenceManager::isSnapshotNeeded() const {
    return snapshotEnabled_ && operationCount_ >= snapshotInterval_;
}

} // namespace core
} // namespace phantomdb