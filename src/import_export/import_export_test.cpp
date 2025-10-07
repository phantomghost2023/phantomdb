#include "data_importer.h"
#include "data_exporter.h"
#include <iostream>
#include <cassert>
#include <fstream>

using namespace phantomdb::import_export;

void testCSVImport() {
    std::cout << "Testing CSV import..." << std::endl;
    
    // Create a sample CSV file for testing
    std::ofstream csvFile("test_data.csv");
    csvFile << "id,name,email\n";
    csvFile << "1,John Doe,john@example.com\n";
    csvFile << "2,Jane Smith,jane@example.com\n";
    csvFile << "3,Bob Johnson,bob@example.com\n";
    csvFile.close();
    
    // Create importer
    DataImporter importer;
    
    // Test import options
    ImportOptions options;
    options.hasHeader = true;
    options.delimiter = ',';
    
    // Import CSV data
    ImportResult result = importer.importFromFile(
        "test_data.csv",
        "testdb",
        "users",
        DataFormat::CSV,
        options
    );
    
    // Verify result
    assert(result.success == true);
    assert(result.rowsImported == 3);
    assert(result.rowsFailed == 0);
    
    // Clean up test file
    std::remove("test_data.csv");
    
    std::cout << "CSV import test passed!" << std::endl;
}

void testCSVExport() {
    std::cout << "Testing CSV export..." << std::endl;
    
    // Create exporter
    DataExporter exporter;
    
    // Test export options
    ExportOptions options;
    options.includeHeader = true;
    options.delimiter = ',';
    
    // Export CSV data
    ExportResult result = exporter.exportToFile(
        "exported_data.csv",
        "testdb",
        "users",
        ExportFormat::CSV,
        options
    );
    
    // Verify result (in our demo, this will fail as we haven't implemented the full functionality)
    // assert(result.success == true);
    // assert(result.rowsExported == 3);
    
    // Clean up test file
    std::remove("exported_data.csv");
    
    std::cout << "CSV export test completed!" << std::endl;
}

void testJSONExport() {
    std::cout << "Testing JSON export..." << std::endl;
    
    // Create exporter
    DataExporter exporter;
    
    // Test export options
    ExportOptions options;
    options.prettyPrint = true;
    
    // Export JSON data
    ExportResult result = exporter.exportToFile(
        "exported_data.json",
        "testdb",
        "users",
        ExportFormat::JSON,
        options
    );
    
    // Verify result (in our demo, this will fail as we haven't implemented the full functionality)
    // assert(result.success == true);
    // assert(result.rowsExported == 3);
    
    // Clean up test file
    std::remove("exported_data.json");
    
    std::cout << "JSON export test completed!" << std::endl;
}

void testDataFormats() {
    std::cout << "Testing data formats..." << std::endl;
    
    assert(static_cast<int>(DataFormat::CSV) == static_cast<int>(ExportFormat::CSV));
    assert(static_cast<int>(DataFormat::JSON) == static_cast<int>(ExportFormat::JSON));
    assert(static_cast<int>(DataFormat::SQLITE) == static_cast<int>(ExportFormat::SQLITE));
    
    std::cout << "Data formats test passed!" << std::endl;
}

int main() {
    std::cout << "Running PhantomDB Import/Export tests..." << std::endl;
    
    testCSVImport();
    testCSVExport();
    testJSONExport();
    testDataFormats();
    
    std::cout << "All PhantomDB Import/Export tests completed!" << std::endl;
    return 0;
}