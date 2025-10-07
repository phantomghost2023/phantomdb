# PhantomDB Code Coverage Reports

This document describes how to generate and interpret code coverage reports for PhantomDB.

## Overview

Code coverage is a metric that measures the degree to which the source code of a program is executed when a particular test suite runs. It helps identify untested parts of the codebase and ensures quality.

## Tools

PhantomDB supports code coverage reporting using different tools depending on the platform:

### Windows (Visual Studio)
- **OpenCppCoverage**: A C++ code coverage tool for Windows
- Generates HTML reports and detailed coverage statistics

### Linux/macOS (GCC/Clang)
- **gcov/lcov**: Standard GNU tools for code coverage
- Generates HTML reports with detailed line-by-line coverage

## Setup

### Windows Setup

1. Install OpenCppCoverage from: https://github.com/OpenCppCoverage/OpenCppCoverage
2. Ensure it's available in your PATH

### Linux/macOS Setup

1. Ensure gcov and lcov are installed:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install lcov
   
   # macOS
   brew install lcov
   ```

## Generating Coverage Reports

### Windows with OpenCppCoverage

```cmd
# Build with debug information
cmake -DCMAKE_BUILD_TYPE=Debug .
cmake --build . --config Debug

# Run coverage analysis
OpenCppCoverage.exe --sources src --export_type html:coverage_report -- .
```

### Linux/macOS with gcov/lcov

```bash
# Build with coverage flags
cmake -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON .
make

# Run tests to generate coverage data
ctest

# Generate HTML report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## Interpreting Results

The coverage report will show:
- Line coverage: Percentage of lines executed
- Function coverage: Percentage of functions called
- Branch coverage: Percentage of branches taken

Aim for:
- Line coverage: >90%
- Function coverage: >85%
- Branch coverage: >80%

## Coverage Goals

1. Core database functionality: 100% coverage
2. Storage engine components: 95% coverage
3. Query processor: 90% coverage
4. Transaction manager: 95% coverage
5. Distributed components: 85% coverage
6. API components: 90% coverage

## Integration with CI

Coverage reports should be generated as part of the continuous integration pipeline and published to coverage tracking services like Codecov or Coveralls.