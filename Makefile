# Compiler and Flags
CXX = g++
# -DMEMTRACE defines the macro for memory tracking
# -Wall -Werror -pedantic are for error checking
CXXFLAGS = -Wall -Werror -pedantic -std=c++11 -DMEMTRACE -MMD -MP

# Source files (used in both main and tests)
COMMON_SRCS = string.cpp memtrace.cpp vector.cpp filesystem_element.cpp directory.cpp file.cpp link.cpp
COMMON_OBJS = $(COMMON_SRCS:.cpp=.o)

# Target executable names
TARGET = fs_sim
TEST_TARGET = fs_sim_test

# Default target
all: $(TARGET)

# Compile the production version (using main.cpp)
$(TARGET): main.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(COMMON_OBJS)

# Compile the test version (using tests.cpp)
test: $(TEST_TARGET)

$(TEST_TARGET): tests.o $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) tests.o $(COMMON_OBJS)

# Generic rule for creating object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f *.o *.d $(TARGET) $(TEST_TARGET)

-include $(COMMON_OBJS:.o=.d) main.d tests.d

.PHONY: all test clean