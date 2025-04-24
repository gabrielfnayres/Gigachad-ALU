# Makefile para o projeto da Etapa 2/task1
# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Nome do executável
TARGET = alu_simulator

# Arquivos fonte
SRCS = main.cpp alu.cpp

# Arquivos objeto gerados a partir dos arquivos fonte
OBJS = $(SRCS:.cpp=.o)

# Arquivo de entrada padrão para testes
INPUT_FILE = programa_etapa2_tarefa1.txt
OUTPUT_FILE = saida_gerada.txt

# Regra padrão
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para compilar arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para executar o programa com o arquivo de entrada padrão
run: $(TARGET)
	./$(TARGET) $(INPUT_FILE) $(OUTPUT_FILE)

# Regra para visualizar o arquivo de saída após a execução
view: run
	cat $(OUTPUT_FILE)

# Regra para limpar arquivos temporários e o executável
clean:
	rm -f $(OBJS) $(TARGET)

# Regra para reconstruir tudo
rebuild: clean all

# Dependências
main.o: main.cpp alu.h
alu.o: alu.cpp alu.h

.PHONY: all run view clean rebuild
