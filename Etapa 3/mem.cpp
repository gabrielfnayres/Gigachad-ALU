#include "mem.h"
using namespace std;

void read(vector<string> &palavras,string path,int len)
{
    ifstream arquivo(path);

    if (!arquivo.is_open())
    {
        cout << "Erro na leitura\n";
        exit(1);
    }

    string temp;
    while (getline(arquivo,temp))
    {
        if (temp.length() != len)
        {
            cout << "tamanho invalido\n";
            exit(1);
        }

        palavras.push_back(temp);
    }

    arquivo.close();
}

void write(vector<string> &palavras,string path)
{
    ofstream arquivo(path,std::ios::trunc);

    for (int i=0;i<palavras.size()-1;i++)
    {
        arquivo << palavras[i] << "\n";
    }
    arquivo << palavras[palavras.size()-1];

    arquivo.close();
}

string decode(int num)
{
    string ret = "";
    
    for (int i=0;i<32;i++)
    {
        if (num&1) ret += "1";
        else ret += "0";

        num >>= 1;
    }

    reverse(ret.begin(),ret.end());
    return ret;
}

int encode(string &s)
{
    int ret = 0;
    int slot = 1;

    for (int i=s.size()-1;i>=0;i--)
    {
        if (s[i] == '1') ret += slot;
        slot <<= 1;
    }

    return ret;
}