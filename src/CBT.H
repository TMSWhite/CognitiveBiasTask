/** CBT Header file **/
/************ GENERAL DEFINES AND TYPEDEFS **********/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#define	USING_MAC

#ifdef USING_MAC

#define	far
#define	interrupt

#define	strdup(x) strcpy(malloc(strlen((x)+1)),(x))
int	stricmp(char *x, char *y);

int	stricmp(char *x, char *y)
{
	char *a, *b;
	int	n=0;
	
	a = strdup(x);
	b = strdup(y);
	if (!a || !b)
		goto cleanup;
		
	for (n=0;n<strlen(a);++n)
		a[n] = tolower(a[n]);
	for (n=0;n<strlen(b);++n)
		b[n] = tolower(b[n]);
		
	n = strcmp(a,b);
cleanup:

	if (a)
		free(a);
	if (b)
		free(b);
	return n;
}

#define	_clearscreen(x)	Cls()

void	Cls(void);

#endif	// USING_MAC



#define MAX_NEXT_STIM	6000

#define K_ESCAPE        0x011b
#define K_LEFTARROW     0x4b00
#define K_RIGHTARROW    0x4d00
#define K_UPARROW       0x4800
#define K_DOWNARROW     0x5000
#define K_PAGEUP        0x4900
#define K_PAGEDN        0x5100
#define K_HOME          0x4700
#define K_END           0x4f00
#define K_GREY_PLUS		0x4e2b
#define K_GREY_MINUS		0x4a2d
#define K_F1				0x3b00
#define K_F10				0x4400
#define K_RETURN			0x1c0d

#define	TEXT_HIGHLIGHTED	0x70
#define	TEXT_BOLD			0x0f
#define	TEXT_NORMAL			0x07

#define	MODE_GRAPHICS	0
#define	MODE_TEXT		1

#define BLACK 				   0
#define BLUE				   1
#define GREEN				   2
#define CYAN				   3
#define RED					   4
#define MAGENTA			   5
#define BROWN				   6
#define LIGHTGRAY			   7
#define DARKGRAY 			   8
#define LIGHTBLUE			   9
#define LIGHTGREEN		  10
#define LIGHTCYAN			  11
#define LIGHTRED			  12
#define LIGHTMAGENTA 	  13
#define YELLOW				  14
#define WHITE				  15

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

#define MAX_MULTI_EXP	10
int	multi_exp[MAX_MULTI_EXP];
int	practice_rounds[MAX_MULTI_EXP];
int	randomize[MAX_MULTI_EXP];
int	immediate_start[MAX_MULTI_EXP];
int	all_immediate=0;

char	*ExpType[] = {
	"Preference", "Most_Different", "Most_Similar", "Preference_Control"
};

char	*InputType[] = {
	"Num_Keys", "Mouse_Mvt", "Mouse_Buttons", "Joystick_Mvt",
	"Joystick_buttons", "User_Keys", "Arrow_Keys", "Plus_Enter"
};

#define	COLORS_FROM_STM_FILE	0
#define	COLORS_FROM_CFG_FILE	1
char	*ColorFromType[] = {"From_STM_File", "From_CFG_File" };

#define	MIN_MOUSE_MVT		2
#define	MIN_JOY_MVT			2
#define	MAX_JOY_DELAY		(0xFFFF)

#define	BOUNDED(x,min,max)	(x=(((x)<(min))?(min):(((x)>(max))?(max):(x))))

#define	CLOCK_RATE		1000
#define	BUF_LEN	250

#define	MAX_MATCHES	(max_same_categories+1)
#define	BCKG_COLOR	0
#define	BASE_COLOR	1
#define	FIRST_COLOR	2

typedef	unsigned char	uchar;
typedef	unsigned int	uint;
typedef	char	*	pchar;
typedef	int	*	pint;
typedef	long	*	plong;
typedef	float	*	pfloat;

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
void	page_flip(int option);
void	calc_matches(void);
int	item_val(int here);
float	Zapprox(int chose, int out_of);
void	PrintCBTscores(FILE *id);
void	PrintGraphs(FILE *id);

int	CounterBalance(void);
void	RandomizeStim(void);
int	NextStim(int index,int stim_left);
int	CheckSameness(int item1, int item2);
void	RevItemVal(int item, int *array);
int	show_instructions(void);

	/******* ACCESSORY FUNCTIONS **********/
void SCREENmode(int mode);
int VGA_ellipse(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color);
int VGA_rect(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color);
int VGA_polygon(int num_points, pint Xpoints, pint Ypoints,
	int filled, int color);
int VGA_reg_polygon(int num_sides, float cent_x, float cent_y,
	float radius, float degrees, int filled, int color);
long	VGA_color(int color);

int	MS_TIMERset(int id, long ms);
long	MS_TIMERcheck(int id);void	CLOCKbind(void (interrupt far *int08)(), int rate);
void	CLOCKunbind(void);
void	interrupt far CLOCKcounter(void);

int	JoystickInit(void);
int	JoystickPressed(int *buttons);
int	JoystickMoved(int *deltaX, int *deltaY);
int	mouseInit(void);
int	mousePressed(int *buttons);
int	mouseMoved(int *deltaX, int *deltaY);
int	GetBool(int y, char *msg, char *choices);
int	GetInt(int y, char *msg, int min, int max);

void BlankLine(int y);
void WriteAttrString(char *string, int x, int y, int attributes);
void WriteAttrChar(char c, int x, int y, int attributes);
void	GetCurPos(int *x, int *y);
void CurMov(int x, int y);
void	CalibrateSaturation(void);
void	Long2RGB(long lcolor, char *r, char *g, char *b);
long	RGB2Long(char r, char g, char b);
void	RGBshow_vals(int current, char r, char g, char b);

unsigned	GetAKey(void);
int KeyPressed(void);

int	CBTparse(int index, int argc, char **argv);
int	CONFIGread_file(char *config_file);
int	CONFIGparse_lines(int num_entries,char **conf_menu);
int CONFIGistag(char *string, int num_entries, char **conf_menu, int *which_var);
char *CONFIGget_arg(void);
int CONFIGisentry(char *string, int num_entries, char **entries);

/******** GLOBAL VARIABLES ****************/

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

#define	AUTOCREATE_COUNTERBALANCE	1
int	autocreate=0;

#define	VERBOSE_GET_HANDEDNESS	2
#define	VERBOSE_ON					1
int	verbose=VERBOSE_ON;
int	verbose_warmup=0;
int	ask_awareness_of_patterns=0;

char	*RandType[] = { "Do_Not", "Fully_Mixed", "Within_Block"};

#define	RAND_DO_NOT				0
#define	RAND_FULLY_MIXED		1
#define	RAND_WITHIN_BLOCK		2
int	rand_type;
int	entered_handedness = 0;

volatile long	TIMERms_counter;
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

#define	MAX_COLOR_VAL	0x3F

/** Variables for data storage **/
static	int	indivH[12];	// so initially 0!
static	int	familyH[6];	// so initially 0!
static	char	user_name[80];
static	int	age=0;
static	int	sex=0;
static	int	years_ed=0;


/********** SEMI-LOCAL VARIABLES ARE STORED ABOVE CODE MODULES **********/
/********************************************************************/

#define	YSEP_PCT		(float) .1
#define	YSEP_INC		(float)	3

float Dsize;	// square size of overall stimulus presentation
float	Dysep;	// separation between stimuli
float	Dx0;

#define	h(n)	((char) 0x##n)
char	vertical[8] = { h(CC), h(CC), h(CC), h(CC), h(CC), h(CC), h(CC), h(CC) };
char	diagonal[8] = { h(99), h(33), h(66), h(CC), h(99), h(33), h(66), h(CC) };
char	horizontal[8] = { h(FF), h(00), h(FF), h(00), h(FF), h(00), h(FF), h(00) };
char	spotted[8] = { h(00), h(66), h(66), h(00), h(00), h(66), h(66), h(00) };
char	arrows[8] = { h(CC), h(66), h(33), h(99), h(99), h(33), h(66), h(CC) };
char	crosshash[8] = { h(22), h(55), h(88), h(55), h(22), h(55), h(88), h(55) };
char	waterhash[8] = { h(88), h(55), h(22), h(00), h(88), h(55), h(22), h(00) };

int	unit=1;
