# Makefile para o projeto da task3
# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Nome do executável
TARGET = mic1_simulator

# Arquivos fonte
SRCS = main.cpp mem.cpp ula.cpp

# Arquivos objeto gerados a partir dos arquivos fonte
OBJS = $(SRCS:.cpp=.o)

# Regra padrão
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para compilar arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para executar o programa com os arquivos de entrada padrão
run: $(TARGET)
	./$(TARGET)

# Regra para visualizar o arquivo de log após a execução
log: run
	cat logout.txt

# Regra para limpar arquivos temporários e o executável
clean:
	rm -f $(OBJS) $(TARGET)

# Regra para reconstruir tudo
rebuild: clean all

# Dependências
main.o: main.cpp mem.h ula.h
mem.o: mem.cpp mem.h
ula.o: ula.cpp ula.h

.PHONY: all run log clean rebuild
