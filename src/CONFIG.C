#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*************** ROUTINES FOR READING INFO FROM CONFIG FILE ***************/

static char	*CBTparams[] = {
	"verbose",
	"multi_exp",
	"num_stimuli",
	"num_categories",
	"num_choices_per_category",
	"ms_btwn_stim",
	"ms_btwn_trials",
	"total_ms_on",
	"mark_correct",
	"use_absolute_size",
	"use_ega",
	"force_aspect_ratio",
	"max_same_categories",
	"input_type",
	"randomize",
	"datafile",
	"colors",
	"num_trials",
	"analysis_blocks",
	"source_file",
	"practice_rounds",
	"use_many_colors",
	"ask_awareness_of_patterns",
	"verbose_warmup",
	"beep_on_error_key",
	"get_colors_from",
	"immediate_start",
	"cleanup_delay",
};


int	CBTparse(int index, int argc, char **argv)
{
	int	n;
	long	lcolor;

	switch(index) {
		case 0: verbose = atoi(argv[0]); break;
		case 1:
			for (n=0;n<argc && n < MAX_MULTI_EXP; ++n) 
				multi_exp[n] = (1 + CONFIGisentry(argv[n],sizeof(ExpType)/sizeof(char *),ExpType));
			break;
		case 2: num_stimuli = atoi(argv[0]); break;
		case 3: num_categories = atoi(argv[0]); break;
		case 4: num_choices_per_category = atoi(argv[0]); break;
		case 5: ms_btwn_stim = atoi(argv[0]); break;
		case 6: ms_btwn_trials = atoi(argv[0]); break;
		case 7: total_ms_on = atoi(argv[0]); break;
		case 8: mark_correct = atoi(argv[0]); break;
		case 9: use_absolute_size = atoi(argv[0]); break;
		case 10: use_ega = 0; break; // atoi(argv[0]); break;
		case 11: force_aspect_ratio = (float) atof(argv[0]); break;
		case 12: max_same_categories = atoi(argv[0]); break;
		case 13:
			input_type = CONFIGisentry(argv[0],sizeof(InputType)/sizeof(char *),InputType);
			if (input_type == -1)
				return 0;
			break;
		case 14:
			for (n=0;n<argc && n < MAX_MULTI_EXP; ++n) {
				randomize[n] = CONFIGisentry(argv[n],sizeof(RandType)/sizeof(char *),RandType);
				if (randomize[n] == -1)
					randomize[n] = 0;
			}
			break;
		case 15: datafile = strdup(argv[0]); break;
		case 16:
			/** Parse color codes **/
			for (n=0;n<argc;++n) {
				if (n > 15)	// max of 16 colors
					break;
				if (sscanf(argv[n],"%8lxH",&lcolor) == 1) {
					_palette[n] = lcolor;
				}
			}
			break;
		case 17: num_trials = atoi(argv[0]); break;
		case 18: analysis_blocks = atoi(argv[0]); break;
		case 19: source_file = strdup(argv[0]); break;
		case 20:
			for (n=0;n<argc && n < MAX_MULTI_EXP; ++n) 
				practice_rounds[n] = atoi(argv[n]);
			break;
		case 21: use_many_colors=atoi(argv[0]); break;
		case 22: ask_awareness_of_patterns = atoi(argv[0]); break;
		case 23: verbose_warmup = atoi(argv[0]); break;
		case 24: beep_on_error_key = atoi(argv[0]); break;
		case 25: get_colors_from = CONFIGisentry(argv[0],2,ColorFromType); break;
		case 26:
			all_immediate = 1;
			for (n=0;n<argc && n < MAX_MULTI_EXP; ++n) {
				immediate_start[n] = atoi(argv[0]);
				BOUNDED(immediate_start[n],0,1);
				all_immediate = (all_immediate && immediate_start[n]);
			}
			break;
		case 27: cleanup_delay = atoi(argv[0]); break;
		default: return 0;
	}
	return 1;
}


#define	CONFIG_MAX_BUF_LEN			250
#define	CONFIG_MAX_FILENAME_LEN		78
#define	CONFIG_MAX_ARGS				30			// max arguments to any tag

FILE	*CONFIGfd;
char	CONFIGbuf[CONFIG_MAX_BUF_LEN+1];
int	CONFIGline;
int	CONFIGerrs;

int	CONFIGread_file(char *config_file)
{
	register	int	n;
	char	*slist[2];
	char	**s = &slist[1];
	int	num_entries;
	char	**conf_menu = CBTparams;

	CONFIGerrs = 0;

	for (n=0;conf_menu[n][0] != '\0';++n);
	num_entries = n;

	if ((CONFIGfd = fopen(config_file,"r+t")) == (FILE *) NULL) 
		return 0;

	if (!CONFIGparse_lines(num_entries, conf_menu)) {
		fclose(CONFIGfd);
		return 0;
	}
	fclose(CONFIGfd);

	return 1;
}

int	CONFIGparse_lines(int num_entries, char **conf_menu)
{
	char  *token="";
	int	which_tag;
	char	*argv_list[CONFIG_MAX_ARGS+1];
	char	**argv = &argv_list[1];	// so that argv[-1] = command tag
	int	argc;


	for (token=CONFIGget_arg();token!=(char *) 0;token=CONFIGget_arg()) {
		if (*token == '\n') {	// blank line
			continue;
		}

		/* Is first arg on the line a valid tag? */
		if (!CONFIGistag(token,num_entries,conf_menu,&which_tag)){
			printf("WARN(L%i): Invalid tag \"%s\"\n",CONFIGline, token);
			++CONFIGerrs;
			continue;
		}

		/** Make array of grabbed tokens and pass them to parser.  Note that
			token replaces occurances of tokensep with '\0', so buf becomes
			a discrete set of strings whose pointers can be stored in argv **/

		argv[-1] = token;
		argc = 0;
		while ((token = CONFIGget_arg()) != (char *) 0) {
			if (argc >= CONFIG_MAX_ARGS) {
				(void) printf("ERR(L%i):  Too many args (max %i)\n",
					CONFIGline, CONFIG_MAX_ARGS);
				++CONFIGerrs;
				goto next_line;
			}
			if (*token == '\n')
				break;
			argv[argc++] = token;
		}
		if (!CBTparse(which_tag, argc, argv)) {
			++CONFIGerrs;
			continue;
		}
		continue;

next_line:
		while ((token=CONFIGget_arg()) != (char *) 0)
			if (*token == '\n')
				break;
	}
	if (CONFIGerrs > 0) {
		printf("<press a key to continue>");
		GetAKey();
	}
	return (!CONFIGerrs);
}

char *CONFIGget_arg(void)
{
	/** Return '\n' when at end of line (so know length), and '\0' for EOF **/

	static	char *copy = (char *) 0;
	static	int	get_next_line=1;
	static	int	init=0;
	char		*token;
	char		*c;
	int		num_quotes;

	if (!init) {
		init=1;
		CONFIGline=1;
	}

	if (get_next_line) {
		get_next_line=0;
read_next_line:
		if (fgets(CONFIGbuf,CONFIG_MAX_BUF_LEN,CONFIGfd) == NULL)
			goto end_of_file;

		++CONFIGline;	// since disallow ones which are too long

		if (strchr(CONFIGbuf,'\n') == NULL) {
			(void) printf("ERR(L%i):  Line too long (max %i): use continuation character '\\'\n",
				CONFIGline, CONFIG_MAX_BUF_LEN);
			goto end_of_file;
		}
		/** Remove comments **/
		if ((c = strchr(CONFIGbuf,'/')) != NULL) {
			if (*(c+1) == '/') {
				*c = '\n';	// so know that end of line
				*(c+1) = '\0';	// so that effective end of line
			}
		}
		copy = CONFIGbuf;
	}

	/** Skip whitespace to start of next token **/
	for (;1;++copy) {
		if (*copy == '\0') 
			goto read_next_line;
		if (*copy == ' ' || *copy == '\t')
			continue;
		else if (*copy == '\\' && *(copy+1) == '\n') {
			goto read_next_line;
		}
		else if (*copy == '\n') {
			get_next_line=1;
			return "\n";	// so know that end of line has been reached
		}
		else
			break;	// found a non-whitespace character
	}

	/** working from start of token, move 'copy' to end of token **/
	for (token=copy,num_quotes=0;1;++copy) {
		if (num_quotes) {
			if (*copy == '\n' || *copy == '\0' || (*copy == '\\' && *(copy+1) == '\n')) {
				printf("ERR(L%i):  Strings may not be continued across newlines\n",CONFIGline);
				goto end_of_file;
			}
			if (*copy == '\"') {
				*copy = '\0';
				++copy;
				return token;
			}
			else
				continue;
		}
		else {
			if (*copy == '\0') {
				printf("ERR(L%i):  Invalid terminating NULL: algorithm problem\n", CONFIGline);
				goto end_of_file;
			}
			else if (*copy == '\n' || (*copy == '\\' && *(copy+1) == '\n')) {
				*(copy+1) = *copy;	// so that next token is end-of-line
				*copy = '\0';
				++copy;
				return token;
			}
			else if (*copy == ' ' || *copy == '\t') {
				*copy = '\0';
				++copy;
				return token;
			}
			else if (*copy == '\"') {
				num_quotes=1;
				token=copy+1;
			}
			/** Else move to next non-whitespace character **/
		}
	}
end_of_file:
	get_next_line=1;
	return (char *) 0;
}

int CONFIGistag(char *string, int num_entries, char **conf_menu, int *which_var)
{
	for (*which_var=0;*which_var<num_entries;++*which_var){
		if (stricmp(string,conf_menu[*which_var]) == 0) {
			return(1);
		}
	}
	return(0);
}

int CONFIGisentry(char *string, int num_entries, char **entries)
{
	/* Returns -1 if not found, else index within array "entries" */
	int	n;
	for (n=0;n<num_entries;++n){
		if (stricmp(string,entries[n]) == 0) {
			return n;
		}
	}
	(void) printf("Invalid entry '%s':  Valid entries are:\n", string);
	for (n=0;n<num_entries;++n) {
		printf("\t%s",entries[n]);
	}
	printf("\n");
	++CONFIGerrs;
	return -1;
}

