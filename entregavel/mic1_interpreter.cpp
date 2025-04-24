#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <iomanip>
#include <tuple>
#include <cstdint>
#include "../task1/alu.h"

// Estrutura para representar uma microinstrução
struct MicroInstruction {
    std::string binary; // Microinstrução de 23 bits para a arquitetura Mic-1
    std::string description; // Descrição do que a microinstrução faz
};

// Estrutura para representar um conjunto de registradores
struct Registers {
    int32_t H;     // 32 bits
    int32_t OPC;   // 32 bits
    int32_t TOS;   // 32 bits
    int32_t CPP;   // 32 bits
    int32_t LV;    // 32 bits
    int32_t SP;    // 32 bits
    int8_t MBR;    // 8 bits
    int32_t PC;    // 32 bits
    int32_t MDR;   // 32 bits
    int32_t MAR;   // 32 bits
    bool N;        // Flag de negativo
    bool Z;        // Flag de zero
};

// Classe para manipular o interpretador Mic-1
class Mic1Interpreter {
private:
    Registers regs;
    std::vector<int32_t> memory;
    std::ofstream logFile;

    // Mapas para armazenar sequências de microinstruções para cada instrução
    std::map<std::string, std::vector<MicroInstruction>> instructionMap;

public:
    Mic1Interpreter() {
        // Inicializa os registradores com 0
        regs.H = 0;
        regs.OPC = 0;
        regs.TOS = 0;
        regs.CPP = 0;
        regs.LV = 0;
        regs.SP = 0;
        regs.MBR = 0;
        regs.PC = 0;
        regs.MDR = 0;
        regs.MAR = 0;
        regs.N = false;
        regs.Z = false;
        
        // Inicializa o mapa de instruções com sequências de microinstruções
        initializeInstructionMap();
    }

    // Initialize the instruction map with microinstruction sequences
    void initializeInstructionMap() {
        // Instrução ILOAD
        instructionMap["ILOAD"] = {
            {"00110100100000000000101", "H = LV"},                 // H = LV
            {"00111000000000001010000", "MAR = H; rd"},           // MAR = H; rd
            {"00111001100000000000000", "H = H+1"},               // H = H+1
            {"00110101000001001100100", "MAR = SP = SP+1; wr"},   // MAR = SP = SP+1; wr
            {"00110100001000000000000", "TOS = MDR"}              // TOS = MDR
        };

        // Instrução DUP
        instructionMap["DUP"] = {
            {"00110100000000010100111", "MDR = TOS; wr"},          // MDR = TOS; wr
            {"00110101000001001100100", "MAR = SP = SP+1; wr"},   // MAR = SP = SP+1; wr
            {"00111001100000000000000", "PC = PC + 1"}            // PC = PC + 1
        };

        // Instrução BIPUSH
        instructionMap["BIPUSH"] = {
            {"00111001100000000000000", "PC = PC + 1"},            // PC = PC + 1
            {"00000000000000000000000", "MBR = byte"},             // Set MBR to the byte value
            {"00110101000001001000100", "SP = MAR = SP+1"},        // SP = MAR = SP+1
            {"00001000001000010101000", "MDR = TOS = H; wr"}      // MDR = TOS = H; wr
        };
    }

    // Carrega os valores iniciais dos registradores a partir de um arquivo
    bool loadRegisters(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open register file: " << filename << std::endl;
            return false;
        }
        
        logFile << "*******************************" << std::endl;
        logFile << "Initial register state" << std::endl;
        logFile << "*******************************" << std::endl;
        
        std::string line;
        while (std::getline(file, line)) {
            // Ignora linhas vazias
            if (line.empty()) continue;
            
            // Registra o valor do registrador no log
            logFile << line << std::endl;
            
            // Analisa o nome e o valor do registrador
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string regName = line.substr(0, pos);
                std::string regValue = line.substr(pos + 1);
                
                // Remove espaços em branco
                regName.erase(0, regName.find_first_not_of(" \t"));
                regName.erase(regName.find_last_not_of(" \t") + 1);
                regValue.erase(0, regValue.find_first_not_of(" \t"));
                regValue.erase(regValue.find_last_not_of(" \t") + 1);
                
                // Define o valor do registrador
                if (regName == "mar") regs.MAR = std::bitset<32>(regValue).to_ulong();
                else if (regName == "mdr") regs.MDR = std::bitset<32>(regValue).to_ulong();
                else if (regName == "pc") regs.PC = std::bitset<32>(regValue).to_ulong();
                else if (regName == "mbr") regs.MBR = std::bitset<8>(regValue).to_ulong();
                else if (regName == "sp") regs.SP = std::bitset<32>(regValue).to_ulong();
                else if (regName == "lv") regs.LV = std::bitset<32>(regValue).to_ulong();
                else if (regName == "cpp") regs.CPP = std::bitset<32>(regValue).to_ulong();
                else if (regName == "tos") regs.TOS = std::bitset<32>(regValue).to_ulong();
                else if (regName == "opc") regs.OPC = std::bitset<32>(regValue).to_ulong();
                else if (regName == "h") regs.H = std::bitset<32>(regValue).to_ulong();
            }
        }
        
        file.close();
        
        logFile << "============================================================" << std::endl;
        logFile << "Start of Program" << std::endl;
        logFile << "============================================================" << std::endl;
        
        return true;
    }

    // Carrega os valores iniciais da memória a partir de um arquivo
    bool loadMemory(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open memory file: " << filename << std::endl;
            return false;
        }
        
        logFile << "============================================================" << std::endl;
        logFile << "Initial memory state" << std::endl;
        logFile << "*******************************" << std::endl;
        
        std::string line;
        int address = 0;
        
        while (std::getline(file, line)) {
            // Ignora linhas vazias
            if (line.empty()) continue;
            
            // Registra o valor da memória no log
            logFile << line << std::endl;
            
            // Converte a string binária para inteiro
            memory.push_back(std::bitset<32>(line).to_ulong());
            address++;
        }
        
        file.close();
        return true;
    }

    // Analisa e executa instruções a partir de um arquivo
    bool executeInstructions(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open instruction file: " << filename << std::endl;
            return false;
        }

        // Abre o arquivo de log
        logFile.open("execution_log.txt");
        if (!logFile.is_open()) {
            std::cerr << "Error: Could not create log file" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Remove espaços em branco
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;

            logFile << "Executing instruction: " << line << std::endl;
            logFile << "----------------------------------------" << std::endl;

            // Analisa a instrução
            std::string instruction, argument;
            size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                instruction = line.substr(0, pos);
                argument = line.substr(pos + 1);
                // Remove espaços em branco
                argument.erase(0, argument.find_first_not_of(" \t"));
                argument.erase(argument.find_last_not_of(" \t") + 1);
            } else {
                instruction = line;
                argument = "";
            }

            // Executa a instrução
            if (instruction == "ILOAD") {
                executeILOAD(argument);
            } else if (instruction == "DUP") {
                executeDUP();
            } else if (instruction == "BIPUSH") {
                executeBIPUSH(argument);
            } else {
                logFile << "Unknown instruction: " << instruction << std::endl;
            }

            // Registra o estado da memória após a execução da instrução
            logMemoryState();
            logFile << "----------------------------------------" << std::endl << std::endl;
        }

        file.close();
        logFile.close();
        return true;
    }

    // Execute ILOAD instruction
    void executeILOAD(const std::string& argument) {
        int index = std::stoi(argument);
        
        // Log initial register state
        logRegisterState("Before ILOAD " + argument);
        
        int cycle = 1;
        // Executa as microinstruções para ILOAD
        for (const auto& microInst : instructionMap["ILOAD"]) {
            // Registra o ciclo no log
            logFile << "Cycle " << cycle++ << std::endl;
            
            // Registra a microinstrução no log
            std::string ir = microInst.binary.substr(0, 8) + " " + 
                             microInst.binary.substr(8, 9) + " " + 
                             microInst.binary.substr(17, 2) + " " + 
                             microInst.binary.substr(19, 4);
            logFile << "ir = " << ir << std::endl;
            
            // Convert binary string to char array for ALU
            char aluInput[8];
            for (int i = 0; i < 8; i++) {
                aluInput[i] = microInst.binary[i];
            }
            
            if (microInst.description.find("MAR = LV + H") != std::string::npos) {
                // Define H com o valor do índice
                regs.H = index;
                
                // Usa a ULA para calcular LV + H
                auto [output, output_sd, N, Z, carry] = ula8bits(aluInput, regs.LV, regs.H);
                regs.MAR = output;
                regs.N = N;
                regs.Z = Z;
            
                // Lê da memória
                if (regs.MAR < memory.size()) {
                    regs.MBR = memory[regs.MAR] & 0xFF; // Mantém apenas 8 bits para MBR
                } else {
                    // Valor padrão se o endereço de memória estiver fora dos limites
                    regs.MBR = 0; 
                }
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MAR = LV + H; rd");
                logBusState("LV, H", "MAR");
            }
            else if (microInst.description.find("H = MBR; PC = PC + 1") != std::string::npos) {
                // Define H = MBR (estendido com sinal para 32 bits)
                regs.H = regs.MBR;
                
                // Increment PC
                auto [output, output_sd, N, Z, carry] = ula8bits(aluInput, regs.PC, 1);
                regs.PC = output;
                regs.N = N;
                regs.Z = Z;
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After H = MBR; PC = PC + 1");
                logBusState("MBR, PC", "H, PC");
            }
            else if (microInst.description.find("MDR = TOS") != std::string::npos) {
                // Set MDR = TOS
                regs.MDR = regs.TOS;
                
                // Extract write signal from the microinstruction (bit 6)
                bool writeSignal = (microInst.binary.length() > 6 && microInst.binary[microInst.binary.length()-7] == '1');
                
                // If write signal is set, write to memory
                if (writeSignal && regs.MAR < memory.size()) {
                    memory[regs.MAR] = regs.MDR;
                }
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MDR = TOS");
                logBusState("TOS", "MDR");
            }
            else if (microInst.description.find("MAR = SP = SP + 1") != std::string::npos) {
                // Usa a ULA para incrementar SP
                auto [output, output_sd, N, Z, carry] = ula8bits(aluInput, regs.SP, 1);
                regs.SP = output;
                regs.MAR = regs.SP;
                regs.N = N;
                regs.Z = Z;
                
                // Escreve na memória: memory[MAR] = MDR
                if (regs.MAR >= memory.size()) {
                    // Expand memory if needed
                    memory.resize(regs.MAR + 1, 0);
                }
                memory[regs.MAR] = regs.MDR;
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MAR = SP = SP + 1; wr");
                logBusState("SP", "MAR, SP");
            }
            else if (microInst.description.find("TOS = MDR") != std::string::npos) {
                // Usa a ULA para definir TOS = MDR
                auto [output, output_sd, N, Z, carry] = ula8bits(aluInput, regs.MDR, 0);
                regs.TOS = output;
                regs.N = N;
                regs.Z = Z;
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After TOS = MDR");
                logBusState("MDR", "TOS");
            }
        }
        
        // Log final register state
        logFile << "Cycle " << cycle << std::endl;
        logFile << "No more lines, EOP." << std::endl;
        logFile << "=====================================================" << std::endl;
    }

    // Execute DUP instruction
    void executeDUP() {
        // Log initial register state
        logRegisterState("Before DUP");
        
        int cycle = 1;
        // Execute microinstructions for DUP
        for (const auto& microInst : instructionMap["DUP"]) {
            // Registra o ciclo no log
            logFile << "Cycle " << cycle++ << std::endl;
            
            // Registra a microinstrução no log
            std::string ir = microInst.binary.substr(0, 8) + " " + 
                             microInst.binary.substr(8, 9) + " " + 
                             microInst.binary.substr(17, 2) + " " + 
                             microInst.binary.substr(19, 4);
            logFile << "ir = " << ir << std::endl;
            
            // Extract control signals from 23-bit microinstruction
            std::string microCode = microInst.binary;
            
            if (microInst.description.find("MDR = TOS") != std::string::npos) {
                // Set MDR = TOS
                regs.MDR = regs.TOS;
                
                // Extract write signal from the microinstruction (bit 6)
                bool writeSignal = (microInst.binary.length() > 6 && microInst.binary[microInst.binary.length()-7] == '1');
                
                // If write signal is set, write to memory
                if (writeSignal && regs.MAR < memory.size()) {
                    memory[regs.MAR] = regs.MDR;
                }
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MDR = TOS");
                logBusState("TOS", "MDR");
            }
            else if (microInst.description.find("MAR = SP = SP + 1") != std::string::npos) {
                // Increment SP
                regs.SP = regs.SP + 1;
                regs.MAR = regs.SP;
                
                // Extract write signal from the microinstruction (bit 6)
                bool writeSignal = (microInst.binary.length() > 6 && microInst.binary[microInst.binary.length()-7] == '1');
                
                // If write signal is set, write to memory
                if (writeSignal) {
                    if (regs.MAR >= memory.size()) {
                        // Expand memory if needed
                        memory.resize(regs.MAR + 1, 0);
                    }
                    memory[regs.MAR] = regs.MDR;
                }
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MAR = SP = SP + 1; wr");
                logBusState("SP", "MAR, SP");
            }
            else if (microInst.description.find("PC = PC + 1") != std::string::npos) {
                // Increment PC
                regs.PC = regs.PC + 1;
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After PC = PC + 1");
                logBusState("PC", "PC");
            }
        }
        
        // Log final register state
        logFile << "Cycle " << cycle << std::endl;
        logFile << "=====================================================" << std::endl;
    }

    // Execute BIPUSH instruction
    void executeBIPUSH(const std::string& byteValue) {
        // Convert byteValue string to integer
        int byteInt = std::stoi(byteValue);
        
        // Log initial register state
        logRegisterState("Before BIPUSH " + byteValue);
        
        int cycle = 1;
        // Execute microinstructions for BIPUSH
        for (const auto& microInst : instructionMap["BIPUSH"]) {
            // Registra o ciclo no log
            logFile << "Cycle " << cycle++ << std::endl;
            
            // Registra a microinstrução no log
            std::string ir = microInst.binary.substr(0, 8) + " " + 
                             microInst.binary.substr(8, 9) + " " + 
                             microInst.binary.substr(17, 2) + " " + 
                             microInst.binary.substr(19, 4);
            logFile << "ir = " << ir << std::endl;
            
            // Extract control signals from 23-bit microinstruction
            std::string microCode = microInst.binary;
            
            if (microInst.description.find("PC = PC + 1") != std::string::npos) {
                // Increment PC
                regs.PC = regs.PC + 1;
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After PC = PC + 1");
                logBusState("PC", "PC");
            }
            else if (microInst.description.find("MBR = byte") != std::string::npos) {
                // Set MBR to the byte value from the instruction
                regs.MBR = byteInt & 0xFF; // Ensure it's only 8 bits
                
                // Set H = MBR (zero-extended to 32 bits)
                regs.H = static_cast<uint32_t>(regs.MBR & 0xFF); // Zero-extension
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MBR = byte; H = MBR");
                logBusState("byte", "MBR, H");
            }
            else if (microInst.description.find("MAR = SP = SP + 1") != std::string::npos) {
                // Increment SP
                regs.SP = regs.SP + 1;
                regs.MAR = regs.SP;
                
                // Extract write signal from the microinstruction (bit 6)
                bool writeSignal = (microInst.binary.length() > 6 && microInst.binary[microInst.binary.length()-7] == '1');
                
                // If write signal is set, write to memory
                if (writeSignal) {
                    if (regs.MAR >= memory.size()) {
                        // Expand memory if needed
                        memory.resize(regs.MAR + 1, 0);
                    }
                    memory[regs.MAR] = regs.MDR;
                }
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After MAR = SP = SP + 1; wr");
                logBusState("SP", "MAR, SP");
            }
            else if (microInst.description.find("TOS = MBR") != std::string::npos) {
                // Convert binary string to char array for ALU
                char aluInput[8];
                for (int i = 0; i < 8; i++) {
                    aluInput[i] = microInst.binary[i];
                }
                
                // Use ALU to set TOS = MBR (sign extended to 32 bits)
                auto [output, output_sd, N, Z, carry] = ula8bits(aluInput, regs.MBR, 0);
                regs.TOS = output;
                regs.N = N;
                regs.Z = Z;
                
                // Increment PC
                auto [pc_output, pc_output_sd, pc_N, pc_Z, pc_carry] = ula8bits(aluInput, regs.PC, 1);
                regs.PC = pc_output;
                
                // Registra o estado dos registradores após esta microinstrução
                logRegisterState("After TOS = MBR; PC = PC + 1");
                logBusState("MBR, PC", "TOS, PC");
            }
        }
        
        // Log final register state
        logFile << "Cycle " << cycle << std::endl;
        logFile << "No more lines, EOP." << std::endl;
        logFile << "=====================================================" << std::endl;
    }

    // Registra o estado dos registradores
    void logRegisterState(const std::string& label) {
        if (label.find("Before") != std::string::npos) {
            logFile << "> Registers before instruction" << std::endl;
        } else if (label.find("After") != std::string::npos) {
            logFile << "> Registers after instruction" << std::endl;
        } else {
            logFile << label << std::endl;
        }
        logFile << "*******************************" << std::endl;
        logFile << "mar = " << std::bitset<32>(regs.MAR) << std::endl;
        logFile << "mdr = " << std::bitset<32>(regs.MDR) << std::endl;
        logFile << "pc = " << std::bitset<32>(regs.PC) << std::endl;
        logFile << "mbr = " << std::bitset<8>(regs.MBR) << std::endl;
        logFile << "sp = " << std::bitset<32>(regs.SP) << std::endl;
        logFile << "lv = " << std::bitset<32>(regs.LV) << std::endl;
        logFile << "cpp = " << std::bitset<32>(regs.CPP) << std::endl;
        logFile << "tos = " << std::bitset<32>(regs.TOS) << std::endl;
        logFile << "opc = " << std::bitset<32>(regs.OPC) << std::endl;
        logFile << "h = " << std::bitset<32>(regs.H) << std::endl;
        logFile << std::endl;
    }

    // Registra o estado dos barramentos
    void logBusState(const std::string& busB, const std::string& busC) {
        logFile << "b = " << busB << std::endl;
        logFile << "c = " << busC << std::endl;
        logFile << std::endl;
    }

    // Registra o estado da memória
    void logMemoryState() {
        logFile << "> Memory after instruction" << std::endl;
        logFile << "*******************************" << std::endl;
        for (size_t i = 0; i < memory.size() && i < 16; ++i) {
            logFile << std::bitset<32>(memory[i]) << std::endl;
        }
        logFile << "============================================================" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::string registerFile = "registradores_etapa3_tarefa1";
    std::string memoryFile = "dados_etapa3_tarefa1.txt";
    std::string instructionFile = "instrucoes.txt";
    
    // Analisa os argumentos da linha de comando
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-r" && i + 1 < argc) {
            registerFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            memoryFile = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            instructionFile = argv[++i];
        }
    }
    
    Mic1Interpreter interpreter;
    
    // Carrega os valores iniciais dos registradores
    if (!interpreter.loadRegisters(registerFile)) {
        std::cerr << "Failed to load registers from " << registerFile << std::endl;
        return 1;
    }
    
    // Carrega os valores iniciais da memória
    if (!interpreter.loadMemory(memoryFile)) {
        std::cerr << "Failed to load memory from " << memoryFile << std::endl;
        return 1;
    }
    
    // Executa as instruções
    if (!interpreter.executeInstructions(instructionFile)) {
        std::cerr << "Failed to execute instructions from " << instructionFile << std::endl;
        return 1;
    }
    
    std::cout << "Execution completed. See execution_log.txt for details." << std::endl;
    
    return 0;
}