/** CBT Header file **/
/************ GENERAL DEFINES AND TYPEDEFS **********/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#ifndef	USING_DOS
	// since can't use command line compiler argument readily on Macs
	#define	USING_MAC
	/* Non-ANSI functions */
	int	Stricmp(char *x, char *y);
	#define	Strdup(x) (strcpy((char *)malloc(strlen(x)+1),(x)))
#else
	#define	Strdup(x)	strdup(x)
	#define	Stricmp(x,y)	stricmp((x),(y))
#endif

#include "keycodes.h"
#include "utils.h"

/************************/
/** General prototypes **/
/************************/



/********* FUNCTION PROTOTYPES **********/
	/***** TASK SPECIFIC FUNCTIONS ******/
int	parse_args(int argc, char **argv);
int	GetName(void);
void	GetHandednessInfo(void);
void	PrintHandedness(FILE *id);
void	validate_args(void);
int	init_arrays(void);
void	init_palette(void);
void  CalcCounterBalancePcts(int low, int high);
void  PrintCounterBalancePcts(FILE *id);
int	TestCounterBalancePcts(void);
void	RecomputeAnalysis(char *source, char *dest);
int	read_data(FILE *id,int line);
void	set_arrays(int item_num, int index);

void	set_pattern(int pattern);
void	draw(int position, int shape, int color, int num, int size, int filled,int pattern);
int	run_trial(int demo);
void	run_trials(void);
void	store_stimuli(void);
void store_stimuli_as_txt(void);
int	read_stimuli(char *filename);
int	autocreate_stimuli(void);
void	cleanup(void);
pint GetOption(int here, int n);
void	analyze(char *dest, int time_now);
void	calc_matches(void);
int	item_val(int here);
float	Zapprox(int chose, int out_of);
void	PrintCBTscores(FILE *id);
void	PrintGraphs(FILE *id);

int	CounterBalance(void);
void	RandomizeStim(void);
int	NextStim(int index,int stim_left);
int	CheckSameness(int item1, int item2);
void	RevItemVal(int item, int array[10]);
int	show_instructions(void);

int	MS_TIMERset(int id, long ms);
long	MS_TIMERcheck(int id);

int	GetBool(int y, char *msg, char *choices);
int	GetInt(int y, char *msg, int min, int max);
char	*Gets(char *buf);

void	CalibrateSaturation(void);
void	Long2RGB(long lcolor, char *r, char *g, char *b);
long	RGB2Long(char r, char g, char b);
void	RGBshow_vals(int current, char r, char g, char b);

int	CBTparse(int index, int argc, char **argv);
int	CONFIGread_file(char *config_file);
int	CONFIGparse_lines(int num_entries,char **conf_menu);
int CONFIGistag(char *string, int num_entries, char **conf_menu, int *which_var);
char *CONFIGget_arg(void);
int CONFIGisentry(char *string, int num_entries, char **entries);

/**********************/
/** General #defines **/
/**********************/

#define MAX_NEXT_STIM	6000

#define	BOUNDED(x,min,max)	(x=(((x)<(min))?(min):(((x)>(max))?(max):(x))))

#define	BUF_LEN	250

#define	MAX_MATCHES	(max_same_categories+1)

#define	MAX_CATEGORIES	6
#define	MAX_CHOICES_PER_CATEGORY 6
#define	MIN_STIMULI	3
#define	MAX_COLORS		6
#define	MAX_PATTERNS	8

#define	INPUT_NUM_KEYS		0
#define	INPUT_MOUSE_MVT	1
#define	INPUT_MOUSE_BUT	2
#define	INPUT_JOY_MVT		3
#define	INPUT_JOY_BUT		4
#define	INPUT_USER_KEYS	5
#define	INPUT_ARROW_KEYS	6
#define	INPUT_PLUS_ENTER	7

#define	CHOOSE_NOTHING		0
#define	CHOOSE_PREFERENCE	1
#define	CHOOSE_DIFFERENT	2
#define	CHOOSE_SAME			3
#define	CHOOSE_PREF_CONTROL	4

#define	COLORS_FROM_STM_FILE	0
#define	COLORS_FROM_CFG_FILE	1

#define	RAND_DO_NOT				0
#define	RAND_FULLY_MIXED		1
#define	RAND_WITHIN_BLOCK		2

#define MAX_MULTI_EXP	10

#define	VERBOSE_GET_HANDEDNESS	2
#define	VERBOSE_ON					1

#define	AUTOCREATE_COUNTERBALANCE	1

#define	MAX_COLOR_VAL	0x3F

#define	YSEP_PCT		(float) .1
#define	YSEP_INC		(float)	3
#define	h(n)	((char) 0x##n)


/******** GLOBAL VARIABLES ****************/
extern	char	*ExpType[];


extern int	multi_exp[MAX_MULTI_EXP];
extern int	practice_rounds[MAX_MULTI_EXP];
extern int	randomize[MAX_MULTI_EXP];
extern int	immediate_start[MAX_MULTI_EXP];
extern int	all_immediate;

extern int	choice_type;
extern int	num_trials;
extern int	analysis_blocks;
extern int	ms_btwn_stim;
extern int	ms_btwn_trials;
extern int	total_ms_on;
extern int	num_stimuli;
extern int	num_categories;
extern int	num_choices_per_category;
extern int	max_same_categories;
extern int	use_timer;
extern int	mark_correct;
extern int	use_many_colors;
extern int	use_absolute_size;
extern int	use_ega;
extern pchar	datafile;
extern pchar	source_file;
extern float	force_aspect_ratio;
extern int	input_type;
extern int	max_item_num;
extern int	num_warmup_trials;
extern int	did_warmup;
extern int	beep_on_error_key;
extern int	get_colors_from;
extern int	immediacy;
extern int	cleanup_delay;

extern int	autocreate;

extern int	verbose;
extern int	verbose_warmup;
extern int	ask_awareness_of_patterns;


extern int	rand_type;
extern int	entered_handedness;

extern float	XMAX;
extern float	YMAX;
extern int	trial_num;
extern int	trials_per_block;
extern int	times_counterbalanced;
extern int	played_before;
extern pint	_user_keys;

/** These are used by CounterBalancePct to calc quality of randomness **/
extern int	sumOpts[10];
extern int	sumInit[10];
extern float	avgInit[10];
extern float	avgOpts[10];
extern float	avgAll[10];
extern float	zInit[10];
extern float	zOpts[10];
extern float	zAll[10];

extern plong	_time;
extern pint	_choice;
extern pint	_most_similar;
extern pint	_match_min;
extern pint	_match_max;

extern pint	_sameness;		// won't this be the same as _matches?
extern pint	_items;			// which have been used
extern pint	_Orig_stim;		// list of stimuli by item number
extern pint	_stim_copy;			// copy of stimuli item numbers for randomization

extern pint	_shape;
extern pint	_color;
extern pint	_num;
extern pint	_size;
extern pint	_filled;
extern pint	_pattern;
extern pint	_matches;

extern long	_palette[16];

extern char	selfeval_str[BUF_LEN+1];
extern char	*demo_msg;
extern char	*demo_msg0;
extern char	*demo_top_select;
extern char	*demo_top_select0;
extern char	*demo_bottom_select;
extern char	*demo_bottom_select0;

extern char	date_str[20];
extern char	time_str[20];
extern char	exp_type_str[30];

/** Variables for data storage **/
extern int	indivH[12];	// so initially 0!
extern int	familyH[6];	// so initially 0!
extern char	user_name[80];
extern int	age;
extern int	sex;
extern int	years_ed;

extern float Dsize;	// square size of overall stimulus presentation
extern float	Dysep;	// separation between stimuli
extern float	Dx0;

extern int	unit;
