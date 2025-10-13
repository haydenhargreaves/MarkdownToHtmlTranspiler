# Define the C++ compiler and flags
CXX = g++
CXXFLAGS = -Wall -g

# Directories
BUILD_DIR = build
SRC_DIR = src
LIB_DIR = lib

# Executable name
TARGET = parser

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
LIB_FILES := $(wildcard $(LIB_DIR)/*.cpp)
ALL_SOURCES = $(SRC_FILES) $(LIB_FILES)

# Generate object file paths in the build directory
OBJECTS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(ALL_SOURCES)))

# Include directories
INCLUDES = -I$(LIB_DIR) -I$(SRC_DIR)

.PHONY: all clean test

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(LIB_DIR)/parser.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/parser.o: $(LIB_DIR)/parser.cpp $(LIB_DIR)/parser.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: all
	./$(TARGET)
	./$(TARGET) '   '
	./$(TARGET) ./test/input.md
	./$(TARGET) ./test/input.md ./test/output.html

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
