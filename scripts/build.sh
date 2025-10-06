#!/bin/bash

# Build script for PhantomDB

# Exit on any error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building PhantomDB...${NC}"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir build
fi

# Change to build directory
cd build

# Install dependencies with Conan
echo -e "${YELLOW}Installing dependencies...${NC}"
conan install .. --build=missing

# Configure with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake ..

# Build the project
echo -e "${YELLOW}Building project...${NC}"
cmake --build .

echo -e "${GREEN}Build completed successfully!${NC}"

# Run tests if requested
if [ "$1" == "test" ]; then
    echo -e "${YELLOW}Running tests...${NC}"
    ctest --output-on-failure
fi