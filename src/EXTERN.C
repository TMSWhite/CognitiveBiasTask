#include "cbt.h"

/******** GLOBAL VARIABLES ****************/
extern	char	*ExpType[];


int	multi_exp[MAX_MULTI_EXP];
int	practice_rounds[MAX_MULTI_EXP];
int	randomize[MAX_MULTI_EXP];
int	immediate_start[MAX_MULTI_EXP];
int	all_immediate=0;

int	choice_type=CHOOSE_NOTHING;
int	num_trials=-2;
int	analysis_blocks=1;
int	ms_btwn_stim=0;
int	ms_btwn_trials=0;
int	total_ms_on=0;
int	num_stimuli=3;
int	num_categories=5;
int	num_choices_per_category=2;
int	max_same_categories=5;
int	use_timer=0;
int	mark_correct=0;
int	use_many_colors=0;
int	use_absolute_size=1;
int	use_ega=1;
pchar	datafile="cbt_data";
pchar	source_file=NULL;
float	force_aspect_ratio=1;
int	input_type=INPUT_ARROW_KEYS;
int	max_item_num;
int	num_warmup_trials;
int	did_warmup;
int	beep_on_error_key=0;
int	get_colors_from=COLORS_FROM_CFG_FILE;
int	immediacy=0;
int	cleanup_delay=0;

int	autocreate=0;

int	verbose=VERBOSE_ON;
int	verbose_warmup=0;
int	ask_awareness_of_patterns=0;


int	rand_type;
int	entered_handedness = 0;

float	XMAX;
float	YMAX;
int	trial_num;
int	trials_per_block;
int	times_counterbalanced;
int	played_before=0;
pint	_user_keys=NULL;

/** These are used by CounterBalancePct to calc quality of randomness **/
int	sumOpts[10];
int	sumInit[10];
float	avgInit[10];
float	avgOpts[10];
float	avgAll[10];
float	zInit[10];
float	zOpts[10];
float	zAll[10];

plong	_time=NULL;
pint	_choice=NULL;
pint	_most_similar=NULL;
pint	_match_min=NULL;
pint	_match_max=NULL;

pint	_sameness;		// won't this be the same as _matches?
pint	_items;			// which have been used
pint	_Orig_stim;		// list of stimuli by item number
pint	_stim_copy;			// copy of stimuli item numbers for randomization

pint	_shape=NULL;
pint	_color=NULL;
pint	_num=NULL;
pint	_size=NULL;
pint	_filled=NULL;
pint	_pattern=NULL;
pint	_matches=NULL;

long	_palette[16];

char	selfeval_str[BUF_LEN+1];
char	*demo_msg = "%schoose (from the cards below) the one %s";
char	*demo_msg0;
char	*demo_top_select = "Choose top card by %s ->";
char	*demo_top_select0;
char	*demo_bottom_select = "Choose bottom card by %s ->";
char	*demo_bottom_select0;

char	date_str[20];
char	time_str[20];
char	exp_type_str[30];

/** Variables for data storage **/
int	indivH[12];	// so initially 0!
int	familyH[6];	// so initially 0!
char	user_name[80];
int	age=0;
int	sex=0;
int	years_ed=0;

float Dsize;	// square size of overall stimulus presentation
float	Dysep;	// separation between stimuli
float	Dx0;

char	vertical[8] = { h(CC), h(CC), h(CC), h(CC), h(CC), h(CC), h(CC), h(CC) };
char	diagonal[8] = { h(99), h(33), h(66), h(CC), h(99), h(33), h(66), h(CC) };
char	horizontal[8] = { h(FF), h(00), h(FF), h(00), h(FF), h(00), h(FF), h(00) };
char	spotted[8] = { h(00), h(66), h(66), h(00), h(00), h(66), h(66), h(00) };
char	arrows[8] = { h(CC), h(66), h(33), h(99), h(99), h(33), h(66), h(CC) };
char	crosshash[8] = { h(22), h(55), h(88), h(55), h(22), h(55), h(88), h(55) };
char	waterhash[8] = { h(88), h(55), h(22), h(00), h(88), h(55), h(22), h(00) };

int	unit=1;
