#include <utils.h>
#include <stdio.h>
#include <sstream>


vector<string>* str_split(string s, char delimiter){
	stringstream stream(s);
	string r;
	vector<string>* v;


	v = new vector<string>;
	while(getline(stream, r, delimiter))
		v->push_back(r);
	return v;
}

string* execute(string cmd){
	char buf[255];
	int i;
	FILE* fp;
	string* s;


	s = new string;
	fp = popen(cmd.c_str(), "r");

	while((i = fread(buf, 1, 254, fp))){
		buf[i] = 0;
		s->append(buf);
	}

	pclose(fp);
	return s;
}
