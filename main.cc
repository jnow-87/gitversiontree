#include <utils.h>
#include <stdio.h>
#include <fstream>
#include <map>

#define GIT_PATH	"/usr/bin/git"
#define DOT_PATH	"/usr/bin/dot"
#define REPO_NAME	"test-repo"
#define LOG_FILE	"log.log"
#define DOT_FILE	"dot.dot"
#define PDF_FILE	"graph.pdf"


int main(int argc, char** argv){
	unsigned int i, j, k;
	string* res;
	string cmd;
	ofstream dot, log;
	vector <string> *lines, *columns, *tmp_v0, *tmp_v1, *tmp_v2;
	vector <vector <string>* > nodes, nodes_nonmaster;
	map <string, string> decorated_map;


	log.open(LOG_FILE, ofstream::out);

	/* get commits */
	cmd = GIT_PATH " log --all --pretty=format:\"%h|%p|%d\"";
	res = execute(cmd);
	log << "[commits]\n" << res->c_str() << "\n\n";

	lines = str_split(*res, '\n');
	delete res;

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');
		if(columns->size() > 2)
			decorated_map[(*columns)[0]] = (*columns)[2];

		delete columns;
	}

	delete lines;

	printf("processed %d decorates\n", decorated_map.size());

	log << "[decorated map]\n";
	i = 1;
	for(map<string, string>::iterator it=decorated_map.begin(); it!=decorated_map.end(); i++, it++)
		log << i << ": " << it->first.c_str() << "  " << it->second.c_str() << "\n";
	log << "\n";

	/* get branches */
	cmd = GIT_PATH " for-each-ref --format=\"%(objectname:short)|%(refname:short)\"";
	res = execute(cmd);
	log << "[branches]\n" << res->c_str() << "\n\n";

	lines = str_split(*res, '\n');
	delete res;

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');

		if((*columns)[1].find("master") != string::npos){
			cmd = GIT_PATH " log --reverse --first-parent --pretty=format:\"%h\" ";
			cmd += (*columns)[0].c_str();

			log << "execute: " << cmd << "\n";
			res = execute(cmd);
			log << res->c_str() << "\n";

			tmp_v0 = str_split(*res, '\n');
			delete res;

			tmp_v1 = new vector <string>;
			for(j=0; j<(*tmp_v0).size(); j++){
				log << "add: " << (*tmp_v0)[j] << "\n";
				(*tmp_v1).push_back((*tmp_v0)[j]);
			}

			nodes.push_back(tmp_v1);
			delete tmp_v0;
			delete columns;
		}
	}

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');

		if((*columns)[1].find("master") == string::npos){
			cmd = GIT_PATH " log --reverse --first-parent --pretty=format:\"%h\" ";
			cmd += (*columns)[0].c_str();

			log << "execute: " << cmd << "\n";
			res = execute(cmd);
			log << res->c_str() << "\n";

			tmp_v0 = str_split(*res, '\n');
			delete res;

			tmp_v1 = new vector <string>;
			for(j=0; j<(*tmp_v0).size(); j++){
				log << "add: " << (*tmp_v0)[j] << "\n";
				(*tmp_v1).push_back((*tmp_v0)[j]);
			}

			nodes.push_back(tmp_v1);
			delete tmp_v0;
			delete columns;
		}
	}


	log << "\n[nodes - branches]\n";

	for(i=0; i<nodes.size(); i++){
		for(j=0; j<nodes[i]->size(); j++)
			log << (*nodes[i])[j].c_str() << "\n";
	}

	log << "\n";

	delete lines;

	/* get merged branches */
	cmd = GIT_PATH " log --all --merges --pretty=format:\"%h|%p\"";
	res = execute(cmd);
	log << "[merged branches]\n" << res->c_str() << "\n\n";

	lines = str_split(*res, '\n');
	delete res;

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');
		tmp_v0 = str_split((*columns)[1], ' ');	// parents

	
		for(j=1; j<tmp_v0->size(); j++){
			cmd = GIT_PATH " log --reverse --first-parent --pretty=format:\"%h\" ";
			cmd += (*tmp_v0)[j].c_str();
			res = execute(cmd);
			log << "execute: " << cmd << "\n";
			log << res->c_str() << "\n";

			tmp_v1 = str_split(*res, '\n');
			delete res;

			tmp_v2 = new vector <string>;
			for(k=0; k<tmp_v1->size(); k++){
				(*tmp_v2).push_back((*tmp_v1)[k].c_str());
				log << "add: " << (*tmp_v1)[k].c_str() << "\n";
			}
			(*tmp_v2).push_back((*columns)[0].c_str());

			nodes.push_back(tmp_v2);
			delete tmp_v1;
		}

		delete columns;
		delete tmp_v0;
	}

	delete lines;

	printf("processed %d branches\n", nodes.size());

	log << "\n[nodes - merged branches]\n";

	for(i=0; i<nodes.size(); i++){
		for(j=0; j<nodes[i]->size(); j++)
			log << (*nodes[i])[j].c_str() << "\n";
	}

	log << "\n";

	/* generate dot file */
	dot.open(DOT_FILE, ofstream::out);
	dot << "strict digraph \"" REPO_NAME "\" {\n";

	// add nodes
	for(i=0; i<nodes.size(); i++){
		dot << "  node[group=\"" << to_string(i + 1) << "\"];\n  \"" << (*nodes[i])[0].c_str() << "\"";

		for(j=1; j<nodes[i]->size(); j++){
			dot << " -> \"" << (*nodes[i])[j].c_str() << "\"";
		}

		dot << ";\n";
	}

	// add decorations
	i = 1;
	for(map<string, string>::iterator it=decorated_map.begin(); it != decorated_map.end(); i++, it++){
		dot << "  subgraph Decorate" << to_string(i) << "\n  {\n    rank=\"same\";\n    \"" << it->second.c_str();

		if(it->second.find("tag:") != string::npos)
			dot << "\" [shape=\"box\", style=\"filled\", fillcolor=\"#ffffdd\"];\n";
		else
			dot << "\" [shape=\"cds\", style=\"filled\", fillcolor=\"#ddddff\"];\n";

		dot << "    \"" << it->second.c_str() << "\" -> \"" << it->first.c_str() << "\" [weight=0, arrowtype=\"none\", dirtype=\"none\", arrowhead=\"none\", style=\"dotted\"];\n  }\n";
	}

	dot << "}\n";


	dot.close();

	cmd = DOT_PATH " -Tpdf -Gsize=30,30 -o " PDF_FILE " " DOT_FILE;
	execute(cmd);

	log.close();

	return 0;
}
