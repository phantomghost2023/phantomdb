#!/bin/bash
# PhantomDB Code Coverage Report Generator (Linux/macOS)
# This script generates code coverage reports using gcov/lcov

echo "Generating PhantomDB Code Coverage Report..."

# Check if required tools are available
if ! command -v lcov &> /dev/null; then
    echo "Error: lcov not found. Please install lcov:"
    echo "  Ubuntu/Debian: sudo apt-get install lcov"
    echo "  macOS: brew install lcov"
    exit 1
fi

if ! command -v gcov &> /dev/null; then
    echo "Error: gcov not found. Please install gcc/gcov"
    exit 1
fi

# Create build directory for coverage
mkdir -p coverage_build
cd coverage_build

# Configure with coverage flags
cmake -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON ..
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    cd ..
    exit 1
fi

# Build the project
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    cd ..
    exit 1
fi

# Clean any existing coverage data
lcov --zerocounters --directory .
lcov --capture --initial --directory . --output-file base.info

# Run tests
echo "Running tests..."
ctest
if [ $? -ne 0 ]; then
    echo "Warning: Some tests failed"
fi

# Capture coverage data
lcov --capture --directory . --output-file test.info
lcov --add-tracefile base.info --add-tracefile test.info --output-file coverage.info

# Remove system and test files from coverage
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/test_*' --output-file coverage.info.cleaned

# Generate HTML report
genhtml coverage.info.cleaned --output-directory ../coverage_report

cd ..

echo ""
echo "Coverage report generated in coverage_report directory"
echo "Open coverage_report/index.html in your browser to view the report"