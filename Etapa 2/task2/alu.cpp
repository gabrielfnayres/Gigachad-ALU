#include "alu.h"
#include <bitset>
#include <cstring>
#include <cstdint>
using namespace std;

Alu::Alu() {
    sll8 = sra1 = f0 = f1 = ena = enb = inva = inc = false;
    strcpy(A, "00000000000000000000000000000001");
    strcpy(B, "10000000000000000000000000000000");
    strcpy(S, "00000000000000000000000000000000");
    strcpy(IR, "00000000");
    PC = 1;
    carryout = Z = N = 0;
    raw_s = 0;
}

void Alu::setSLL8(bool val) { sll8 = val; }
void Alu::setSRA1(bool val) { sra1 = val; }
void Alu::setF0(bool val) { f0 = val; }
void Alu::setF1(bool val) { f1 = val; }
void Alu::setEna(bool val) { ena = val; }
void Alu::setEnb(bool val) { enb = val; }
void Alu::setInva(bool val) { inva = val; }
void Alu::setInc(bool val) { inc = val; }
void Alu::setA(const string& val) { strncpy(A, val.c_str(), 32); A[32] = '\0'; }
void Alu::setB(const string& val) { strncpy(B, val.c_str(), 32); B[32] = '\0'; }
void Alu::setS(const string& val) { strncpy(S, val.c_str(), 32); S[32] = '\0'; }
void Alu::setIR(const string& val) { strncpy(IR, val.c_str(), 8); IR[8] = '\0'; }
void Alu::setPC(int val) { PC = val; }
void Alu::setCarryout(int val) { carryout = val; }
void Alu::setRawS(unsigned int val) { raw_s = val; }

string Alu::getA() { return A; }
string Alu::getB() { return B; }
string Alu::getS() { return S; }
string Alu::getIR() { return IR; }
int Alu::getPC() { return PC; }
int Alu::getCarryout() { return carryout; }
int Alu::getZ() { return Z; }
int Alu::getN() { return N; }
unsigned int Alu::getRawS() { return raw_s; }

bool Alu::lineIsEmpty(const string& line) {
    for (char c : line) if (!isspace(static_cast<unsigned char>(c))) return false;
    return true;
}

int Alu::execute_operation() {
    carryout = 0;
    Z = 0;
    N = 0;

    bitset<32> bsA(A), bsB(B);
    uint32_t opA = ena ? bsA.to_ulong() : 0;
    uint32_t opB = enb ? bsB.to_ulong() : 0;

    if (inva) opA = ~opA;

    uint64_t s = 0;
    int op = (f0 ? 1 : 0) * 2 + (f1 ? 1 : 0);

    switch(op) {
        case 0: // f0 = 0 ; f1 = 0 (0)
            s = opA & opB;
            break; 
        case 1: // f0 = 0 ; f1 = 1 (1)
            s = opA | opB;
            break;         
        case 2: // f0 = 1 ; f1 = 0 (2)
            s = opA; 
            break;              
        case 3: // f0 = 1 ; f1 = 1 (3)
            s = opA + opB;
            break;        
    }

    if (inc) s += 1;
    if (s > 0xFFFFFFFF) carryout = 1;
    s &= 0xFFFFFFFF;

    raw_s = static_cast<uint32_t>(s); // salva resultado antes do shift

    uint32_t sd = raw_s;
    if (sll8) sd = (sd << 8) & 0xFFFFFFFF;
    else if (sra1) sd = static_cast<int32_t>(sd) >> 1;

    Z = (sd == 0);
    N = ((sd >> 31) & 1);

    string binSd = bitset<32>(sd).to_string();
    strncpy(S, binSd.c_str(), 32); S[32] = '\0';

    return sd;
}
