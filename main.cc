#include <map>
#include <vector>
#include <utils.h>
#include <git.h>
#include <dot.h>



int main(int argc, char **argv){
	vector <vector <string>*> graph;
	map <string, string> decorations;
	map <string, string> commit_info;


	/* parse command line */
	if(parse_cmdline(argc, argv))
		return 1;

	/* collect git information */
	git_generate(&graph, &decorations, &commit_info);

	/* generate pdf file */
	dot_generate(&graph, &decorations, &commit_info);

	return 0;
}
