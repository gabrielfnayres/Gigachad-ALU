#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cstdint>
#include "alu.h"
#include <limits>

// Define os tipos dos registradores
typedef uint32_t reg32_t;
typedef uint8_t reg8_t;

// Registradores
reg32_t H = 0;
reg32_t OPC = 0;
reg32_t TOS = 0;
reg32_t CPP = 0;
reg32_t LV = 0;
reg32_t SP = 0;
reg32_t PC = 0;
reg32_t MDR = 0;
reg32_t MAR = 0;
reg8_t MBR = 0;

// Registrador de Instrução (usando string para armazenar a instrução de 21 bits)
std::string IR = "";
// Armazenamento das instruções do programa
std::vector<std::string> program;

// Função auxiliar para converter string binária para inteiro sem sinal
uint32_t bin_to_uint32(const std::string& bin_str) {
    try {
        return std::stoul(bin_str, nullptr, 2);
    } catch (const std::invalid_argument& ia) {
        std::cerr << "Argumento inválido para conversão binária: " << bin_str << std::endl;
        return 0;
    } catch (const std::out_of_range& oor) {
        std::cerr << "Valor fora do intervalo para conversão binária: " << bin_str << std::endl;
        return 0;
    }
}

uint8_t bin_to_uint8(const std::string& bin_str) {
    try {
        return static_cast<uint8_t>(std::stoul(bin_str, nullptr, 2));
    } catch (const std::invalid_argument& ia) {
        std::cerr << "Argumento inválido para conversão binária: " << bin_str << std::endl;
        return 0;
    } catch (const std::out_of_range& oor) {
        std::cerr << "Valor fora do intervalo para conversão binária: " << bin_str << std::endl;
        return 0;
    }
}

// Função auxiliar para converter uint32_t para string binária de 32 bits
std::string uint32_to_bin_string(reg32_t val) {
    return std::bitset<32>(val).to_string();
}

// Função auxiliar para extensão de sinal (8 bits para 32 bits)
reg32_t sign_extend_8_to_32(reg8_t val) {
    // Verifica se o bit de sinal (bit 7) está setado
    if (val & 0x80) {
        // Extensão de sinal: preenche os 24 bits superiores com 1s
        return static_cast<reg32_t>(val) | 0xFFFFFF00;
    } else {
        // Extensão com zeros: os 24 bits superiores são 0s (implícito na conversão)
        return static_cast<reg32_t>(val);
    }
}

// Função para carregar os valores iniciais dos registradores
bool load_initial_registers(const std::string& filename) {
    std::ifstream reg_file(filename);
    if (!reg_file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de registradores: " << filename << std::endl;
        return false;
    }

    std::string line, reg_name, eq, value_str;
    while (std::getline(reg_file, line)) {
        // Remove espaços
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        if (line.empty()) continue;

        std::stringstream ss(line);
        if (std::getline(ss, reg_name, '=') && std::getline(ss, value_str)) {
            if (reg_name == "mar") MAR = bin_to_uint32(value_str);
            else if (reg_name == "mdr") MDR = bin_to_uint32(value_str);
            else if (reg_name == "pc") PC = bin_to_uint32(value_str);
            else if (reg_name == "mbr") MBR = bin_to_uint8(value_str);
            else if (reg_name == "sp") SP = bin_to_uint32(value_str);
            else if (reg_name == "lv") LV = bin_to_uint32(value_str);
            else if (reg_name == "cpp") CPP = bin_to_uint32(value_str);
            else if (reg_name == "tos") TOS = bin_to_uint32(value_str);
            else if (reg_name == "opc") OPC = bin_to_uint32(value_str);
            else if (reg_name == "h") H = bin_to_uint32(value_str);
            else {
                std::cerr << "Aviso: Nome de registrador desconhecido no arquivo: " << reg_name << std::endl;
            }
        }
    }
    reg_file.close();
    return true;
}

// Função para carregar as instruções do programa
bool load_program(const std::string& filename) {
    std::ifstream prog_file(filename);
    if (!prog_file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo do programa: " << filename << std::endl;
        return false;
    }

    std::string instruction;
    while (std::getline(prog_file, instruction)) {
        // Remove espaços, se houver
        instruction.erase(std::remove(instruction.begin(), instruction.end(), ' '), instruction.end());
        if (instruction.length() == 21) { // Valida o comprimento da instrução
            program.push_back(instruction);
        } else if (!instruction.empty()) {
             std::cerr << "Aviso: Instrução com comprimento inválido ignorada: " << instruction << " (comprimento " << instruction.length() << ")" << std::endl;
        }
    }
    prog_file.close();
     if (program.empty()) {
        std::cerr << "Erro: Nenhuma instrução válida carregada do arquivo " << filename << std::endl;
        return false;
    }
    return true;
}

// Função auxiliar para imprimir os registradores no formato requerido
void print_registers(const std::string& state) {
    std::cout << "> Registradores " << state << " instrução" << std::endl;
    std::cout << "mar = " << std::bitset<32>(MAR) << std::endl;
    std::cout << "mdr = " << std::bitset<32>(MDR) << std::endl;
    std::cout << "pc = " << std::bitset<32>(PC) << std::endl;
    std::cout << "mbr = " << std::bitset<8>(MBR) << std::endl;
    std::cout << "sp = " << std::bitset<32>(SP) << std::endl;
    std::cout << "lv = " << std::bitset<32>(LV) << std::endl;
    std::cout << "cpp = " << std::bitset<32>(CPP) << std::endl;
    std::cout << "tos = " << std::bitset<32>(TOS) << std::endl;
    std::cout << "opc = " << std::bitset<32>(OPC) << std::endl;
    std::cout << "h = " << std::bitset<32>(H) << std::endl;
}

int main() {
    if (!load_initial_registers("registradores_etapa2_tarefa2.txt")) {
        return 1;
    }

    if (!load_program("programa_etapa2_tarefa2.txt")) {
        return 1;
    }

    std::cout << "Registradores iniciais carregados. Programa carregado com " << program.size() << " instruções." << std::endl;
    print_registers("iniciais"); // Imprime o estado inicial
    std::cout << "=====================================================" << std::endl; // Separador após o estado inicial

    // Loop principal de simulação
    bool n_flag = false;
    bool z_flag = false;
    Alu mic1_alu; // Instancia o objeto ALU uma vez fora do loop

    for (int cycle = 0; cycle < program.size(); ++cycle) {
        IR = program[cycle];

        std::cout << "Ciclo " << cycle +1 << std::endl; // A contagem de ciclos começa em 1 na saída de exemplo
        std::cout << "ir = " << IR.substr(0, 8) << " " << IR.substr(8, 9) << " " << IR.substr(17, 4) << std::endl << std::endl;

        // 1. Analisa a Instrução (21 bits)
        // Garante que o IR tenha o comprimento correto antes de substr
        if (IR.length() != 21) {
             std::cerr << "Erro: Instrução corrompida encontrada durante a execução: " << IR << std::endl;
             continue; // Pula este ciclo
        }
        std::string alu_ctrl_str = IR.substr(0, 8);
        std::string c_bus_sel_str = IR.substr(8, 9);
        std::string b_bus_dec_str = IR.substr(17, 4);

        uint8_t alu_control = bin_to_uint8(alu_ctrl_str);
        uint16_t c_bus_sel = static_cast<uint16_t>(bin_to_uint32(c_bus_sel_str)); // Usa uint32 por segurança, depois converte
        uint8_t b_bus_dec = bin_to_uint8(b_bus_dec_str);

        // Imprime registradores ANTES da execução
        print_registers("antes da");
        std::cout << std::endl;

        // 2. Decodifica a Fonte do Barramento B
        reg32_t b_bus_val = 0;
        std::string b_bus_reg_name = "<inválido>";

        switch (b_bus_dec) {
            case 0: b_bus_val = MDR; b_bus_reg_name = "mdr"; break;
            case 1: b_bus_val = PC; b_bus_reg_name = "pc"; break;
            case 2: b_bus_val = sign_extend_8_to_32(MBR); b_bus_reg_name = "mbr"; break; // MBR com sinal
            case 3: b_bus_val = static_cast<reg32_t>(MBR); b_bus_reg_name = "mbru"; break; // MBR sem sinal
            case 4: b_bus_val = SP; b_bus_reg_name = "sp"; break;
            case 5: b_bus_val = LV; b_bus_reg_name = "lv"; break;
            case 6: b_bus_val = CPP; b_bus_reg_name = "cpp"; break;
            case 7: b_bus_val = TOS; b_bus_reg_name = "tos"; break;
            case 8: b_bus_val = OPC; b_bus_reg_name = "opc"; break;
            default: std::cerr << "Erro: Seletor de barramento B inválido: " << static_cast<int>(b_bus_dec) << std::endl; break;
        }

        std::cout << "b_bus = " << b_bus_reg_name << std::endl;

        // 3. Executa a Operação da ALU
        reg32_t alu_a = H; // A entrada A é sempre H
        reg32_t alu_b = b_bus_val;

        // Configura entradas e controles para o objeto Alu
        mic1_alu.setA(uint32_to_bin_string(alu_a));
        mic1_alu.setB(uint32_to_bin_string(alu_b));
        mic1_alu.setSLL8((alu_control >> 7) & 1);
        mic1_alu.setSRA1((alu_control >> 6) & 1);
        mic1_alu.setF0((alu_control >> 5) & 1);
        mic1_alu.setF1((alu_control >> 4) & 1);
        mic1_alu.setEna((alu_control >> 3) & 1);
        mic1_alu.setEnb((alu_control >> 2) & 1);
        mic1_alu.setInva((alu_control >> 1) & 1);
        mic1_alu.setInc((alu_control >> 0) & 1);

        // Executa a operação
        mic1_alu.execute_operation();

        // Obtém o resultado e as flags
        reg32_t alu_result = bin_to_uint32(mic1_alu.getS());
        n_flag = mic1_alu.getN();
        z_flag = mic1_alu.getZ();

        // 4. Escreve nos Destinos do Barramento C
        std::string c_bus_dest_names = "";
        if ((c_bus_sel >> 0) & 1) { MAR = alu_result; c_bus_dest_names += "mar "; }
        if ((c_bus_sel >> 1) & 1) { MDR = alu_result; c_bus_dest_names += "mdr "; }
        if ((c_bus_sel >> 2) & 1) { PC = alu_result; c_bus_dest_names += "pc "; }
        if ((c_bus_sel >> 3) & 1) { SP = alu_result; c_bus_dest_names += "sp "; }
        if ((c_bus_sel >> 4) & 1) { LV = alu_result; c_bus_dest_names += "lv "; }
        if ((c_bus_sel >> 5) & 1) { CPP = alu_result; c_bus_dest_names += "cpp "; }
        if ((c_bus_sel >> 6) & 1) { TOS = alu_result; c_bus_dest_names += "tos "; }
        if ((c_bus_sel >> 7) & 1) { OPC = alu_result; c_bus_dest_names += "opc "; }
        if ((c_bus_sel >> 8) & 1) { H = alu_result; c_bus_dest_names += "h "; }

         // Remove o espaço final se algum nome foi adicionado
        if (!c_bus_dest_names.empty()) {
            c_bus_dest_names.pop_back();
        }

        std::cout << "c_bus = " << (c_bus_dest_names.empty() ? "<nenhum>" : c_bus_dest_names) << std::endl << std::endl;

        // Imprime registradores APÓS a execução
        print_registers("após a");
        std::cout << "=====================================================" << std::endl;
    }
    // Fim do loop de simulação

    return 0;
}