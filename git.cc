#include <utils.h>
#include <stdio.h>
#include <git.h>
#include <config.h>
#include <regex>
#include <fstream>


using namespace std;


int git_generate(vector <vector <string>*>* graph, map <string, string>* decorations, map <string, string>* commit_info){
	unsigned int i, j, k;
	string *res;
	string cmd;
	ofstream log;
	vector <string> *lines, *columns, *tmp_v0, *tmp_v1, *tmp_v2;
	vector <vector <string>*> nodes_nonmaster;


	if(config.log)
		log.open(config.log_file, ofstream::out);

	/* get commits */
	cmd = config.git_cmd;

	// commit data
	// 	%h - abbrev hash
	// 	%p - abbrev parent hash
	// 	%cd - date (according to --data option)
	// 	%s - subject
	// 	%d - decorations
	cmd += " log --all --pretty=format:\"%h|%p|%cd|%s|%d\" --date=short";
	res = execute(cmd);
	log << "[commits]\n" << res->c_str() << "\n\n";

	lines = str_split(*res, '\n');
	delete res;

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');

		(*commit_info)[(*columns)[0]] = (*columns)[2] + "\\n" + regex_replace((*columns)[3], regex("[%\">]"), "\\$&");

		if(columns->size() > 4){
			str_trim(&(*columns)[2], "() ");
			(*decorations)[(*columns)[0]] = (*columns)[4];
		}

		delete columns;
	}

	delete lines;

	log << "processed " << (*decorations).size() << " decorates\n";

	log << "[decorated map]\n";
	i = 1;
	for(map<string, string>::iterator it=(*decorations).begin(); it!=(*decorations).end(); i++, it++)
		log << i << ": " << it->first.c_str() << "  " << it->second.c_str() << "\n";
	log << "\n";

	/* get branches */
	cmd = config.git_cmd;
	cmd += " for-each-ref --format=\"%(objectname:short)|%(refname:short)\"";
	res = execute(cmd);
	log << "[branches]\n" << res->c_str() << "\n\n";

	lines = str_split(*res, '\n');
	delete res;

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');

		cmd = config.git_cmd;
		cmd += " log --reverse --first-parent --pretty=format:\"%h\" " + (*columns)[0];

		log << "execute: " << cmd << "\n";
		res = execute(cmd);
		log << res->c_str() << "\n";

		tmp_v0 = str_split(*res, '\n');
		delete res;

		tmp_v1 = new vector <string>;
		for(j=0; j<(*tmp_v0).size(); j++)
			(*tmp_v1).push_back((*tmp_v0)[j]);

		if((*columns)[1].find("master") != string::npos)
			(*graph).push_back(tmp_v1);
		else
			nodes_nonmaster.push_back(tmp_v1);

		delete tmp_v0;
		delete columns;
	}

	(*graph).insert((*graph).end(), nodes_nonmaster.begin(), nodes_nonmaster.end());


	log << "\n[(*graph) - branches]\n";

	for(i=0; i<(*graph).size(); i++){
		for(j=0; j<(*graph)[i]->size(); j++)
			log << (*(*graph)[i])[j].c_str() << "\n";
	}

	log << "\n";

	delete lines;

	/* get merged branches */
	cmd = config.git_cmd;
	cmd += " log --all --merges --pretty=format:\"%h|%p\"";
	res = execute(cmd);
	log << "[merged branches]\n" << res->c_str() << "\n\n";

	lines = str_split(*res, '\n');
	delete res;

	for(i=0; i<lines->size(); i++){
		columns = str_split((*lines)[i], '|');
		tmp_v0 = str_split((*columns)[1], ' ');	// parents

		for(j=1; j<tmp_v0->size(); j++){
			cmd = config.git_cmd;
			cmd += " log --reverse --first-parent --pretty=format:\"%h\" " + (*tmp_v0)[j];
			res = execute(cmd);
			log << "execute: " << cmd << "\n";
			log << res->c_str() << "\n";

			tmp_v1 = str_split(*res, '\n');
			delete res;

			tmp_v2 = new vector <string>;
			for(k=0; k<tmp_v1->size(); k++)
				(*tmp_v2).push_back((*tmp_v1)[k].c_str());
			(*tmp_v2).push_back((*columns)[0].c_str());

			(*graph).push_back(tmp_v2);
			delete tmp_v1;
		}

		delete columns;
		delete tmp_v0;
	}

	delete lines;

	log << "processed " << (*graph).size() << " branches\n";

	log << "\n[(*graph) - merged branches]\n";

	for(i=0; i<(*graph).size(); i++){
		for(j=0; j<(*graph)[i]->size(); j++)
			log << (*(*graph)[i])[j].c_str() << "\n";
	}

	log << "\n";

	if(config.log)
		log.close();

	return 0;
}
