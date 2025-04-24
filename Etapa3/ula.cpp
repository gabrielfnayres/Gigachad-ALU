#include <bits/stdc++.h>
using namespace std;

int op(int *&b,int &h,int &mdr,int &pc,string &comando)
{
    int b_op;
    b_op = *b;
    string c3(comando.begin()+2,comando.begin()+8);

    int ret;
    if (c3 == "011000") ret = h;
    else if (c3 == "010100") ret = b_op; 
    else if (c3 == "011010") ret = ~h; 
    else if (c3 == "101100") ret = ~b_op; 
    else if (c3 == "111100") ret = h+b_op;
    else if (c3 == "111101") ret = h+b_op+1; 
    else if (c3 == "111001") ret = h+1; 
    else if (c3 == "110101") {ret = b_op+1;}
    else if (c3 == "111111") ret = b_op-h;
    else if (c3 == "110110") ret = b_op-1;
    else if (c3 == "111011") ret = -h;
    else if (c3 == "001100") ret = b_op&h;
    else if (c3 == "011100") ret = b_op|h;
    else if (c3 == "010000") ret = 0;
    else if (c3 == "110001") ret = 1;
    else if (c3 == "110010") ret = -1;
    else if (c3 == "110100") ret = b_op;
    else cout << "op não implementada\n";

    return ret;
}