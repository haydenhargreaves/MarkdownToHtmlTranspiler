# Define the C++ compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -fPIE

# Directories
BUILD_DIR = build
SRC_DIR = src
LIB_DIR = lib

# Executable name
TARGET = parser

# Automatically find all source files
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
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ -pie

# Generic rule for all .cpp files in the src/ directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -pie

# Generic rule for all .cpp files in the lib/ directory
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -pie

test: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
