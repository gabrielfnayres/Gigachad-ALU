CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Target executable
TARGET = mic1_interpreter

# Source files
SRCS = mic1_interpreter.cpp ../task1/alu.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up
clean:
	rm -f $(TARGET) $(OBJS) execution_log.txt

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run with specific input files
run_with_files: $(TARGET)
	./$(TARGET) -r registradores_etapa3_tarefa1 -m dados_etapa3_tarefa1.txt -i instrucoes.txt

.PHONY: all clean run run_with_files
