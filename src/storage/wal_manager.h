#ifndef PHANTOMDB_WAL_MANAGER_H
#define PHANTOMDB_WAL_MANAGER_H

#include <string>
#include <memory>

namespace phantomdb {
namespace storage {

class WALManager {
public:
    WALManager();
    ~WALManager();
    
    // Initialize the WAL manager
    bool initialize();
    
    // Shutdown the WAL manager
    void shutdown();
    
    // Write a log entry
    bool writeLogEntry(const std::string& data);
    
    // Replay log entries
    bool replayLogs();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_WAL_MANAGER_H