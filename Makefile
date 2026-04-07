# Compiler and Flags
CXX = g++
CXXFLAGS = -g -std=c++11 -Wall

# Automatically find ALL .cpp files in the directory
# This ensures Vec3.cpp, Matrix4.cpp, Color.cpp, etc. are included
SRCS = $(wildcard *.cpp)

# Generate object file names from source file names
OBJS = $(SRCS:.cpp=.o)

# Output executable name
TARGET = rasterizer

# Default target
all: $(TARGET)

# Linking stage
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Compilation stage
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET) *.ppm