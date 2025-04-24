#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

void read(vector<string> &palavras,string path,int len);

void write(vector<string> &palavras,string path);

string decode(int num);

int encode(string &s);