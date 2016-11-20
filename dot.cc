#include <dot.h>
#include <fstream>
#include <config.h>
#include <utils.h>


void dot_generate(vector <vector <string>*>* graph, map <string, string>* decorations, map <string, string>* commit_info){
	unsigned int i, j;
	ofstream dot;
	string cmd;
	vector <string>* columns;


	dot.open(config.dot_file, ofstream::out);
	dot << "strict digraph \"" << config.repo_name << "\" {\n";

	// add nodes
	for(i=0; i<graph->size(); i++){
		dot << "  node[group=\"" << to_string(i + 1) << "\"];\n";

		// set node style
		for(j=0; j<((*graph)[i])->size(); j++)
			dot << "      \"" << (*(*graph)[i])[j].c_str() << "\"" << " [shape=\"Mrecord\",label=\"" << (*(*graph)[i])[j].c_str() << ": " << (*commit_info)[(*(*graph)[i])[j].c_str()].c_str() << "\"];\n";

		// set graph links
		dot << "      \"" << (*(*graph)[i])[0].c_str() << "\"";

		for(j=1; j<((*graph)[i])->size(); j++)
			dot << " -> \"" << (*(*graph)[i])[j].c_str() << "\"";

		dot << ";\n";
	}

	// add decorations
	i = 1;
	for(map<string, string>::iterator it=(*decorations).begin(); it != (*decorations).end(); i++, it++){
		columns = str_split(it->second.c_str(), ',');

		for(j=0; j<(columns)->size(); j++){
			dot << "  subgraph Decorate" << i << j << "\n  {\n    rank=\"same\";\n    \"" << (*columns)[j].c_str();

			if((*columns)[j].find("tag:") != string::npos)
				dot << "\" [shape=\"record\", style=\"filled\", fillcolor=\"#ffffdd\"];\n";
			else
				dot << "\" [shape=\"cds\", style=\"filled\", fillcolor=\"#ddddff\"];\n";

			dot << "    \"" << (*columns)[j].c_str() << "\" -> \"" << it->first.c_str() << "\" [weight=0, arrowtype=\"none\", dirtype=\"none\", arrowhead=\"none\", style=\"dotted\"];\n  }\n";
		}
	}

	dot << "}\n";

	dot.close();

	// generate pdf
	cmd = config.dot_cmd;
	cmd += " -Tpdf -Gsize=30,30 -o " + (string)config.out_file + " " + config.dot_file;
	execute(cmd);

	if(!config.keep_dot){
		cmd = "rm ";
		cmd += config.dot_file;
		execute(cmd);
	}
}
