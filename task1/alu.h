#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <algorithm>
#include <cstring>

class Alu{

    private:
        bool f0;
        bool f1;
        bool ena;
        bool enb;
        bool inva;
        bool inc;
        char A[33]; // 32 bits + null terminator
        char B[33];
        char S[33];
        char IR[7]; // 6 bits + null terminator
        int PC, carryout;

    public:

        Alu();
        Alu(bool f0, bool f1, bool ena, bool enb, bool inva, bool inc, const char* A, const char* B, const char* IR, int PC, int carryout);
        
        void setF0(bool f0);
        void setF1(bool f1);
        void setEna(bool ena);
        void setEnb(bool enb);
        void setInva(bool inva);
        void setInc(bool inc);
        void setA(const std::string& A);
        void setB(const std::string& B);
        void setS(const std::string& S);
        void setIR(const std::string& IR);
        void setPC(int PC);
        void setCarryout(int carryout);

        bool getF0();
        bool getF1();
        bool getEna();
        bool getEnb();
        bool getInva();
        bool getInc();
        std::string getA();
        std::string getB();
        std::string getS();
        std::string getIR();
        
        int getPC();
        int getCarryout();
        
        int execute_operation();
        bool lineIsEmpty(const std::string& line);

 

};