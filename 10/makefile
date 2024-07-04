# Define the compiler
CXX = g++

# Define the flags for the compiler
CXXFLAGS = -Wall -pthread

# Define the source files
SRCS = server.cpp proactor.cpp

# Define the object files
OBJS = server.o proactor.o

# Define the output executable
TARGET = server

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to build the object files
server.o: server.cpp proactor.hpp
	$(CXX) $(CXXFLAGS) -c server.cpp

proactor.o: proactor.cpp proactor.hpp
	$(CXX) $(CXXFLAGS) -c proactor.cpp

# Rule to clean the build directory
clean:
	rm -f $(TARGET) $(OBJS)