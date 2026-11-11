# Compiler and Flags
CXX = g++
# -DMEMTRACE defines the macro for memory tracking
# -Wall -Werror -pedantic are for error checking
CXXFLAGS = -Wall -Werror -pedantic -std=c++11 -DMEMTRACE -MMD -MP

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files (used in both main and tests)
COMMON_SRCS = string.cpp memtrace.cpp vector.cpp filesystem_element.cpp directory.cpp file.cpp link.cpp
COMMON_OBJS = $(addprefix $(BUILD_DIR)/,$(COMMON_SRCS:.cpp=.o))

# Target executable names
TARGET = fs_sim
TEST_TARGET = fs_sim_test

# Default target
all: $(TARGET)

# Compile the production version (using main.cpp)
$(TARGET): $(BUILD_DIR)/main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile the test version (using tests.cpp)
test: $(TEST_TARGET)

$(TEST_TARGET): $(BUILD_DIR)/tests.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Generic rule for creating object files in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all test clean
