#!/bin/bash
# SC3020 Database Management System - Run Script
# 
# This script automates the complete build and execution process for the
# database management system. It ensures a clean build and proper execution.
# 
# Process:
# 1. Clean previous build artifacts
# 2. Compile the project with optimization flags
# 3. Execute the database system
# 4. Display generated results
# 
# Usage: ./run_project.sh
# 
# This script compiles and runs the database system project.
# It provides a convenient way to build and execute the program.
#


echo "================================================"
echo "SC3020 Database Management System"
echo "================================================"
echo ""

# Check if we're in the correct directory
if [ ! -f "Makefile" ]; then
    echo "Error: Makefile not found. Please run this script from the project directory."
    exit 1
fi

# Clean previous build
echo "Cleaning previous build..."
make clean

# Build the project
echo "Building the project..."
make

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful! Running the database system..."
    echo "================================================"
    echo ""
    
    # Run the program
    ./database_system
    
    echo ""
    echo "================================================"
    echo "Program completed!"
    echo "Check the output/ directory for generated files."
    echo "================================================"
else
    echo ""
    echo "Build failed! Please check the error messages above."
    exit 1
fi
