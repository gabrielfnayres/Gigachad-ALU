#pragma once
#include <string>
using namespace std;

class Alu {
private:
    bool sll8, sra1, f0, f1, ena, enb, inva, inc;
    char A[33];
    char B[33];
    char S[33]; // representa o sd (resultado final)
    char IR[9];
    int PC;
    int carryout;
    int Z, N;
    unsigned int raw_s; // resultado bruto da ULA (antes do deslocamento)

public:
    Alu();
    int execute_operation();
    bool lineIsEmpty(const string& line);

    // Setters
    void setSLL8(bool val);
    void setSRA1(bool val);
    void setF0(bool val);
    void setF1(bool val);
    void setEna(bool val);
    void setEnb(bool val);
    void setInva(bool val);
    void setInc(bool val);
    void setA(const string& val);
    void setB(const string& val);
    void setS(const string& val);
    void setIR(const string& val);
    void setPC(int val);
    void setCarryout(int val);
    void setRawS(unsigned int val);

    // Getters
    string getA();
    string getB();
    string getS();
    string getIR();
    int getPC();
    int getCarryout();
    int getZ();
    int getN();
    unsigned int getRawS();
};
