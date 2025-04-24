CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = alu_simulator
SRCS = main.cpp alu.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET) $(COMPARE_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(COMPARE_TARGET): compare_outputs.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(COMPARE_TARGET)

run: $(TARGET)
	./$(TARGET) programa_etapa1.txt output.txt


test: run compare

.PHONY: all clean run compare test
