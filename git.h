#ifndef GIT_H
#define GIT_H


#include <string>
#include <vector>
#include <map>


using namespace std;


int git_generate(vector <vector <string>*>* graph, map <string, string>* decorations, map <string, string>* commit_info);


#endif // GIT_H
