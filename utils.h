#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <vector>


using namespace std;


vector<string>* str_split(string s, char delimiter);
void str_trim(string* s, const char* rm);
string* execute(string cmd);
int parse_cmdline(int argc, char** argv);
void help();


#endif // UTILS_H
