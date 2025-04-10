#include "alu.h"


Alu::Alu(){
    f0 = false;
    f1 = false;
    ena = false;
    enb = false;
    inva = false;
    inc = false;
    
    strcpy(A, "11111111111111111111111111111111");
    strcpy(B, "00000000000000000000000000000001");
    strcpy(S, "00000000000000000000000000000000");
    strcpy(IR, "000000");
    PC = 1;
    carryout = 0;
}

Alu::Alu(bool f0, bool f1, bool ena, bool enb, bool inva, bool inc, const char* A, const char* B, const char* IR, int PC, int carryout){
    this->f0 = f0;
    this->f1 = f1;
    this->ena = ena;
    this->enb = enb;
    this->inva = inva;
    this->inc = inc;
    
    strncpy(this->A, A, 32);
    this->A[32] = '\0';
    
    strncpy(this->B, B, 32);
    this->B[32] = '\0';
    
    strncpy(this->IR, IR, 6);
    this->IR[6] = '\0';
    
    this->PC = PC;
    this->carryout = carryout;
}

void Alu::setF0(bool f0){
    this->f0 = f0;
}

void Alu::setF1(bool f1){
    this->f1 = f1;
}

void Alu::setEna(bool ena){
    this->ena = ena;
}

void Alu::setEnb(bool enb){
    this->enb = enb;
}

void Alu::setInva(bool inva){
    this->inva = inva;
}
    
void Alu::setInc(bool inc){
    this->inc = inc;
}

void Alu::setA(const std::string& A){
    strncpy(this->A, A.c_str(), 32);
    this->A[32] = '\0';
}

void Alu::setB(const std::string& B){
    strncpy(this->B, B.c_str(), 32);
    this->B[32] = '\0';
}

void Alu::setIR(const std::string& IR){
    strncpy(this->IR, IR.c_str(), 6);
    this->IR[6] = '\0';
}

void Alu::setS(const std::string& S){
    strncpy(this->S, S.c_str(), 32);
    this->S[32] = '\0';
}

void Alu::setPC(int PC){
    this->PC = PC;
}

void Alu::setCarryout(int carryout){
    this->carryout = carryout;
}




bool Alu::getF0(){
    return this->f0;
}

bool Alu::getF1(){
    return this->f1;
}
    
bool Alu::getEna(){
    return this->ena;
}

bool Alu::getEnb(){
    return this->enb;
}
    
bool Alu::getInva(){
    return this->inva;
}

bool Alu::getInc(){
    return this->inc;
}

std::string Alu::getA(){
    return std::string(this->A);
}

std::string Alu::getB(){
    return std::string(this->B);
}

std::string Alu::getS(){
    return std::string(this->S);
}

std::string Alu::getIR(){
    return std::string(this->IR);
}

int Alu::getPC(){
    return this->PC;
}

int Alu::getCarryout(){
    return this->carryout;
}

bool Alu::lineIsEmpty(const std::string& line) {
    for (char c : line) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

int Alu::execute_operation() {
    // Initialize carryout to 0
    carryout = 0;
    
    // Convert binary strings to integers using bitset
    std::bitset<32> bsA(A);
    std::bitset<32> bsB(B);
    
    // Apply enable signals - if not enabled, the value is 0 for the operation
    unsigned int opA = ena ? bsA.to_ulong() : 0;
    unsigned int opB = enb ? bsB.to_ulong() : 0;
    
    // Apply INVA if needed - invert the A input
    if (inva) {
        opA = (~opA) & 0xFFFFFFFF;
    }
    
    // Determine operation based on F0 and F1
    int op = (f0 ? 1 : 0) * 2 + (f1 ? 1 : 0);
    
    unsigned long long result = 0;
    
    // Perform operation based on F0 and F1
    switch(op) {
        case 0:  // 00: AND
            result = opA & opB;
            break;
        case 1:  // 01: OR
            result = opA | opB;
            break;
        case 2:  // 10: ADD
            result = (unsigned long long)opA + opB;
            break;
        case 3:  // 11: ADD with INC
            result = (unsigned long long)opA + opB;
            break;
    }
    
    // Apply INC if needed (carry-in)
    if (inc) {
        result += 1;
    }
    
    // Detect carryout if the result exceeds 32 bits
    if (result > 0xFFFFFFFF) {
        carryout = 1;
    }
    
    // Keep only the lower 32 bits
    unsigned int res32 = (unsigned int)(result & 0xFFFFFFFF);
    
    // Convert the result to string binary and store in S
    std::string binRes = std::bitset<32>(res32).to_string();
    strncpy(S, binRes.c_str(), 32);
    S[32] = '\0';
    
    return res32;
}