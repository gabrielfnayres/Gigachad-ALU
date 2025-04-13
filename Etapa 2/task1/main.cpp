#include "alu.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>  // necessário para usar std::bitset

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input_file> [output_file]" << endl;
        return 1;
    }

    string input_filename = argv[1];
    string output_filename = (argc > 2) ? argv[2] : "saida_gerada.txt";

    ifstream input_file(input_filename);
    ofstream output_file(output_filename);

    if (!input_file.is_open() || !output_file.is_open()) {
        cerr << "Erro ao abrir arquivos." << endl;
        return 1;
    }

    Alu alu;
    string instruction;
    int cycle = 1;

    output_file << "b = " << alu.getB() << endl;
    output_file << "a = " << alu.getA() << endl;
    output_file << "\nStart of Program\n";

    while (getline(input_file, instruction)) {
        output_file << "============================================================\n";
        output_file << "Cycle " << cycle << "\n\n";
        alu.setPC(cycle);


        alu.setIR(instruction);
        if (instruction.size() != 8 || instruction.find_first_not_of("01") != string::npos) {
            output_file << "PC = " << cycle << "\nIR = " << instruction << "\n> Error, invalid control signals.\n";
            cycle++;
            continue;
        }

        
        bool sll8 = instruction[0] == '1';
        bool sra1 = instruction[1] == '1';

        // Verifica conflito entre deslocamentos
        if (sll8 && sra1) {
            output_file << "PC = " << alu.getPC() << "\n";
            output_file << "IR = " << instruction << "\n";
            output_file << "> Error, invalid control signals.\n";
            cycle++;
            continue;
        }

        alu.setSLL8(sll8);
        alu.setSRA1(sra1);
        alu.setF0(instruction[2] == '1');
        alu.setF1(instruction[3] == '1');
        alu.setEna(instruction[4] == '1');
        alu.setEnb(instruction[5] == '1');
        alu.setInva(instruction[6] == '1');
        alu.setInc(instruction[7] == '1');

        alu.execute_operation();

        string binS = bitset<32>(alu.getRawS()).to_string(); // pega o valor bruto da ULA

        output_file << "PC = " << alu.getPC() << "\n";
        output_file << "IR = " << alu.getIR() << "\n";
        output_file << "b = " << alu.getB() << "\n";
        output_file << "a = " << alu.getA() << "\n";
        output_file << "s = " << binS << "\n";               // s sem deslocamento
        output_file << "sd = " << alu.getS() << "\n";        // sd com deslocamento
        output_file << "n = " << alu.getN() << "\n";
        output_file << "z = " << alu.getZ() << "\n";
        output_file << "co = " << alu.getCarryout() << "\n";

        cycle++;
    }

    output_file << "============================================================\n";
    output_file << "Cycle " << cycle << "\n\n";
    output_file << "PC = " << cycle << "\n";
    output_file << "> Line is empty, EOP.\n";

    input_file.close();
    output_file.close();

    cout << "Execução concluída. Resultado salvo em " << output_filename << endl;
    return 0;
}
