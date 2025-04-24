#include "mem.h"
#include "ula.h"
using namespace std;
int mar,mdr,pc,mbr,sp,lv,cpp,tos,opc,h;
ofstream logout("logout.txt");

void define_b(int *&c,string &inst)
{
    if (inst == "0000") {c = &mdr; logout << "mdr controla B";}
    else if (inst == "0001") {c = &pc; logout << "pc controla B";}
    else if (inst == "0010") {c = &mbr;logout << "mbr controla B";}
    else if (inst == "0011") {c = &sp;logout << "sp controla B";}
    else if (inst == "0100") {c = &lv;logout << "lv controla B";}
    else if (inst == "0101") {c = &cpp;logout << "cpp controla B";}
    else if (inst == "0110") {c = &tos;logout << "tos controla B";}
    else if (inst == "0111") {c = &opc;logout << "opc controla B";}
    else
    {
        logout << "Nenhuma instrução válida\n";
        exit(1);
    }

    logout << "\n";

}

void save(int &res, string &temp) // 9 bits
{
    if (temp[0] == '1') {h = res; logout << "h habilitado para escrita\n";}
    if (temp[1] == '1') {opc = res; logout << "opc habilitado para escrita\n";}
    if (temp[2] == '1') {tos = res;logout << "tos habilitado para escrita\n";}
    if (temp[3] == '1') {cpp = res;logout << "cpp habilitado para escrita\n";}
    if (temp[4] == '1') {lv = res;logout << "lv habilitado para escrita\n";}
    if (temp[5] == '1') {sp = res;logout << "sp habilitado para escrita\n";}
    if (temp[6] == '1') {pc = res;logout << "pc habilitado para escrita\n";}
    if (temp[7] == '1') {mdr = res;logout << "mdr habilitado para escrita\n";}
    if (temp[8] == '1') {mar = res;logout << "mar habilitado para escrita\n";}
}

int main()
{
    mar = 4;
    sp = 4;
    vector<string> dados;
    read(dados,"dados.txt",32);
    vector<string> inst;
    read(inst,"insts.txt",23);


    logout << "==============================================";
    logout << "\nVALORES INICIAIS\n\n";

    logout << "mar: " << decode(mar) << "\n";
    logout << "mdr: " << decode(mdr) << "\n";
    logout << "pc:" << decode(pc) << "\n";
    logout << "mbr: " << decode(mbr) << "\n";
    logout << "sp: " << decode(sp) << "\n";
    logout << "lv: " << decode(lv) << "\n";
    logout << "cpp: " << decode(cpp) << "\n";
    logout << "tos: " << decode(tos) << "\n";
    logout << "opc: " << decode(opc) << "\n";
    logout << "h: " << decode(h) << "\n\n";

    logout << "MEMÓRIA:\n";
    for (int i=0;i<dados.size();i++)
    {
        logout << dados[i] << "\n";
    }


    logout << "==============================================\n";


    for (int i=0;i<inst.size();i++)
    {
        // define quem controla o barramento b
        int *b_controller = NULL;
        string temp(inst[i].end()-4,inst[i].end());
        define_b(b_controller,temp);
        
        // envia a instrução para ula e captura o resultado
        temp.clear();
        temp = string(inst[i].begin(),inst[i].begin()+8);
        int res = op(b_controller,h,mdr,pc,temp);

        // analisa e salva nos registradores corretos o valor do barramento C
        temp.clear();
        temp = string(inst[i].begin()+8,inst[i].begin()+17);
        save(res, temp);

        if (inst[i][17] == '1') // escrita na memória
        {
            if (mar >= dados.size()) {logout << "out of bounds memory write\n";exit(1);}
            dados[mar] = decode(mdr);
            logout << "copiando dados de mdr na linha " << mar << "\n";
        } 

        if (inst[i][18] == '1') // leitura na memória
        {
            if (mar >= dados.size()){logout << "out of bounds memory read\n";exit(1);}

            logout << "copiando dados da linha " << mar << " para mdr\n";
            mdr = encode(dados[mar]);
        }

        logout << "\nFIM DA ITERAÇÃO " << i+1 << ":\n";

        logout << "mar: " << decode(mar) << "\n";
        logout << "mdr: " << decode(mdr) << "\n";
        logout << "pc:" << decode(pc) << "\n";
        logout << "mbr: " << decode(mbr) << "\n";
        logout << "sp: " << decode(sp) << "\n";
        logout << "lv: " << decode(lv) << "\n";
        logout << "cpp: " << decode(cpp) << "\n";
        logout << "tos: " << decode(tos) << "\n";
        logout << "opc: " << decode(opc) << "\n";
        logout << "h: " << decode(h) << "\n\n";

        logout << "MEMÓRIA:\n";
        for (int i=0;i<dados.size();i++)
        {
            logout << dados[i] << "\n";
        }

        logout << "==============================================\n";

    }

    logout.close();
}
