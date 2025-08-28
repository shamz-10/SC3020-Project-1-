# SC3020 Database Management System Project Makefile
# 
# This Makefile provides build automation for the database management system.
# It handles compilation, linking, and project organization.
# 
# Build Process:
# 1. Compile all source files with appropriate flags
# 2. Link object files into the final executable
# 3. Generate output files in the output/ directory
# 
# Usage:
#   make        - Build the project
#   make clean  - Remove all build artifacts
#   make run    - Build and run the system
# 
# This Makefile provides build targets for compiling the database system
# and managing the build process. It includes optimization flags and
# proper include paths for all source files.
#


# Compiler settings
CXX = g++                    # C++ compiler
CXXFLAGS = -std=c++11 -Wall -Wextra -O2  # Compiler flags: C++11, warnings, optimization
INCLUDES = -Isrc -Isrc/storage -Isrc/indexing -Isrc/utils  # Include paths for headers

# Source files - all C++ source files in the project
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/storage/storage.cpp \
          $(SRCDIR)/indexing/bptree.cpp \
          $(SRCDIR)/utils/parser.cpp

# Object files - compiled object files (automatically generated from sources)
OBJECTS = $(SOURCES:.cpp=.o)

# Target executable
TARGET = database_system

# Default target - builds the complete project
all: $(TARGET)

# Build the executable - links all object files into the final binary
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile source files - converts .cpp files to .o object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f output/*.bin

# Install dependencies (for macOS)
install-deps:
	brew install gcc

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CXXFLAGS += -DNDEBUG
release: $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all        - Build the project (default)"
	@echo "  clean      - Remove build files"
	@echo "  run        - Build and run the program"
	@echo "  debug      - Build with debug information"
	@echo "  release    - Build optimized release version"
	@echo "  install-deps - Install required dependencies"
	@echo "  help       - Show this help message"

.PHONY: all clean run debug release install-deps help
