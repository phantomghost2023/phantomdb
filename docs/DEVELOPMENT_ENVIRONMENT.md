# Development Environment Setup

This document provides instructions for setting up the development environment for Phantom-DB development.

## Overview

The Phantom-DB development environment is designed to support cross-platform development with a focus on performance, reliability, and developer experience. This guide covers the setup process for all supported platforms.

## System Requirements

### Minimum Requirements
- **Operating System**: Windows 10+, macOS 10.15+, or Linux (Ubuntu 20.04+, CentOS 8+)
- **CPU**: 4-core processor (8-core recommended)
- **Memory**: 8GB RAM (16GB recommended)
- **Storage**: 50GB free disk space (SSD recommended)
- **Network**: Internet connection for dependency management

### Recommended Requirements
- **Operating System**: Latest stable versions of Windows, macOS, or Linux
- **CPU**: 8-core processor or better
- **Memory**: 16GB RAM or more
- **Storage**: 100GB free SSD space
- **Network**: High-speed internet connection

## Prerequisites

### Required Software
1. **Git** (version 2.30 or later)
2. **Build Tools**:
   - Windows: Visual Studio 2022 or later with C++ tools
   - macOS: Xcode Command Line Tools
   - Linux: build-essential package
3. **Programming Languages**:
   - C++ (C++17 or later)
   - Python (3.8 or later) for build scripts and tools
   - Rust (latest stable) for performance-critical components
4. **Package Managers**:
   - Conan (C++ package manager)
   - pip (Python package manager)
5. **Containerization** (Optional but recommended):
   - Docker (20.10 or later)
   - Docker Compose (1.29 or later)

### Optional Software
1. **IDE/Editor**:
   - Visual Studio Code with C++ extensions
   - CLion
   - Visual Studio
2. **Performance Analysis Tools**:
   - Valgrind (Linux/macOS)
   - Visual Studio Profiler (Windows)
3. **Documentation Tools**:
   - Doxygen
   - Graphviz (for diagrams)

## Setup Instructions

### 1. Clone the Repository
```bash
git clone https://github.com/your-org/phantom-db.git
cd phantom-db
```

### 2. Install Required Software

#### Windows
1. Install Git from https://git-scm.com/
2. Install Visual Studio 2022 with C++ development tools
3. Install Python from https://python.org/
4. Install Rust from https://rust-lang.org/
5. Install Conan:
   ```bash
   pip install conan
   ```

#### macOS
1. Install Xcode Command Line Tools:
   ```bash
   xcode-select --install
   ```
2. Install Homebrew:
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```
3. Install required tools:
   ```bash
   brew install git python rust
   pip install conan
   ```

#### Linux (Ubuntu/Debian)
1. Update package list:
   ```bash
   sudo apt update
   ```
2. Install required tools:
   ```bash
   sudo apt install git build-essential python3 python3-pip rustc
   pip3 install conan
   ```

### 3. Configure Conan
```bash
conan profile new default --detect
conan profile update settings.compiler.cppstd=17 default
```

### 4. Install Dependencies
```bash
cd phantom-db
conan install . --build=missing
```

### 5. Build the Project
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .
```

## Project Structure

After setup, the project directory will have the following structure:
```
phantom-db/
├── build/                 # Build artifacts (generated)
├── cmake/                 # CMake configuration files
├── conanfile.txt          # Conan dependencies
├── docs/                  # Documentation
├── scripts/               # Build and utility scripts
├── src/                   # Source code
│   ├── core/              # Core architecture components
│   ├── storage/           # Storage engine implementation
│   ├── query/             # Query processor
│   ├── transaction/       # Transaction system
│   ├── distributed/       # Distributed systems components
│   ├── tools/             # Administration tools
│   └── api/               # API interfaces
├── tests/                 # Test suite
├── third_party/           # Third-party libraries
└── CMakeLists.txt         # Main CMake configuration
```

## Development Workflow

### 1. Branching Strategy
- **main**: Production-ready code
- **develop**: Integration branch for ongoing development
- **feature/***: Feature-specific branches
- **hotfix/***: Urgent production fixes
- **release/***: Release preparation branches

### 2. Building the Project
```bash
# Incremental build
cd build
cmake --build .

# Clean build
cd ..
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

### 3. Running Tests
```bash
# Run all tests
cd build
ctest

# Run specific test suite
ctest -R storage_engine_tests

# Run with verbose output
ctest -V
```

### 4. Code Formatting
```bash
# Format code using clang-format
scripts/format-code.sh

# Check formatting without changes
scripts/check-format.sh
```

### 5. Static Analysis
```bash
# Run static analysis
scripts/static-analysis.sh
```

## IDE Configuration

### Visual Studio Code
1. Install recommended extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - Python (Microsoft)
   - Rust (rust-lang)

2. Configure settings in `.vscode/settings.json`:
```json
{
    "cmake.configureOnOpen": true,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "editor.formatOnSave": true,
    "python.defaultInterpreterPath": "./venv/bin/python"
}
```

### CLion
1. Open the project folder in CLion
2. Select the CMakeLists.txt file when prompted
3. Configure toolchain to use the installed compiler
4. Enable automatic reloading of CMake projects

### Visual Studio
1. Open the CMakeLists.txt file in Visual Studio
2. Select the appropriate configuration (Debug/Release)
3. Build using the standard build process

## Testing Framework

### Unit Testing
- **Framework**: Google Test
- **Coverage**: Target > 90% code coverage
- **Execution**: Integrated with CTest

### Integration Testing
- **Framework**: Custom test framework
- **Scope**: Component interaction testing
- **Execution**: CTest integration

### Performance Testing
- **Framework**: Google Benchmark
- **Metrics**: Throughput, latency, resource usage
- **Execution**: Dedicated performance test suite

### Stress Testing
- **Framework**: Custom stress testing tools
- **Scope**: Concurrency, resource limits, failure scenarios
- **Execution**: Manual and automated execution

## Continuous Integration

### GitHub Actions
The project uses GitHub Actions for CI with the following workflows:
- **Build and Test**: Runs on every push and pull request
- **Code Quality**: Static analysis and formatting checks
- **Performance**: Performance regression testing
- **Security**: Security scanning and vulnerability checks

### Local CI Simulation
```bash
# Run the full CI pipeline locally
scripts/ci-simulation.sh
```

## Debugging

### Debugging Tools
- **GDB** (Linux/macOS)
- **LLDB** (macOS)
- **Visual Studio Debugger** (Windows)

### Debug Builds
```bash
# Create debug build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Logging
- **Framework**: spdlog
- **Levels**: Trace, Debug, Info, Warning, Error, Critical
- **Configuration**: Runtime-configurable logging levels

## Performance Profiling

### Tools
- **Linux**: perf, Valgrind
- **macOS**: Instruments, Shark
- **Windows**: Visual Studio Profiler, WPA

### Profiling Builds
```bash
# Create profiling build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build .
```

## Documentation

### Code Documentation
- **Style**: Doxygen-style comments
- **Generation**: Automated with Doxygen
- **Location**: docs/api/

### Technical Documentation
- **Format**: Markdown
- **Location**: docs/
- **Generation**: Manual updates with periodic reviews

## Troubleshooting

### Common Issues

#### Conan Installation Issues
```bash
# Clear Conan cache
conan remove "*" --force

# Update Conan
pip install --upgrade conan
```

#### CMake Configuration Issues
```bash
# Clear CMake cache
cd build
rm -rf *
cmake ..
```

#### Build Failures
```bash
# Clean build
cd ..
rm -rf build
mkdir build
cd build
cmake ..
cmake --build .
```

#### Test Failures
```bash
# Run tests with verbose output
ctest -V

# Run specific test with debugger
gdb ./tests/storage_engine_tests
```

## Contributing

### Code Review Process
1. Create a feature branch from develop
2. Implement changes with tests
3. Ensure all tests pass
4. Create pull request to develop
5. Address review feedback
6. Merge after approval

### Coding Standards
- **Style**: Google C++ Style Guide
- **Naming**: Descriptive names with clear purpose
- **Comments**: Explain why, not what
- **Documentation**: Document public APIs

### Commit Guidelines
- **Message Format**: "component: brief description"
- **Scope**: Focus on single logical change
- **Size**: Keep commits reasonably sized
- **Frequency**: Commit frequently with clear messages

## Next Steps

After completing the setup:
1. Verify the build process completes successfully
2. Run the test suite to ensure everything works
3. Review the project structure and documentation
4. Begin work on your assigned tasks
5. Join the development team communication channels

This setup guide provides everything needed to begin contributing to Phantom-DB development. If you encounter any issues during setup, please consult the troubleshooting section or reach out to the development team for assistance.