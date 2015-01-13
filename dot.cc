#include <dot.h>
#include <fstream>
#include <config.h>
#include <utils.h>


void dot_generate(vector <vector <string>*>* graph, map <string, string>* decorations){
	unsigned int i, j;
	ofstream dot;
	string cmd;


	dot.open(config.dot_file, ofstream::out);
	dot << "strict digraph \"" << config.repo_name << "\" {\n";

	// add nodes
	for(i=0; i<graph->size(); i++){
		dot << "  node[group=\"" << to_string(i + 1) << "\"];\n  \"" << (*(*graph)[i])[0].c_str() << "\"";

		for(j=1; j<((*graph)[i])->size(); j++){
			dot << " -> \"" << (*(*graph)[i])[j].c_str() << "\"";
		}

		dot << ";\n";
	}

	// add decorations
	i = 1;
	for(map<string, string>::iterator it=(*decorations).begin(); it != (*decorations).end(); i++, it++){
		dot << "  subgraph Decorate" << to_string(i) << "\n  {\n    rank=\"same\";\n    \"" << it->second.c_str();

		if(it->second.find("tag:") != string::npos)
			dot << "\" [shape=\"box\", style=\"filled\", fillcolor=\"#ffffdd\"];\n";
		else
			dot << "\" [shape=\"cds\", style=\"filled\", fillcolor=\"#ddddff\"];\n";

		dot << "    \"" << it->second.c_str() << "\" -> \"" << it->first.c_str() << "\" [weight=0, arrowtype=\"none\", dirtype=\"none\", arrowhead=\"none\", style=\"dotted\"];\n  }\n";
	}

	dot << "}\n";

	dot.close();

	cmd = config.dot_cmd;
	cmd += " -Tpdf -Gsize=30,30 -o " + (string)config.out_file + " " + config.dot_file;
	execute(cmd);

	if(!config.keep_dot){
		cmd = "rm ";
		cmd += config.dot_file;
		execute(cmd);
	}
}
