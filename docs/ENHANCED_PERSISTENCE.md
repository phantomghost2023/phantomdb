# Enhanced Persistence in PhantomDB

This document describes the enhanced persistence features implemented in PhantomDB, including file-backed storage with snapshot and append-only log functionality.

## Features

### 1. File-Backed Persistence
PhantomDB now supports durable storage of database contents to disk files. Each database is saved as a CSV-formatted file that preserves:
- Table structures and column definitions
- All row data
- Metadata including timestamps

### 2. Transaction Logging (Append-Only)
All database operations are logged to an append-only transaction log file. This provides:
- Audit trail of all operations
- Recovery capability in case of crashes
- Operational transparency

### 3. Automatic Snapshots
PhantomDB can automatically create periodic snapshots of the database state:
- Configurable snapshot intervals (default: every 1000 operations)
- Timestamped snapshot files for historical recovery
- Low-overhead background operation

## API Usage

### Basic Persistence Operations

```cpp
#include "database.h"

phantomdb::core::Database db;

// Save database to disk
db.saveToDisk("my_database");

// Load database from disk
db.loadFromDisk("my_database");
```

### Configuration

```cpp
// Set custom data directory
db.setDataDirectory("/path/to/data");

// Enable/disable snapshots
db.setSnapshotEnabled(true);

// Set snapshot interval (operations between snapshots)
db.setSnapshotInterval(500);
```

### Manual Operations

```cpp
// Append custom entry to transaction log
std::unordered_map<std::string, std::string> logData = {
    {"user", "admin"},
    {"action", "maintenance"}
};
db.appendTransactionLog("my_database", "MAINTENANCE", logData);

// Create manual snapshot
db.createSnapshot("my_database");
```

## File Structure

```
data/
├── my_database.db          # Main database file (CSV format)
├── my_database.log         # Transaction log (append-only)
└── my_database_snapshot_20231201_120000  # Timestamped snapshot
```

## Implementation Details

### EnhancedPersistenceManager
The core of the enhanced persistence system is the `EnhancedPersistenceManager` class, which handles:

1. **File I/O Operations**: Reading and writing database files in CSV format
2. **Transaction Logging**: Appending operations to log files with timestamps
3. **Snapshot Management**: Creating and managing periodic database snapshots
4. **Configuration**: Managing persistence settings

### Data Format
Database files use a simple CSV-based format with section headers:
```
# PhantomDB Database File
# Database: my_database
# Format: CSV
# Generated: 2023-12-01 12:00:00 UTC

[TABLE:users]
COLUMNS:id:int,name:string,email:string
DATA:id,name,email
ROW:1,John Doe,john@example.com
ROW:2,Jane Smith,jane@example.com
```

## Performance Considerations

1. **Asynchronous Operations**: File I/O operations are performed synchronously but are optimized for minimal overhead
2. **Batching**: Transaction logs are appended without expensive file reopening
3. **Snapshot Efficiency**: Snapshots are created efficiently without blocking database operations
4. **Memory Management**: Large datasets are processed in chunks to minimize memory usage

## Recovery Process

In case of a crash, PhantomDB can recover by:
1. Loading the most recent snapshot
2. Replaying transaction log entries since the snapshot
3. Restoring database to consistent state

## Future Enhancements

Planned improvements include:
- Compression of snapshot files
- Encryption of persistence files
- Incremental backup support
- Cloud storage integration