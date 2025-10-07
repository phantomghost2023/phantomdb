# PhantomDB Import/Export Module

Data import and export capabilities for PhantomDB supporting CSV, JSON, and SQLite formats.

## Features

- Import data from CSV, JSON, and SQLite files
- Export data to CSV, JSON, and SQLite files
- Configurable import/export options
- Batch processing for large datasets
- Error handling and reporting
- Header row support for CSV files
- Custom delimiters for CSV files
- Pretty printing for JSON exports

## Supported Formats

### CSV (Comma-Separated Values)
- Import and export support
- Custom delimiter support
- Quote handling
- Header row support
- Null value handling

### JSON (JavaScript Object Notation)
- Import and export support
- Pretty printing option
- Nested object support
- Array support

### SQLite
- Import from SQLite databases
- Export to SQLite databases
- Table schema preservation
- Index preservation

## Usage

### Importing Data

```cpp
#include "data_importer.h"

using namespace phantomdb::import_export;

int main() {
    // Create importer
    DataImporter importer;
    
    // Configure import options
    ImportOptions options;
    options.hasHeader = true;
    options.delimiter = ',';
    options.nullValue = "NULL";
    options.ignoreDuplicates = false;
    options.batchSize = 1000;
    
    // Import CSV data
    ImportResult result = importer.importFromFile(
        "data.csv",
        "mydatabase",
        "users",
        DataFormat::CSV,
        options
    );
    
    if (result.success) {
        std::cout << "Imported " << result.rowsImported << " rows successfully!" << std::endl;
    } else {
        std::cerr << "Import failed: " << result.errorMessage << std::endl;
    }
    
    return 0;
}
```

### Exporting Data

```cpp
#include "data_exporter.h"

using namespace phantomdb::import_export;

int main() {
    // Create exporter
    DataExporter exporter;
    
    // Configure export options
    ExportOptions options;
    options.includeHeader = true;
    options.delimiter = ',';
    options.nullValue = "NULL";
    options.prettyPrint = true;
    options.batchSize = 1000;
    
    // Export data to CSV
    ExportResult result = exporter.exportToFile(
        "exported_data.csv",
        "mydatabase",
        "users",
        ExportFormat::CSV,
        options
    );
    
    if (result.success) {
        std::cout << "Exported " << result.rowsExported << " rows successfully!" << std::endl;
    } else {
        std::cerr << "Export failed: " << result.errorMessage << std::endl;
    }
    
    return 0;
}
```

## API Reference

### DataImporter Class

#### Methods

- `ImportResult importFromFile(const std::string& filePath, const std::string& databaseName, const std::string& tableName, DataFormat format, const ImportOptions& options)`
- `ImportResult importFromString(const std::string& data, const std::string& databaseName, const std::string& tableName, DataFormat format, const ImportOptions& options)`
- `ImportResult importFromSQLite(const std::string& sqlitePath, const std::string& databaseName, const std::string& tableName, const ImportOptions& options)`
- `void setDatabaseConnection(std::shared_ptr<DatabaseConnection> connection)`

#### ImportOptions Structure

```cpp
struct ImportOptions {
    bool hasHeader = true;        // Whether the first row contains headers
    char delimiter = ',';         // CSV delimiter character
    std::string nullValue = "NULL"; // String representation of NULL values
    bool ignoreDuplicates = false; // Whether to ignore duplicate rows
    int batchSize = 1000;         // Number of rows to process in each batch
};
```

### DataExporter Class

#### Methods

- `ExportResult exportToFile(const std::string& filePath, const std::string& databaseName, const std::string& tableName, ExportFormat format, const ExportOptions& options)`
- `ExportResult exportToString(const std::string& databaseName, const std::string& tableName, ExportFormat format, const ExportOptions& options)`
- `ExportResult exportToSQLite(const std::string& sqlitePath, const std::string& databaseName, const std::string& tableName, const ExportOptions& options)`
- `void setDatabaseConnection(std::shared_ptr<DatabaseConnection> connection)`

#### ExportOptions Structure

```cpp
struct ExportOptions {
    bool includeHeader = true;    // Whether to include header row
    char delimiter = ',';         // CSV delimiter character
    std::string nullValue = "NULL"; // String representation of NULL values
    bool prettyPrint = false;     // Whether to pretty-print JSON
    int batchSize = 1000;         // Number of rows to process in each batch
};
```

## Command Line Interface

The import/export functionality is also available through the PhantomDB CLI:

```bash
# Import CSV data
phantomdb-cli import --format=csv --database=testdb --table=users data.csv

# Export data to JSON
phantomdb-cli export --format=json --database=testdb --table=users output.json

# Import from SQLite
phantomdb-cli import --format=sqlite --database=testdb --table=users data.db

# Export to SQLite
phantomdb-cli export --format=sqlite --database=testdb --table=users output.db
```

## Performance Considerations

- Use batch processing for large datasets
- Consider memory usage when importing/exporting large files
- Use appropriate data types to minimize storage requirements
- Monitor progress for long-running operations

## Future Enhancements

- Compression support (gzip, zip)
- Streaming import/export for very large datasets
- Parallel processing for multi-core systems
- Progress reporting and cancellation support
- Data validation and transformation rules
- Incremental import/export support
- Cloud storage integration (S3, Google Cloud, Azure)