#!/bin/bash

# CinemaStudio Build Script
# This script builds the CinemaStudio video editor

set -e

echo "================================================"
echo "  CinemaStudio - Modern Video Editor"
echo "  Build Script"
echo "================================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: CMakeLists.txt not found!${NC}"
    echo "Please run this script from the CinemaStudio root directory."
    exit 1
fi

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p build
cd build

# Run CMake
echo -e "${YELLOW}Running CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo -e "${YELLOW}Building CinemaStudio...${NC}"
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}================================================${NC}"
    echo -e "${GREEN}  Build Successful!${NC}"
    echo -e "${GREEN}================================================${NC}"
    echo ""
    echo "Executable location: $(pwd)/CinemaStudio"
    echo ""
    echo "To run CinemaStudio:"
    echo "  ./build/CinemaStudio"
    echo ""
else
    echo ""
    echo -e "${RED}================================================${NC}"
    echo -e "${RED}  Build Failed!${NC}"
    echo -e "${RED}================================================${NC}"
    exit 1
fi
