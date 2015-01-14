#include <config.h>
#include <options.h>
#include <utils.h>
#include <stdio.h>
#include <string.h>
#include <sstream>


config_t config;


vector<string>* str_split(string s, char delimiter){
	stringstream stream(s);
	string r;
	vector<string>* v;


	v = new vector<string>;
	while(getline(stream, r, delimiter))
		v->push_back(r);
	return v;
}

void str_trim(string* s, const char* rm){
	int i, j;


	for(i=s->size()-1; i>=0; i--){
		for(j=0; j<strlen(rm); j++){
			if((*s)[i] == rm[j]){
				s->erase(i, 1);
				break;
			}
		}
	}
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

int parse_cmdline(int argc, char** argv){
	unsigned int i;


	config.git_cmd = (char*)GIT_PATH;
	config.dot_cmd = (char*)DOT_PATH;
	config.repo_name = (char*)REPO_NAME;
	config.out_file = (char*)PDF_FILE;
	config.log_file = (char*)LOG_FILE;
	config.dot_file = (char*)DOT_FILE;
	config.log = false;
	config.keep_dot = false;

	for(i=1; i<argc; i++){
		switch(options_map::lookup(argv[i], strlen(argv[i]))->value){
		case OPT_OFILE:
			if(i + 1 >= argc){
				help();
				return 1;
			}

			config.out_file = argv[++i];
			break;
				
		case OPT_LOG:
			config.log = true;
			break;

		case OPT_KEEPDOT:
			config.keep_dot = true;
			break;

		case OPT_HELP:
		default:
			help();
			return 1;
		};
	}

	return 0;
}

void help(){
	printf("usage: gitversiontree <options>\n");
	printf("    %15.15s   %s\n", "-o <ofile>", "define output file");
	printf("    %15.15s   %s\n", "-l", "enable logging");
	printf("    %15.15s   %s\n", "-d", "keep .dot file");
	printf("    %15.15s   %s\n", "-h", "print help message");
}
