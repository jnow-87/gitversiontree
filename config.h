#ifndef CONFIG_H
#define CONFIG_H


#define GIT_PATH	"/usr/bin/git"
#define DOT_PATH	"/usr/bin/dot"
#define REPO_NAME	"test-repo"
#define LOG_FILE	"/tmp/gitversiontree.log"
#define DOT_FILE	"/tmp/gitversiontree.dot"
#define PDF_FILE	"graph.pdf"


enum options{
	OPT_HELP = 1,
	OPT_OFILE,
	OPT_LOG,
	OPT_KEEPDOT
};


struct config_t{
	char *git_cmd,
		 *dot_cmd,
		 *repo_name,
		 *out_file,
		 *log_file,
		 *dot_file;

	bool log,
		 keep_dot;
};


extern config_t config;


#endif // CONFIG_H
