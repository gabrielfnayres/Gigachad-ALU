#include "alu.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <map>

int main(int argc, char* argv[]) {
    // Check if input file is provided
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        return 1;
    }

    // Get input and output file names
    std::string input_filename = argv[1];
    std::string output_filename = (argc > 2) ? argv[2] : "output.txt";

    // Open input file
    std::ifstream input_file(input_filename);
    if (!input_file.is_open()) {
        std::cerr << "Error: Could not open input file " << input_filename << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream output_file(output_filename);
    if (!output_file.is_open()) {
        std::cerr << "Error: Could not open output file " << output_filename << std::endl;
        return 1;
    }

    // Initialize ALU
    Alu* alu = new Alu();
    
    // Set initial values for A and B (32-bit binary strings)
    std::string a_value = "11111111111111111111111111111111"; // Initial value for A
    std::string b_value = "00000000000000000000000000000001"; // Initial value for B
    
    alu->setA(a_value);
    alu->setB(b_value);
    

    // Write initial values to output file
    output_file << "b = " << b_value << std::endl;
    output_file << "a = " << a_value << std::endl;
    output_file << std::endl;
    output_file << "Start of Program" << std::endl;

    // Read and execute instructions
    std::string instruction;
    int cycle = 1;
    

    
    while (std::getline(input_file, instruction)) {
        // Skip empty lines
        
        
        if (instruction.empty()) {
            continue;
        }
        
        // Check if instruction is valid (6 bits)
        if (instruction.length() != 6) {
            std::cerr << "Warning: Skipping invalid instruction: " << instruction << std::endl;
            continue;
        }
        
        // Update IR and PC
        alu->setIR(instruction);
        alu->setPC(cycle);
        
        // Extract control signals according to the format: F0 F1 ENA ENB INVA INC
        bool f0 = (instruction[0] == '1');
        bool f1 = (instruction[1] == '1');
        bool ena = (instruction[2] == '1');
        bool enb = (instruction[3] == '1');
        bool inva = (instruction[4] == '1');
        bool inc = (instruction[5] == '1');
        

        
        // Set control signals
        alu->setF0(f0);
        alu->setF1(f1);
        alu->setEna(ena);
        alu->setEnb(enb);
        alu->setInva(inva);
        alu->setInc(inc);
        
        // Execute operation
        alu->execute_operation();
        
        // Write to output file
        output_file << "============================================================" << std::endl;
        output_file << "Cycle " << cycle << std::endl;
        output_file << std::endl;
        output_file << "PC = " << alu->getPC() << std::endl;
        output_file << "IR = " << alu->getIR() << std::endl;
        output_file << "b = " << alu->getB() << std::endl;
        output_file << "a = " << alu->getA() << std::endl;
        output_file << "s = " << alu->getS() << std::endl;
        output_file << "co = " << alu->getCarryout() << std::endl;
        
        // Increment cycle
        cycle++;
        
        // Update register A based on the ENA signal from the current instruction
        if (!ena) {
            alu->setA("00000000000000000000000000000000");
        } else {
            alu->setA("11111111111111111111111111111111");
        }
        // Keep B value unchanged
        alu->setB(alu->getB());
    }
    
    // End of program
    output_file << "============================================================" << std::endl;
    output_file << "Cycle " << cycle << std::endl;
    output_file << std::endl;
    output_file << "PC = " << cycle << std::endl;
    output_file << "> Line is empty, EOP." << std::endl;
    
    // Close files
    input_file.close();
    output_file.close();
    
    delete alu;

    std::cout << "Processing complete. Output written to " << output_filename << std::endl;
    
    return 0;
}
