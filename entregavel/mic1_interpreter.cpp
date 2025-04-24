#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <iomanip>
#include "../task1/alu.h"

// Structure to represent a microinstruction
struct MicroInstruction {
    std::string binary; // 23-bit microinstruction
    std::string description; // Description of what the microinstruction does
};

// Structure to represent a register set
struct Registers {
    std::string H;     // 32 bits
    std::string OPC;   // 32 bits
    std::string TOS;   // 32 bits
    std::string CPP;   // 32 bits
    std::string LV;    // 32 bits
    std::string SP;    // 32 bits
    std::string MBR;   // 8 bits
    std::string PC;    // 32 bits
    std::string MDR;   // 32 bits
    std::string MAR;   // 32 bits
};

// Class to handle the Mic-1 interpreter
class Mic1Interpreter {
private:
    Registers regs;
    std::vector<std::string> memory;
    Alu alu;
    std::ofstream logFile;

    // Maps to store microinstruction sequences for each instruction
    std::map<std::string, std::vector<MicroInstruction>> instructionMap;

public:
    Mic1Interpreter() {
        // Initialize the instruction map with microinstruction sequences
        initializeInstructionMap();
    }

    // Initialize the instruction map with microinstruction sequences
    void initializeInstructionMap() {
        // ILOAD x microinstructions
        instructionMap["ILOAD"] = {
            {"00000000000000000000000", "MAR = LV + H; rd"},
            {"00000000000000000000000", "H = MBR; PC = PC + 1"},
            {"00000000000000000000000", "MDR = TOS"},
            {"00000000000000000000000", "MAR = SP = SP + 1; wr"},
            {"00000000000000000000000", "TOS = MDR"}
        };

        // DUP microinstructions
        instructionMap["DUP"] = {
            {"00000000000000000000000", "MDR = TOS"},
            {"00000000000000000000000", "MAR = SP = SP + 1; wr"},
            {"00000000000000000000000", "PC = PC + 1"}
        };

        // BIPUSH byte microinstructions
        instructionMap["BIPUSH"] = {
            {"00000000000000000000000", "PC = PC + 1"},
            {"00000000000000000000000", "MBR = byte; MDR = TOS"}, // First 8 bits will be replaced with the byte value
            {"00000000000000000000000", "MAR = SP = SP + 1; wr"},
            {"00000000000000000000000", "TOS = MBR; PC = PC + 1"}
        };
    }

    // Load initial register values from file
    bool loadRegisters(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open register file: " << filename << std::endl;
            return false;
        }

        std::string line, reg, value;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                reg = line.substr(0, pos);
                // Remove whitespace
                reg.erase(0, reg.find_first_not_of(" \t"));
                reg.erase(reg.find_last_not_of(" \t") + 1);
                
                value = line.substr(pos + 1);
                // Remove whitespace
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                if (reg == "mar") regs.MAR = value;
                else if (reg == "mdr") regs.MDR = value;
                else if (reg == "pc") regs.PC = value;
                else if (reg == "mbr") regs.MBR = value;
                else if (reg == "sp") regs.SP = value;
                else if (reg == "lv") regs.LV = value;
                else if (reg == "cpp") regs.CPP = value;
                else if (reg == "tos") regs.TOS = value;
                else if (reg == "opc") regs.OPC = value;
                else if (reg == "h") regs.H = value;
            }
        }

        file.close();
        return true;
    }

    // Load initial memory values from file
    bool loadMemory(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open memory file: " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (!line.empty()) {
                memory.push_back(line);
            }
        }

        file.close();
        return true;
    }

    // Parse and execute instructions from file
    bool executeInstructions(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open instruction file: " << filename << std::endl;
            return false;
        }

        // Open log file
        logFile.open("execution_log.txt");
        if (!logFile.is_open()) {
            std::cerr << "Error: Could not create log file" << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Remove whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty()) continue;

            logFile << "Executing instruction: " << line << std::endl;
            logFile << "----------------------------------------" << std::endl;

            // Parse instruction
            std::string instruction, argument;
            size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                instruction = line.substr(0, pos);
                argument = line.substr(pos + 1);
                // Remove whitespace
                argument.erase(0, argument.find_first_not_of(" \t"));
                argument.erase(argument.find_last_not_of(" \t") + 1);
            } else {
                instruction = line;
                argument = "";
            }

            // Execute the instruction
            if (instruction == "ILOAD") {
                executeILOAD(argument);
            } else if (instruction == "DUP") {
                executeDUP();
            } else if (instruction == "BIPUSH") {
                executeBIPUSH(argument);
            } else {
                logFile << "Unknown instruction: " << instruction << std::endl;
            }

            // Log memory state after instruction execution
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
        
        // Execute microinstructions for ILOAD
        for (const auto& microInst : instructionMap["ILOAD"]) {
            // Log microinstruction
            logFile << "Executing microinstruction: " << microInst.description << std::endl;
            
            // Simulate microinstruction execution
            // This is a simplified simulation - in a real implementation, you would
            // decode the binary microinstruction and execute the appropriate operations
            
            // For ILOAD, we need to:
            // 1. Set MAR = LV + index
            // 2. Read from memory (MBR = memory[MAR])
            // 3. Set TOS = MBR
            // 4. Increment PC
            
            if (microInst.description.find("MAR = LV + H") != std::string::npos) {
                // Set H to the index value (converted to binary)
                regs.H = std::bitset<32>(index).to_string();
                
                // Set MAR = LV + H
                int lvValue = std::bitset<32>(regs.LV).to_ulong();
                int hValue = std::bitset<32>(regs.H).to_ulong();
                regs.MAR = std::bitset<32>(lvValue + hValue).to_string();
                
                // Log register state after this microinstruction
                logRegisterState("After MAR = LV + H");
                logBusState("LV", "MAR");
            }
            else if (microInst.description.find("H = MBR; PC = PC + 1") != std::string::npos) {
                // Read from memory: MBR = memory[MAR]
                int marValue = std::bitset<32>(regs.MAR).to_ulong();
                if (marValue < memory.size()) {
                    regs.MBR = memory[marValue];
                } else {
                    regs.MBR = "00000000"; // Default value if memory address is out of bounds
                }
                
                // Set H = MBR
                regs.H = std::string(24, '0') + regs.MBR; // Extend 8-bit MBR to 32-bit H
                
                // Increment PC
                int pcValue = std::bitset<32>(regs.PC).to_ulong();
                regs.PC = std::bitset<32>(pcValue + 1).to_string();
                
                // Log register state after this microinstruction
                logRegisterState("After H = MBR; PC = PC + 1");
                logBusState("MBR", "H, PC");
            }
            else if (microInst.description.find("MDR = TOS") != std::string::npos) {
                // Set MDR = TOS
                regs.MDR = regs.TOS;
                
                // Log register state after this microinstruction
                logRegisterState("After MDR = TOS");
                logBusState("TOS", "MDR");
            }
            else if (microInst.description.find("MAR = SP = SP + 1") != std::string::npos) {
                // Increment SP
                int spValue = std::bitset<32>(regs.SP).to_ulong();
                regs.SP = std::bitset<32>(spValue + 1).to_string();
                
                // Set MAR = SP
                regs.MAR = regs.SP;
                
                // Write to memory: memory[MAR] = MDR
                int marValue = std::bitset<32>(regs.MAR).to_ulong();
                if (marValue >= memory.size()) {
                    // Expand memory if needed
                    memory.resize(marValue + 1, "00000000000000000000000000000000");
                }
                memory[marValue] = regs.MDR;
                
                // Log register state after this microinstruction
                logRegisterState("After MAR = SP = SP + 1; wr");
                logBusState("SP", "MAR, SP");
            }
            else if (microInst.description.find("TOS = MDR") != std::string::npos) {
                // Set TOS = MDR
                regs.TOS = regs.MDR;
                
                // Log register state after this microinstruction
                logRegisterState("After TOS = MDR");
                logBusState("MDR", "TOS");
            }
        }
        
        // Log final register state
        logRegisterState("After ILOAD " + argument);
    }

    // Execute DUP instruction
    void executeDUP() {
        // Log initial register state
        logRegisterState("Before DUP");
        
        // Execute microinstructions for DUP
        for (const auto& microInst : instructionMap["DUP"]) {
            // Log microinstruction
            logFile << "Executing microinstruction: " << microInst.description << std::endl;
            
            // Simulate microinstruction execution
            if (microInst.description.find("MDR = TOS") != std::string::npos) {
                // Set MDR = TOS
                regs.MDR = regs.TOS;
                
                // Log register state after this microinstruction
                logRegisterState("After MDR = TOS");
                logBusState("TOS", "MDR");
            }
            else if (microInst.description.find("MAR = SP = SP + 1") != std::string::npos) {
                // Increment SP
                int spValue = std::bitset<32>(regs.SP).to_ulong();
                regs.SP = std::bitset<32>(spValue + 1).to_string();
                
                // Set MAR = SP
                regs.MAR = regs.SP;
                
                // Write to memory: memory[MAR] = MDR
                int marValue = std::bitset<32>(regs.MAR).to_ulong();
                if (marValue >= memory.size()) {
                    // Expand memory if needed
                    memory.resize(marValue + 1, "00000000000000000000000000000000");
                }
                memory[marValue] = regs.MDR;
                
                // Log register state after this microinstruction
                logRegisterState("After MAR = SP = SP + 1; wr");
                logBusState("SP", "MAR, SP");
            }
            else if (microInst.description.find("PC = PC + 1") != std::string::npos) {
                // Increment PC
                int pcValue = std::bitset<32>(regs.PC).to_ulong();
                regs.PC = std::bitset<32>(pcValue + 1).to_string();
                
                // Log register state after this microinstruction
                logRegisterState("After PC = PC + 1");
                logBusState("PC", "PC");
            }
        }
        
        // Log final register state
        logRegisterState("After DUP");
    }

    // Execute BIPUSH instruction
    void executeBIPUSH(const std::string& byteValue) {
        // Log initial register state
        logRegisterState("Before BIPUSH " + byteValue);
        
        // Execute microinstructions for BIPUSH
        for (const auto& microInst : instructionMap["BIPUSH"]) {
            // Log microinstruction
            logFile << "Executing microinstruction: " << microInst.description << std::endl;
            
            // For BIPUSH, the second microinstruction needs special handling
            // The first 8 bits should be set to the byte value from the instruction
            std::string modifiedMicroInst = microInst.binary;
            
            if (microInst.description.find("MBR = byte") != std::string::npos) {
                // Set MBR to the byte value from the instruction
                regs.MBR = byteValue;
                
                // Log register state after this microinstruction
                logRegisterState("After MBR = byte; MDR = TOS");
                logBusState("", "MBR, MDR");
                
                // Also set MDR = TOS
                regs.MDR = regs.TOS;
            }
            else if (microInst.description.find("PC = PC + 1") != std::string::npos) {
                // Increment PC
                int pcValue = std::bitset<32>(regs.PC).to_ulong();
                regs.PC = std::bitset<32>(pcValue + 1).to_string();
                
                // Log register state after this microinstruction
                logRegisterState("After PC = PC + 1");
                logBusState("PC", "PC");
            }
            else if (microInst.description.find("MAR = SP = SP + 1") != std::string::npos) {
                // Increment SP
                int spValue = std::bitset<32>(regs.SP).to_ulong();
                regs.SP = std::bitset<32>(spValue + 1).to_string();
                
                // Set MAR = SP
                regs.MAR = regs.SP;
                
                // Write to memory: memory[MAR] = MDR
                int marValue = std::bitset<32>(regs.MAR).to_ulong();
                if (marValue >= memory.size()) {
                    // Expand memory if needed
                    memory.resize(marValue + 1, "00000000000000000000000000000000");
                }
                memory[marValue] = regs.MDR;
                
                // Log register state after this microinstruction
                logRegisterState("After MAR = SP = SP + 1; wr");
                logBusState("SP", "MAR, SP");
            }
            else if (microInst.description.find("TOS = MBR") != std::string::npos) {
                // Set TOS = MBR (extended to 32 bits)
                regs.TOS = std::string(24, '0') + regs.MBR;
                
                // Increment PC
                int pcValue = std::bitset<32>(regs.PC).to_ulong();
                regs.PC = std::bitset<32>(pcValue + 1).to_string();
                
                // Log register state after this microinstruction
                logRegisterState("After TOS = MBR; PC = PC + 1");
                logBusState("MBR", "TOS, PC");
            }
        }
        
        // Log final register state
        logRegisterState("After BIPUSH " + byteValue);
    }

    // Log register state
    void logRegisterState(const std::string& label) {
        logFile << label << ":" << std::endl;
        logFile << "  H   = " << regs.H << std::endl;
        logFile << "  OPC = " << regs.OPC << std::endl;
        logFile << "  TOS = " << regs.TOS << std::endl;
        logFile << "  CPP = " << regs.CPP << std::endl;
        logFile << "  LV  = " << regs.LV << std::endl;
        logFile << "  SP  = " << regs.SP << std::endl;
        logFile << "  MBR = " << regs.MBR << std::endl;
        logFile << "  PC  = " << regs.PC << std::endl;
        logFile << "  MDR = " << regs.MDR << std::endl;
        logFile << "  MAR = " << regs.MAR << std::endl;
        logFile << std::endl;
    }

    // Log bus state
    void logBusState(const std::string& busB, const std::string& busC) {
        logFile << "  Bus B = " << busB << std::endl;
        logFile << "  Bus C = " << busC << std::endl;
        logFile << std::endl;
    }

    // Log memory state
    void logMemoryState() {
        logFile << "Memory state:" << std::endl;
        for (size_t i = 0; i < memory.size(); ++i) {
            logFile << "  Memory[" << i << "] = " << memory[i] << std::endl;
        }
        logFile << std::endl;
    }
};

int main(int argc, char* argv[]) {
    std::string registerFile = "/Users/fnayres/Gigachad-ALU/entregavel/registradores_etapa3_tarefa1";
    std::string memoryFile = "/Users/fnayres/Gigachad-ALU/entregavel/dados_etapa3_tarefa1.txt";
    std::string instructionFile = "/Users/fnayres/Gigachad-ALU/entregavel/instrucoes.txt";
    
    // Parse command line arguments
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
    
    // Load initial register values
    if (!interpreter.loadRegisters(registerFile)) {
        std::cerr << "Failed to load registers from " << registerFile << std::endl;
        return 1;
    }
    
    // Load initial memory values
    if (!interpreter.loadMemory(memoryFile)) {
        std::cerr << "Failed to load memory from " << memoryFile << std::endl;
        return 1;
    }
    
    // Execute instructions
    if (!interpreter.executeInstructions(instructionFile)) {
        std::cerr << "Failed to execute instructions from " << instructionFile << std::endl;
        return 1;
    }
    
    std::cout << "Execution completed. See execution_log.txt for details." << std::endl;
    
    return 0;
}
