#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <graph.h>
#include <malloc.h>
#include <math.h>
#include <conio.h>	// for kbhit()
#include <bios.h>		// for _bios_keybrd()
#include <time.h>
#include <dos.h>
#include <ctype.h>
#include <graph.h>

/************ GENERAL DEFINES AND TYPEDEFS **********/
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
long	MS_TIMERcheck(int id);
void	CLOCKbind(void (interrupt far *int08)(), int rate);
void	CLOCKunbind(void);
void	interrupt far CLOCKcounter(void);

int	JoystickInit(void);
int	JoystickPressed(int *buttons);
int	JoystickMoved(int *deltaX, int *deltaY);
int	mouseInit(void);
int	mousePressed(int *buttons);
int	mouseMoved(int *deltaX, int *deltaY);
int	GetBool(int y, char *msg, char *choices);

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

/***************** START OF CODE ************/

void main(int argc, char **argv)
{
	int	cycle;

	init_palette();

	if (!parse_args(argc,argv))
		return;
	
	if (verbose) {
		char *title="CBT - Neuropsychological Testing";
		char *to_start = "Press a key to begin";
		_clearscreen(0);
		WriteAttrString(title,(40-strlen(title)/2),12,TEXT_BOLD);
		WriteAttrString(to_start,(40-strlen(to_start)/2),14,TEXT_NORMAL);
		GetAKey();
	}

	if (verbose)
		played_before = GetName();

	if (verbose == VERBOSE_GET_HANDEDNESS && !played_before) {
		GetHandednessInfo();
	}

	/* For multi exps, assumes that you want to use exactly the same
		order each time.  To re-order, run from scratch several times */

	if (all_immediate) {
		if (use_timer)
			CLOCKbind(CLOCKcounter,CLOCK_RATE);
	}


	for (cycle=0;cycle<MAX_MULTI_EXP;++cycle) {
		if (multi_exp[cycle] == CHOOSE_NOTHING)
			break;
		rand_type = randomize[cycle];
		choice_type = multi_exp[cycle];
		num_warmup_trials = practice_rounds[cycle];
		immediacy = immediate_start[cycle];
		did_warmup = 0;
		RandomizeStim();	// if so required
		if (!immediacy)
			MS_TIMERset(0,0);	// remove initial delay if not needed
		run_trials();
	}

	if (all_immediate){
		if (use_timer)
			CLOCKunbind();
	}

	if (cleanup_delay && use_timer) {
		long	time;
		CLOCKbind(CLOCKcounter,CLOCK_RATE);
		time=TIMERms_counter+cleanup_delay;
		while(TIMERms_counter < time)
			;
		CLOCKunbind();
	}

	SCREENmode(MODE_TEXT);

	cleanup();

	if (verbose) printf("Thank you\n");
}

void	run_trials(void)
{
	char	buf[80];
	int	n;

	if (!show_instructions()) 
		return;	// allows early abort via ESCAPE

	SCREENmode(MODE_GRAPHICS);

	/* Must reset response arrays each time new exp is run **/
	for (n=0;n<num_trials;++n){
		_choice[n]=0;	// to indicate that nothing selected
		_time[n]=0L;
	}

	if (!immediacy) {
		if (use_timer)
			CLOCKbind(CLOCKcounter,CLOCK_RATE);
	}

	for (trial_num=0;trial_num<num_trials;++trial_num) {
		if (run_trial(0)==-1)	// ESCAPE pressed
			break;
	}
	if (!immediacy) {
		if (use_timer)
			CLOCKunbind();

		SCREENmode(MODE_TEXT);
	}

	if (ask_awareness_of_patterns && (choice_type == CHOOSE_PREFERENCE || choice_type == CHOOSE_PREF_CONTROL)) {
		if (GetBool(1,"Were you aware of any conscious pattern(s) to your preferences","ny")) {
			printf("Please briefly describe them\n->");
			gets(selfeval_str);
		}
	}

	store_stimuli();
	store_stimuli_as_txt();

	sprintf(buf,"%s.dat",datafile);
	analyze(buf,1);
}

int	show_instructions(void)
{
	char	buf[80];
	int	i;
	int	ok=0;

	_settextcolor(7);	// NORMAL COLOR
	trial_num=0;	// so all demos do not fill response arrays

	if (immediacy == 1) {
		return 1;
	}

	if (verbose) {
		_clearscreen(0);
		WriteAttrString("INSTRUCTIONS",34,1,TEXT_HIGHLIGHTED);
		WriteAttrString("You will see cards that have different designs on them.  The designs",1,3,TEXT_NORMAL);
		if (choice_type == CHOOSE_PREF_CONTROL) {
			WriteAttrString("may vary in several respects.  Of the cards shown, choose the one you",1,4,TEXT_NORMAL);
			WriteAttrString("like the most.",1,5,TEXT_BOLD);
		}
		else {
			WriteAttrString("may vary in several respects.  After looking at the top card,",1,4,TEXT_NORMAL);
			sprintf(buf,"%i new cards will appear below it.  Please choose the one card from the",(num_stimuli-1));
			WriteAttrString(buf,1,5,TEXT_NORMAL);
		}

		switch(choice_type) {
			case CHOOSE_PREFERENCE:
//				WriteAttrString("Please choose the card from the new set which you ",1,5,TEXT_NORMAL);
				WriteAttrString("new set which you ", 1, 6, TEXT_NORMAL);
				WriteAttrString("like the best.",0,0,TEXT_BOLD);
				demo_msg0 = "you like the best";
				break;
			case CHOOSE_SAME:
				WriteAttrString("new cards which is ", 1, 6, TEXT_NORMAL);
//				WriteAttrString("Please choose the card from the new set which is ",1,5,TEXT_NORMAL);
				WriteAttrString("most similar",0,0,TEXT_BOLD);
				WriteAttrString(" to the first card.",0,0,TEXT_NORMAL);
				demo_msg0 = "MOST SIMILAR to the one above";
				break;
			case CHOOSE_DIFFERENT:
				WriteAttrString("new cards which is ", 1, 6, TEXT_NORMAL);
//				WriteAttrString("Please choose the card from the new set which is ",1,5,TEXT_NORMAL);
				WriteAttrString("most different",0,0,TEXT_BOLD);
				WriteAttrString(" from the first card.",0,0,TEXT_NORMAL);
				demo_msg0 = "MOST DIFFERENT from the one above";
				break;
			case CHOOSE_PREF_CONTROL:
				demo_msg0 = "you like the best";
				break;
		}
		if (choice_type == CHOOSE_PREFERENCE || choice_type == CHOOSE_PREF_CONTROL) {
			WriteAttrString("There are no \"correct\" or \"incorrect\" responses.  Your choice",1,8,TEXT_NORMAL);
			WriteAttrString("is entirely up to you.",1,9,TEXT_NORMAL);
		}
		WriteAttrString("Please try to make your choices rather quickly.",1,11,TEXT_NORMAL);

		WriteAttrString("When making selection,",1,14,TEXT_NORMAL);
		WriteAttrString("select UPPER card by:",3,15,TEXT_NORMAL);
		WriteAttrString("select LOWER card by:",3,16,TEXT_NORMAL);
		switch(input_type) {
			case INPUT_NUM_KEYS:
				demo_top_select0="pressing number '1'";
				demo_bottom_select0="pressing number '2'    ";	// allocates sufficient space
				sprintf(demo_bottom_select0,"pressing number '%i'",(num_stimuli-1));
				break;
			case INPUT_MOUSE_MVT:
				demo_top_select0="moving mouse up";
				demo_bottom_select0="moving mouse down";
				break;
			case INPUT_MOUSE_BUT:
				demo_top_select0="clicking left mouse button";
				demo_bottom_select0="clicking right mouse button";
				break;
			case INPUT_JOY_MVT:
				demo_top_select0="pulling joystick down";
				demo_bottom_select0="pushing joystick up";
				break;
			case INPUT_JOY_BUT:
				demo_top_select0="pressing 1st joystick button";
				demo_bottom_select0="pressing 2nd joystick button";
				break;
			case INPUT_USER_KEYS:
				demo_top_select0="pressing letter 'c'  ";
				sprintf(demo_top_select0,"pressing letter '%c'",_user_keys[0]);
				demo_bottom_select0="pressing letter 'c'  ";
				sprintf(demo_bottom_select0,"pressing letter '%c'",_user_keys[num_stimuli-1]);
				break;
			case INPUT_ARROW_KEYS:
				demo_top_select0="pressing UP arrow";
				demo_bottom_select0="pressing DOWN arrow";
				break;
			case INPUT_PLUS_ENTER:
				demo_top_select0="pressing GREY-PLUS key";
				demo_bottom_select0="pressing GREY-ENTER key";
				break;
		}
		WriteAttrString(demo_top_select0,26,15,TEXT_NORMAL);
		WriteAttrString(demo_bottom_select0,26,16,TEXT_NORMAL);

		WriteAttrString("Press F1 for help", 1, 25, TEXT_NORMAL);
		WriteAttrString("Press SPACE to start", 60, 25, TEXT_BOLD);

		while (!ok) {
			i = GetAKey();
			if (i == K_F1) {
				SCREENmode(MODE_GRAPHICS);
				if (use_timer)
					CLOCKbind(CLOCKcounter,CLOCK_RATE);
				while(run_trial(1)==0)
					;
				if (use_timer)
					CLOCKunbind();
				SCREENmode(MODE_TEXT);
				return show_instructions();
			}
			if (i == K_ESCAPE)
				return 0;

			if ((char) i == ' ') {
				ok = 1;
				break;
			}
		}
	}
	else {
		GetAKey();
	}

	SCREENmode(MODE_GRAPHICS);

	if (!did_warmup && num_warmup_trials > 0) {
		if (verbose_warmup) {
			_settextcolor(15);	// WHITE
			_settextposition(12,20);
			_outtext("Press SPACE to try a few practice rounds");
			if (use_ega)
				page_flip(0);	// so displayed

			while(1) {
				i = GetAKey();
				if (i == K_ESCAPE)
					break;
				if (i == K_F1) {
					if (use_timer)
						CLOCKbind(CLOCKcounter,CLOCK_RATE);
					while(run_trial(1)==0)	// show graphical help
						;
					if (use_timer)
						CLOCKunbind();
				}

				if ((char) i == ' ')
					break;
			}
		}
		if (use_timer)
			CLOCKbind(CLOCKcounter,CLOCK_RATE);
	
		/* Require X answered trials in a row */
		i=0;
		do {
			switch(run_trial(2)) {
				case 1: ++i; break;
				case 0: i=0; break;
				case -1: i=num_warmup_trials; break;	// abort early
			}
		} while(i<num_warmup_trials);
		did_warmup=1;

		if (use_timer)
			CLOCKunbind();

		if (verbose_warmup) {
			if (use_ega)
				page_flip(0);	// so displayed
			else
				_clearscreen(0);
			_settextcolor(15);	// WHITE
			_settextposition(12,19);
			_outtext("Great!  Now press SPACE to start experiment");
			if (use_ega)
				page_flip(0);	// so displayed

			while(1) {
				i = GetAKey();
				if (i == K_ESCAPE)
					return 0;
				if ((char) i == ' ')
					break;
			}
		}
	}
	return 1;
}

int	parse_args(int argc, char **argv)
{
	/** to make my life easier for adding stuff **/
	char	type = *argv[1];
	long	lcolor;
	int	num_args;
	int	n;

	if (argc == 1)
		goto usage_msg;

	switch(type) {
		case 'c':
		case 'C':
			/** Parse color codes **/
			num_args=1;
			for (n=num_args+1;n<argc;++n) {
				if (n-num_args > 15)	// max of 16 colors
					break;
				if (sscanf(argv[n],"%8lxH",&lcolor) == 1) {
					_palette[n-num_args] = lcolor;
				}
			}
			CalibrateSaturation();
			return 0;	// so that breaks out of main
			break;
		case 'r':
		case 'R':
			if (argc != 5)
				goto usage_msg;
			analysis_blocks=atoi(argv[2]);
			RecomputeAnalysis(argv[3],argv[4]);
			return 0;
		case 'f':
		case 'F':
			num_args = 3;
			if (!CONFIGread_file(argv[2]))
				return 0;
			if (source_file) {
				if (!read_stimuli(source_file))
					return 0;
			}
			else {
				validate_args();
				if (!init_arrays())
					return 0;
				if (!autocreate_stimuli())
					return 0;
			}
			break;
		default:	goto usage_msg;
	}
		
	return 1;

usage_msg:
	printf("CBT program accepts these command line arguments:\n\n");
	printf("\nCBT C [optional color codes]  - lets you calibrate the color saturation\n");
	printf("\nCBT F config_file             - run experiment(s) according to config_file\n");
	printf("\nCBT R num_blocks source dest  - recomputes statistics on .DAT file\n");

	return 0;
}

void	RecomputeAnalysis(char *source, char *dest)
{
	FILE 	*id;
	int	line=1;
	int	num_blocks=analysis_blocks;

	printf("Reading data from \"%s\"...\n",source);

	if ((id=fopen(source,"r+")) == (FILE *) 0) {
		printf("File \"%s\" does not exist\n", source);
		return;
	}
	do {
		analysis_blocks=num_blocks;
		line=read_data(id,line);
		BOUNDED(analysis_blocks,1,num_trials);
		analyze(dest,0);
		cleanup();	// so that free for next time!
	} while (line != -1);


	fclose(id);
}

void	validate_args(void)
{
	int	n;

	if (num_choices_per_category < 0) {
		num_choices_per_category=-num_choices_per_category;
		use_many_colors=1;
	}
	else {
		use_many_colors=0;
	}

	BOUNDED(verbose,0,2);
	BOUNDED(choice_type,0,4);
	BOUNDED(num_stimuli,MIN_STIMULI,10);
	BOUNDED(num_categories,1,MAX_CATEGORIES);
	BOUNDED(num_choices_per_category,1,MAX_CHOICES_PER_CATEGORY);
	BOUNDED(ms_btwn_stim,0,10000);
	BOUNDED(ms_btwn_trials,0,10000);
	BOUNDED(total_ms_on,0,10000);
	BOUNDED(mark_correct,0,1);
	BOUNDED(use_absolute_size,0,1);
	BOUNDED(use_ega,0,1);
	BOUNDED(force_aspect_ratio,(float)0,force_aspect_ratio);
	BOUNDED(max_same_categories,1,num_categories);
	BOUNDED(input_type,0,7);
	BOUNDED(rand_type,0,2);
	BOUNDED(familyH[3],0,familyH[2]);
	BOUNDED(familyH[5],0,familyH[4]);

	if (ms_btwn_stim==0 && ms_btwn_trials==0 && total_ms_on==0)
		use_timer=0;
	else
		use_timer=1;

	for (n=0;n<=max_same_categories;++n)
		trials_per_block+=n;

	if (num_trials < 0) {
		int	n;

		analysis_blocks=(-num_trials*2);

		times_counterbalanced=(-num_trials);

		num_trials = 2 * (-num_trials) * trials_per_block;
		autocreate=AUTOCREATE_COUNTERBALANCE;
	}
	else {
		autocreate=0;
	}
}

/*******************************************************************/
/************************* Handedness Info *************************/
/*******************************************************************/
int	GetInt(int y, char *msg, int min, int max);

static char *BriggsQuest[] = {
	"1.  To write a letter legibly",
	"2.  To throw a ball to hit a target",
	"3.  To play a game requiring the use of a racket",
	"4.  At the top of a broom to sweep dust from the floor",
	"5.  At the top of a shovel to move sand",
	"6.  To hold a match when striking it",
	"7.  To hold scissors when cutting paper",
	"8.  To hold thread to guide through the eye of a needle",
	"9.  To deal playing cards",
	"10. To hammer a nail into wood",
	"11. To hold a toothbrush while cleaning teeth",
	"12. To unscrew the lid of a jar",
};

int	GetName(void)
{
	int	ok=0;
	int	len;
	int	comma=-1;
	FILE *id;
	char	buf[15];
	int	n;

	_clearscreen(0);
	printf("Neuropsychological Testing");

	while(!ok) {
		BlankLine(2);
		printf("What is your full name? [last, first]:  ");
		gets(user_name);
		len=strlen(user_name);
		for (n=0;n<len;++n) {
			if (!(isalnum(user_name[n]) || user_name[n] == ',' || user_name[n] == ' ' || user_name[n] == '-'))
				break;
			if (user_name[n] == '-')
				user_name[n] = '_';
			if ((user_name[n] == ',' || user_name[n] == ' ') && comma==-1) {
				comma = n;
			}
		}
		if (len > 0 && n == len)
			ok=1;
	}
	if (!(verbose & VERBOSE_GET_HANDEDNESS))
		return 0;

	if (comma != -1) {
		user_name[comma] = '\0';
		datafile=strdup(user_name);
		user_name[comma] = ',';
	}
	else
		datafile=strdup(user_name);

	/** Does file exist? If so, may not need to re-enter handedness info **/
	for (n=0,len=strlen(datafile);n<len && n < 8;++n) {
		buf[n]=datafile[n];
	}
	buf[n++]='.';
	buf[n++]='d';
	buf[n++]='a';
	buf[n++]='t';
	buf[n]='\0';

	if ((id = fopen(buf,"r")) != (FILE *) 0) {
		fclose(id);
		if (GetBool(3,"Have you entered handedness data before","ny")) {
			return 1;
		}
	}
	return 0;
}

void	GetHandednessInfo(void)
{
	int	n;
	int	line=5;

	age = GetInt(line++, "Age", 0, -1);
	years_ed = GetInt(line++, "Years of Education", 0, -1);
	sex = GetBool(line++,"Gender", "mf");
	familyH[0] = GetBool(line++,"Is your Father left handed","ny");
	familyH[1] = GetBool(line++,"Is your Mother left handed","ny");
	familyH[2] = GetInt(line++,"How many Brothers do you have",0,-1);
	if (familyH[2] > 0)
		familyH[3] = GetInt(line++,"  How many of them are left handed", 0, familyH[2]);
	else
		familyH[3] = 0;
	familyH[4] = GetInt(line++,"How many Sisters do you have",0,-1);
	if (familyH[4]> 0)
		familyH[5] = GetInt(line++,"  How many of them are left handed", 0, familyH[4]);
	else
		familyH[5] = 0;

	_clearscreen(0);
	puts("**On the remaining 12 questions, please indicate hand preference:**");
	puts("  0 = Always Left");
	puts("  1 = Usually Left");
	puts("  2 = No Preference");
	puts("  3 = Usually Right");
	puts("  4 = Always Right");

	for (n=0;n<12;++n) {
		indivH[n] = GetInt(n+8,BriggsQuest[n],0,4);
	}
	entered_handedness = 1;
}

void	PrintHandedness(FILE *id)
{
	int	n;
	int	score;

	if (!entered_handedness)
		return;

	entered_handedness = 0;	// reset in case multiple runs for a given trial

	fprintf(id,"Sex: %i\n", sex);
	fprintf(id,"Age: %i\n", age);
	fprintf(id,"Years_ed: %i\n", years_ed);
	if (familyH[0] || familyH[1] || familyH[3] || familyH[5])
		score=1;
	else
		score=0;
	fprintf(id,"Familial Handedness: %i\n", score);
	for (n=0,score=0;n<12;++n) {
		score += indivH[n];
	}
	fprintf(id,"Briggs Handedness Score: %i\n", score);

	fprintf(id,"  Family data:");
	for (n=0;n<6;++n)
		fprintf(id," %i", familyH[n]);

	fprintf(id,"\n  Briggs data:");
	for (n=0;n<12;++n)
		fprintf(id," %i", indivH[n]);
	fprintf(id,"\n\n");
}

int	GetBool(int y, char *msg, char *choices)
{
	int	ok=0;
	int	val;
	char	buf[40];

	while(!ok) {
		BlankLine(y);
		printf("%s? (%c/%c): ", msg, choices[0], choices[1]);

		if (!gets(buf))
			continue;
		if (strlen(buf) == 0)
			continue;
		
		if (*buf == choices[0]) {
			val = 0;
			ok=1;
		}
		if (*buf == choices[1]) {
			val=1;
			ok=1;
		}
	}
	return val;
}

int	GetInt(int y, char *msg, int min, int max)
{
	int	ok=0;
	int	len;
	int	n;
	int	val;
	char	buf[40];

	while(!ok) {
		BlankLine(y);
		if (min < max) 
			printf("%s? (%i-%i): ", msg, min, max);
		if (max < min) 
			printf("%s? (>=%i): ", msg, min);
		if (min == max) 
			printf("%s?: ", msg);

		if (!gets(buf))
			continue;
		len=strlen(buf);
		for (n=0;n<len;++n) {
			if (!isdigit(buf[n]))
				break;
		}
		if (n < len || len == 0)
			continue;

		val = atoi(buf);
		
		if (min < max && val >= min && val <= max)
			ok=1;
		if (max < min && val >= min)
			ok=1;
		if (max == min)
			ok=1;
	}
	return val;
}

void BlankLine(int y)
{
	union		REGS	reg;

	CurMov(1,y);

	reg.h.ah = 9;
	reg.h.al = ' ';
	reg.h.bl = 0x07;
	reg.h.bh = 0;
	reg.x.cx = 80;
	int86(0x10,&reg,&reg);

	CurMov(1,y);
}

/********************************************************************/
/********************** Here is test stuff **************************/
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

void	CalibrateSaturation(void)
{
	/** Draw several rectangles, so can control color saturation for each **/
	int	n;
	int	current=FIRST_COLOR;
	int	key;
	char	r,g,b;
	long	lcolor;
	int	change_color=1;
	
	num_stimuli=4;
	num_choices_per_category=2;
	num_categories=5;
	use_ega=0;	// so that don't need to double buffer

	SCREENmode(MODE_GRAPHICS);

	_settextcolor(15);	// WHITE

	for (n=0;n<num_stimuli;++n)
		draw(n,0,n,0,0,0,0);

	Long2RGB(_palette[current],&r,&g,&b);
	RGBshow_vals(current, r, g, b);
	_settextposition(3,1);
	_outtext("left/right arrows change current");
	_settextposition(4,1);
	_outtext("RGB up/down with keypad");
	_settextposition(5,1);
	_outtext("GREY+/- change step size");
	_settextposition(6,1);
	_outtext("ESCAPE to save values");


	while (1) {
		key = GetAKey();
		switch(key) {
			case K_ESCAPE:
				SCREENmode(MODE_TEXT);
				printf("Palette values\n");
				for (n=0;n<(num_stimuli+2);++n) 
					printf("color %2i:  %8lxH\n", n, _palette[n]);
				return;
				break;
			case K_LEFTARROW:
				--current;  if (current < 0) current = 0;
				change_color=1;
				break;
			case K_RIGHTARROW:
				++current;
				if (current >= (num_stimuli+1)) current = num_stimuli+1;
				change_color=1;
				break;
			case K_UPARROW:
				g += unit;
				if (g > MAX_COLOR_VAL) g = MAX_COLOR_VAL;
				break;
			case K_DOWNARROW:
				g -= unit;
				if (g < 0) g = 0;
				break;
			case K_PAGEUP:
				b += unit;
				if (b > MAX_COLOR_VAL) b = MAX_COLOR_VAL;
				break;
			case K_PAGEDN:
				b -= unit;
				if (b < 0) b = 0;
				break;
			case K_HOME:
				r += unit;
				if (r > MAX_COLOR_VAL) r = MAX_COLOR_VAL;
				break;
			case K_END:
				r -= unit;
				if (r < 0) r = 0;
				break;
			case K_GREY_PLUS:
				++unit;
				if (unit > 16) unit = 16;
				break;
			case K_GREY_MINUS:
				--unit;
				if (unit < 1) unit = 1;
				break;
		}
		if (change_color) {
			change_color = 0;
			Long2RGB(_palette[current],&r,&g,&b);
		}

		RGBshow_vals(current, r, g, b);

		lcolor = RGB2Long(r,g,b);
		_palette[current] = lcolor;
		_remapallpalette(_palette);

	}
}

long	RGB2Long(char r, char g, char b)
{
	long lcolor;
	lcolor = (((long) b & (long) MAX_COLOR_VAL) << 16) |
			(((long) g & (long) MAX_COLOR_VAL) << 8) |
			((long) r & (long) MAX_COLOR_VAL);
	return lcolor;
}
void	Long2RGB(long lcolor, char *r, char *g, char *b)
{
	*r = (char) (lcolor & (long) MAX_COLOR_VAL);
	*g = (char) ((lcolor >> 8) & (long) MAX_COLOR_VAL);
	*b = (char) ((lcolor >> 16) & (long) MAX_COLOR_VAL);
}

void	RGBshow_vals(int current, char r, char g, char b)
{
	char	buf[40];
	sprintf(buf,"[#%i] (%2x,%2x,%2x) (unit=%i)",
		current, (int) r, (int) g, (int) b, unit);
	_settextposition(1,1);
	_outtext(buf);
}

int	autocreate_stimuli(void)
{
#define	MAX_AUTOCREATE_TRIES	100

	int	n,i;
	int	here;
	pint	parr;
	pint	pmatch;
	int	opt;
	int	count;
	pint	difflist;
	pint	matches;
	int	ok;

	srand((unsigned)time(NULL));

	if (autocreate == AUTOCREATE_COUNTERBALANCE) {
		if (verbose) printf("Calculating Counterbalancing for Test...\n");
		for (n=0;n<MAX_AUTOCREATE_TRIES;++n) {
			if (CounterBalance() && TestCounterBalancePcts()) {
				ok = 1;
				break;
			}
			else {
//				if (verbose) printf("Auto-counterbalancing not good enough: re-trying\n");
				ok = 0;
			}
		}
		if (n == MAX_AUTOCREATE_TRIES) {
//			printf("Auto-counterbalancing failed after %i tries.  Please try again later\n", MAX_AUTOCREATE_TRIES);
//			return 0;
			return autocreate_stimuli();
		}
		return ok;
	}

	difflist=calloc(MAX_MATCHES,sizeof(int));
	matches=calloc(num_stimuli,sizeof(int));
	if (!difflist || !matches)
		return 0;

	for (count=0;count<num_trials;++count) {
		here=count*num_stimuli;
		pmatch=&_matches[here];
		matches[0]=pmatch[0]=-1;	// marker that not used

		/** By how many paramters each choice will differ from fixspot? **/
		for (n=0;n<MAX_MATCHES;++n)
			difflist[n]=0;

		for (n=1;n<num_stimuli;++n) {
			while(1) {
				opt=(rand()%MAX_MATCHES);
				if (difflist[opt] == 0)
					break;
			}
			difflist[opt]=1;
			matches[n]=pmatch[n]=opt;
		}

		/** Randomly assign fields **/
		for (n=0;n<num_categories;++n) {
			parr=GetOption(here,n);
			if(use_many_colors) {
				switch(n){
					case 0: parr[0]=(rand()%MAX_COLORS); break;
					case 5: parr[0]=(rand()%MAX_PATTERNS); break;
					default: parr[0]=(rand()%num_choices_per_category); break;
				}
			}
			else {
				parr[0]=(rand()%num_choices_per_category);
			}

			/** Make sure matches match, and non-matches are randomly scattered **/
			for (i=1;i<num_stimuli;++i) {
				if (matches[i]>0 && rand()%2) {
					parr[i]=parr[0];
					--matches[i];
				}
				else {
					while (1) {
					 	if(use_many_colors) {
							switch(n){
								case 0: parr[i]=(rand()%MAX_COLORS); break;
								case 5: parr[i]=(rand()%MAX_PATTERNS); break;
								default: parr[i]=(rand()%num_choices_per_category); break;
							}
						}
						else {
							parr[i]=(rand()%num_choices_per_category);
						}
						if (parr[0]!=parr[i])
							break;
					}
				}
			}
		}
		/** Add in missing matches **/
		for (n=1;n<num_stimuli;++n) {
			while(matches[n] > 0) {
				parr=GetOption(here,rand()%num_categories);
				if (parr[n]!=parr[0]) {
					parr[n]=parr[0];
					--matches[n];
				}
			}
		}
	}
	if (difflist)
		free(difflist);
	if (matches)
		free(matches);

	calc_matches();	// to confirm and generate _most_similar[]
	return 1;
}

/****************************************************/
/** Routines for generating counterbalanced design **/
/****************************************************/
static	long	NextStim_cycle;	// so know how deeply searched
static	int	premature_abort;

int	CounterBalance(void)
{
	/** For now, only code this for binary choices! **/
	int	cb,n,i,k;
	int	needed_items;
	int	avg_num_items;
	int	ok=0;
	int	cycle;

	needed_items=num_trials*num_stimuli;
	avg_num_items=needed_items/max_item_num;
	if ((avg_num_items * max_item_num) < needed_items)
		++avg_num_items;


	/** Generate all possible combinations: This algorithm valid for binary **/
	for (cb=0,k=0;cb<times_counterbalanced;++cb) {
		for (n=0;n<max_same_categories;++n) {
			for (i=n+1;i<=max_same_categories;++i) {
				/* Counterbalance: ensure TOP and BOTTOM positions equally
					likely to be most similar */
				_sameness[k++]=-1;	// place holder
				_sameness[k++]=n;
				_sameness[k++]=i;
				_sameness[k++]=-1;	// place holder
				_sameness[k++]=i;
				_sameness[k++]=n;
			}
		}
	}

	for (cycle=0;cycle<5;++cycle) {
		/** Now recursively try to fill _sameness array **/
		for (n=0;n<max_item_num;++n)
			_items[n]=avg_num_items;	// so can test vs 0

		NextStim_cycle = 0L;
		premature_abort = 0;
		if (NextStim(0,(num_trials*num_stimuli)-1) && !premature_abort) {
//			if (verbose) printf("Counterbalancing took %li cycles\n",NextStim_cycle);
			ok=1;
			break;
		}
		else {
//			if (verbose) printf("Retrying after %li cycles\n", NextStim_cycle);
			ok=0;
		}
	}

	for (i=0;i<num_stimuli*num_trials;++i) {
		set_arrays(_Orig_stim[i],i);
	}

	calc_matches();	// to set _match_min/max arrays

	return ok;
}


void	set_arrays(int item_num, int index)
{
	int	arr[10];	// so == 0

	RevItemVal(item_num,arr);
	_color[index] = arr[0];
	_shape[index] = arr[1];
	_num[index] = arr[2];
	_size[index] = arr[3];
	_filled[index] = arr[4];
	_pattern[index] = arr[5];
}

void	RandomizeStim(void)
{
	/** randomly fill _matches and other arrays **/
	int	here,index;
	int	n,i,k;

	memcpy(_stim_copy,_Orig_stim,sizeof(int)*num_trials*num_stimuli);

	if (rand_type == RAND_DO_NOT) 
		return;

	if (rand_type == RAND_FULLY_MIXED) {
		for (n=0,index=0;n<num_trials;++n,index+=num_stimuli) {
			do {
				here = num_stimuli * (rand() % num_trials);
			} while (_stim_copy[here] < 0);

			for (i=0;i<num_stimuli;++i) {
				set_arrays(_stim_copy[here+i],index+i);
				_stim_copy[here+i]=-1;	// so not accessed in future
			}
		}
	}
	if (rand_type == RAND_WITHIN_BLOCK) {
		int	min;
		for (k=0,index=0;k<(num_trials/trials_per_block);++k) {
			min = k*trials_per_block;
			for (n=0;n<trials_per_block;++n,index+=num_stimuli) {
				do {
					here = num_stimuli * (min + (rand() % trials_per_block));
				} while (_stim_copy[here] < 0);

				for (i=0;i<num_stimuli;++i) {
					set_arrays(_stim_copy[here+i],index+i);
					_stim_copy[here+i]=-1;	// so not accessed in future
				}
			}
		}
	}

	calc_matches();	// to set _match_min/max arrays
}

int	NextStim(int index,int stim_left)
{
	/** This function is called recursively: depth first algorithm **/

	int	n;
	int	delta;
	int	i;
	int	offset;
	int	base;
	int	ok;

	if (premature_abort)
		return 1;

	offset=index%num_stimuli;
	base=index-offset;

	for (n=0;n<max_item_num;++n) {
		if (premature_abort)
			return 1;

		if (++NextStim_cycle > MAX_NEXT_STIM) {
			premature_abort = 1;
			return 1;
		}

		if (n==0) {
			i=(int) (rand() % max_item_num);	// randomly pick first #
			delta=((rand() % 2) ? 1 : -1);	// randomly pick direction
		}
		if (_items[i]) {
			if (offset) {
				if (CheckSameness(i,_Orig_stim[base]) == _sameness[index])
					ok=1;
				else
					ok=0;
			}
			else {
				/** Then the original target, so just pick randomly **/
				ok=1;
			}

			if (ok) {
				--_items[i];		// mark item as used; try to assign additional ones
				_Orig_stim[index]=i;
				if (stim_left) {
					if (NextStim(index+1,stim_left-1)) {
						return 1;
					}
					else {
						++_items[i];	// unmark item: couldn't assign additional ones
					}
				}
				else {
					return 1;
				}
			}
		}

		i += delta;
		if (i >= max_item_num)
			i = 0;
		if (i < 0)
			i = max_item_num-1;
	}
	return 0;
}

int	CheckSameness(int item1, int item2)
{
	int	val1,val2;
	int	n;
	int	count=0;
	int	mul=max_item_num/num_choices_per_category;

	for (n=num_categories-1;n>0;--n,mul/=num_choices_per_category) {
		val1=item1/mul;
		val2=item2/mul;
		if (val1 == val2)
			++count;
		item1-=(val1 * mul);
		item2-=(val2 * mul);
	}
	if (item1 == item2)
		++count;

	return count;
}

void	RevItemVal(int item, int *array)
{
	int	n;
	int	mul=max_item_num/num_choices_per_category;

	for (n=num_categories-1;n>0;--n,mul/=num_choices_per_category) {
		array[n]=item/mul;
		item-=(array[n] * mul);
	}
	array[0]=item;
}


/** This function controls all stimulus presentation and data recording **/

int	run_trial(int demo)
{
	int	key;
	char	c;
	int	n;
	int	here;
	int	opt;
	long	times[3];
	int	deltaX,deltaY,buttons;
	char	buf[80];
	int	tdemo[5][5];
	int	tstim[5];
	int	no_response=0;

	here=trial_num*num_stimuli;

	/** Clear screen and wait a period of time **/
	if (use_ega)
		page_flip(1);
	else
		_clearscreen(0);


	if (demo) {
		/* create sample stimuli */
		int	i,j;
		int	ok;

		tstim[0] = rand() % max_item_num;
		tstim[1] = rand() % max_item_num;

		for (i=2;i<num_stimuli;) {
			ok=1;
			tstim[i] = rand() % max_item_num;
			for (j=1;j<i;++j) {
				if (CheckSameness(tstim[j],tstim[0]) == CheckSameness(tstim[i],tstim[0])) {
					ok=0;
					break;
				}
			}
			if (ok)
				++i; 	// no similarity matches with other stimuli
		}
		for (i=0;i<num_stimuli;++i){
			RevItemVal(tstim[i],tdemo[i]);
		}
	}

	/** Wait until end of inter-trial interval, then draw stimulus **/

	if (!use_ega)
		while (MS_TIMERcheck(0) > 0L)
			;

	if (choice_type != CHOOSE_PREF_CONTROL) {
		if (demo) {
			draw(0,tdemo[0][0],tdemo[0][1],tdemo[0][2],tdemo[0][3],tdemo[0][4],0);
			if (demo==1) {
				_settextcolor(15);	// WHITE
				_settextposition(3,1);
				_outtext("First look at this card ->");
			}
		}
		else
			draw(0,_shape[here],_color[here],_num[here],_size[here],_filled[here],_pattern[here]);
	}

	if (use_ega) {
		while (MS_TIMERcheck(0) > 0L)
			;
		page_flip(0);
	}

	times[0]=TIMERms_counter;

	/** Wait inter-stimulus interval, then draw choice cards **/
	if (choice_type != CHOOSE_PREF_CONTROL) {
		MS_TIMERset(1,ms_btwn_stim);
	}
	else {
		MS_TIMERset(1,0);	// no first item for CHOICE_PREF
	}

	if (!use_ega)
		while(MS_TIMERcheck(1) > 0L)
			;

	if (demo) {
		if (use_ega && choice_type != CHOOSE_PREF_CONTROL) 
			draw(0,tdemo[0][0],tdemo[0][1],tdemo[0][2],tdemo[0][3],tdemo[0][4],0);
		for (n=1;n<num_stimuli;++n)
			draw(n,tdemo[n][0],tdemo[n][1],tdemo[n][2],tdemo[n][3],tdemo[n][4],0);
		if (demo==1) {
			sprintf(buf,demo_msg,(choice_type==CHOOSE_PREF_CONTROL)?"":"then ",demo_msg0);
			_settextposition(10,(40-strlen(buf)/2));
			_outtext(buf);
			sprintf(buf,demo_top_select,demo_top_select0);
			_settextposition(12,1);
			_outtext(buf);
			sprintf(buf,demo_bottom_select,demo_bottom_select0);
			_settextposition(22,1);
			_outtext(buf);
		}
	}
	else {
		if (use_ega && choice_type != CHOOSE_PREF_CONTROL)
			n=0;
		else
			n=1;
		for (here+=n;n<num_stimuli;++n,++here)
			draw(n,_shape[here],_color[here],_num[here],_size[here],
				_filled[here],_pattern[here]);
	}

	if (use_ega) {
		while(MS_TIMERcheck(1) > 0L)
			;
		page_flip(0);
	}

	times[1]=TIMERms_counter;
	MS_TIMERset(2,total_ms_on);

	/** Clear keyboard buffer so only post-drawing responses registered **/
	while(KeyPressed())
		GetAKey();

	switch(input_type) {
		case INPUT_MOUSE_MVT:
		case INPUT_MOUSE_BUT:
			mouseMoved(&deltaX,&deltaY);	// to re-zero the mouse
			break;
		case INPUT_JOY_MVT:
		case INPUT_JOY_BUT:
			JoystickMoved(&deltaX, &deltaY);	// to re-zero the joystick
			break;
	}


	/** Wait for reaction **/
	while(1) {
		if (demo == 1) {
			static int F1_init=0;
			if (!F1_init) {	// give 7 seconds of extra time
				MS_TIMERset(3,7000);
				F1_init = 1;
			}
			if (MS_TIMERcheck(3) == 0L) {
				// if no response by then, tell what needs to be done
				char *msg = "Please make a choice to show that you understand";
				_settextcolor(13);	// LIGHTMAGENTA
				_settextposition(15,(40-strlen(msg)/2));
				_outtext(msg);
				if (use_ega)
					page_flip(0);
				MS_TIMERset(3,2000);
				while(MS_TIMERcheck(3) > 0L)
					;
				_settextcolor(7);	// NORMAL TEXT
				F1_init=0;	// in case want to see demo again
				return 0;	// so know to try this again.
			}
		}
		else if (total_ms_on && MS_TIMERcheck(2) == 0) { // exceeded max allowed time
			no_response=1;
			goto setup_next_trial;
		}
				
		if (KeyPressed()) {
			/** Allow early abort, but wait appropriate amount of time **/
			key=GetAKey();
			if (key == K_ESCAPE) {
				while(MS_TIMERcheck(2) > 0L)
					;
				MS_TIMERset(0,ms_btwn_trials);
				return -1;
			}
			if (key == K_F1 && demo == 2) {
				while(run_trial(1)==0)	// show graphical help
					;
				return run_trial(2);
			}
		}
		else {
			key=0;
		}
		switch(input_type) {
			case INPUT_NUM_KEYS:
				if (!key)
					break;
				c=(char) key;
				if (c < '1' || c > ('1'+(num_stimuli-2)))
					goto error_key;
				else {
					if (demo==1)
						return 1;
					opt=(int) (c-'1');
					_choice[trial_num]=opt+1;	// so records window # selected
					goto setup_next_trial;
				}
				break;
			case INPUT_MOUSE_MVT:
				/** Only allows for binary choices! **/
				if (mouseMoved(&deltaX,&deltaY)) {
					if (demo==1)
						return 1;
					if (deltaY > MIN_MOUSE_MVT)
						_choice[trial_num]=1;	// top choice
					else if (deltaY < -MIN_MOUSE_MVT)
						_choice[trial_num]=2;	// bottom choice
					else
						goto error_key;
					goto setup_next_trial;
				}
				break;
			case INPUT_MOUSE_BUT:
				if (mousePressed(&buttons)) {
					opt=(buttons & (0xFF >> (9-num_stimuli)));
					if (opt > 0 && opt < num_stimuli) {
						if (demo==1)
							return 1;
						_choice[trial_num]=opt;
						goto setup_next_trial;
					}
					goto error_key;
				}
				break;
			case INPUT_JOY_MVT:
				if (JoystickMoved(&deltaX, &deltaY)) {
					if (demo==1)
						return 1;
					if (deltaY > MIN_JOY_MVT)
						_choice[trial_num]=1;	// top choice
					else if (deltaY < -MIN_JOY_MVT)
						_choice[trial_num]=2;	// bottom choice
					else
						goto error_key;
					goto setup_next_trial;
				}
				break;
			case INPUT_JOY_BUT:
				if (JoystickPressed(&buttons)) {
					opt=(buttons & (0xFF >> (9-num_stimuli)));
					if (opt > 0 && opt < num_stimuli) {
						if (demo==1)
							return 1;
						_choice[trial_num]=opt;
						goto setup_next_trial;
					}
					goto error_key;
				}
				break;
			case INPUT_USER_KEYS:
				if (!key)
					break;
				for (n=0;n<num_stimuli-1;++n) {
					if (key == _user_keys[n]) {
						if (demo==1)
							return 1;
						_choice[trial_num]=n+1;
						goto setup_next_trial;
					}
				}
				goto error_key;
				break;
			case INPUT_ARROW_KEYS:
				if (!key)
					break;
				if (!(key == K_UPARROW || key == K_DOWNARROW))
					goto error_key;
				else {
					opt = (key == K_UPARROW) ? 1 : 2;
					if (demo==1)
						return 1;
					_choice[trial_num]=opt;	// so records window # selected
					goto setup_next_trial;
				}
				break;
			case INPUT_PLUS_ENTER:
				if (!key)
					break;
				if (!(key == K_GREY_PLUS || key == K_RETURN))
					goto error_key;
				else {
					opt = (key == K_GREY_PLUS) ? 1 : 2;
					if (demo==1)
						return 1;
					_choice[trial_num]=opt;
					goto setup_next_trial;
				}
				break;
		}
		continue;

error_key:
		if (beep_on_error_key)
			printf("\a");
	}

setup_next_trial:
	times[2]=TIMERms_counter;	// once response is registered
	_time[trial_num]=(times[2]-times[1]);

	while(MS_TIMERcheck(2) > 0L)
		;
	MS_TIMERset(0,ms_btwn_trials);

	/** Show which was most similar **/
	if (mark_correct) {
		if (use_ega) {
			if (demo) {
				int	n,max,val,max_id;
				if (choice_type != CHOOSE_PREF_CONTROL) 
					draw(0,tdemo[0][0],tdemo[0][1],tdemo[0][2],tdemo[0][3],tdemo[0][4],0);
				for (n=1;n<num_stimuli;++n)
					draw(n,tdemo[n][0],tdemo[n][1],tdemo[n][2],tdemo[n][3],tdemo[n][4],0);

				for (n=1,max=0,max_id=1;n<num_stimuli;++n) {
					val = CheckSameness(tstim[n],tstim[0]);
					if (val > max) {
						max = val;
						max_id = n;
					}
				}
				draw(max_id,0,5,0,1,0,0);
			}
			else {
				for (n=0,here=trial_num*num_stimuli;n<num_stimuli;++n,++here)
					draw(n,_shape[here],_color[here],_num[here],_size[here],
						_filled[here],_pattern[here]);
				draw(_most_similar[trial_num],0,5,0,1,0,0);
			}
			page_flip(0);
		}
		else {
			draw(_most_similar[trial_num],0,5,0,1,0,0);
		}
		MS_TIMERset(3,50);
		while(MS_TIMERcheck(3) > 0L)
			;
	}
	return (!no_response);
}

pint GetOption(int here, int n)
{
	static	int	ibuf[10];
	pint	parr;

	switch(n) {
		case 0: parr=&_color[here]; break;
		case 1: parr=&_shape[here]; break;
		case 2: parr=&_num[here]; break;
		case 3: parr=&_size[here]; break;
		case 4: parr=&_filled[here]; break;
		case 5: parr=&_pattern[here]; break;
		default: parr=ibuf; break;
	}
	return parr;
}

int init_arrays(void)
{
	int	total;
	int	n;

	switch(input_type) {
		case INPUT_NUM_KEYS:
		case INPUT_ARROW_KEYS:
		case INPUT_PLUS_ENTER:
			break;
		case INPUT_MOUSE_MVT:
		case INPUT_MOUSE_BUT:
			if (!mouseInit()) {
				if (verbose) {
					printf("No mouse driver present\n");
					return 0;
				}
			}
			break;
		case INPUT_JOY_MVT:
		case INPUT_JOY_BUT:
			if (!JoystickInit()) {
				printf("No joystick present\n");
				return 0;
			}
			break;
		case INPUT_USER_KEYS:
			_user_keys = calloc(num_stimuli-1,sizeof(int));
			if (!_user_keys)
				goto out_of_memory;
			for (n=0;n<num_stimuli-1;++n) {
				while(KeyPressed())
					GetAKey();
				printf("Press the key which will be used for stimulus %i->",(n+1));
				_user_keys[n] = GetAKey();
				printf("\n");
			}
			break;
	}

	_time=calloc(num_trials,sizeof(long));
	_choice=calloc(num_trials,sizeof(int));
	_most_similar=calloc(num_trials,sizeof(int));
	_match_min=calloc(num_trials,sizeof(int));
	_match_max=calloc(num_trials,sizeof(int));

	total=num_trials*num_stimuli;

	_shape=calloc(total,sizeof(int));
	_color=calloc(total,sizeof(int));
	_num=calloc(total,sizeof(int));
	_size=calloc(total,sizeof(int));
	_filled=calloc(total,sizeof(int));
	_pattern=calloc(total,sizeof(int));
	_matches=calloc(total,sizeof(int));

	for (n=0,max_item_num=1;n<num_categories;++n)
		max_item_num *= num_choices_per_category;

	_items=calloc(max_item_num,sizeof(int));
	_sameness=calloc(total,sizeof(int));
	_Orig_stim=calloc(total,sizeof(int));
	_stim_copy=calloc(total,sizeof(int));


	if (_sameness && _Orig_stim && _stim_copy && _items &&
		_shape && _color && _num && _size && _filled && _pattern && _matches &&
		_time && _choice && _most_similar && _match_min && _match_max) 
		;
	else
		goto out_of_memory;

	return 1;

out_of_memory:
	if (verbose) printf("Insufficient memory for arrays\n");
	cleanup();
	return 0;
}

/********************************************/
/*********** READ/WRITE DATA ****************/
/********************************************/
pchar global_msg="#Trials #Stimuli #Categories #Choices #MaxSame ms_btwn_stim ms_btwn_trials total_ms_on\n";
pchar global_str="%7i %8i %11i %8i %8i %12i %14i %11i\n";
pchar	color_msg="BckgColor BaseColor 1st_color 2nd_color 3rd_color 4th_color 5th_color\n";
pchar	color_str="%8lxH %8lxH %8lxH %8lxH %8lxH %8lxH %8lxH\n";
pchar stm_msg=" Trial#  Stim#  Color  Shape    Num   Size Filled Pattrn Match Item\n";
pchar stm_str="%7i %6i %6i %6i %6i %6i %6i %6i %5i %4i\n";
pchar dat_msg=" Trial#  Stim#  Color  Shape    Num   Size Filled Pattrn Match Item Min Max Time\n";
pchar dat_str="%7i %6i %6i %6i %6i %6i %6i %6i %5i %4i %3i %3i %4li\n";
pchar fdat_str="%7s %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n";
pchar idat_str="%7s %6i %6i %6i %6i %6i %6i %6i\n";

int	read_stimuli(char *filename)
{
	FILE	*id;
	int	n,i;
	int	here;
	int	inada;
	int	line=0;
	char	buf[BUF_LEN+1];
	pchar	err_msg;

	if (verbose) printf("Reading stimuli from \"%s\"...\n",filename);

	if ((id=fopen(filename,"r+")) == (FILE *) 0) {
		puts("File does not exist");
		return 0;
	}
	for (n=0;n<5;++n) {
		if (!fgets(buf,BUF_LEN,id)) 
			goto end_of_file;
		++line;
	}

	if (strlen(global_msg) != strlen(buf)) {
		err_msg=global_msg;
		goto invalid_file_format;
	}

	if (!fgets(buf,BUF_LEN,id))
		goto end_of_file;
	++line;

	if (sscanf(buf,global_str,&num_trials,&num_stimuli,&num_categories,
		&num_choices_per_category,&max_same_categories,
		&ms_btwn_stim,&ms_btwn_trials,&total_ms_on) != 8)
		goto unexpected_args;

	validate_args();
	if (!init_arrays()) {
		fclose(id);
		return 0;
	}

	for (n=0;n<2;++n) {
		if (!fgets(buf,BUF_LEN,id))
			goto end_of_file;
		++line;
	}
	if (strlen(color_msg) != strlen(buf)) {
		err_msg=color_msg;
		goto invalid_file_format;
	}
	if (!fgets(buf,BUF_LEN,id))
		goto end_of_file;
	++line;

	if (get_colors_from == COLORS_FROM_STM_FILE) {
		if (sscanf(buf,color_str,&_palette[0],&_palette[1],&_palette[2],
			&_palette[3],&_palette[4],&_palette[5],&_palette[6]) != 7)
			goto unexpected_args;
	}
	/* Else use the ones parsed in from .cfg file, if at all */

	for (n=0;n<2;++n) {
		if (!fgets(buf,BUF_LEN,id))
			goto end_of_file;
		++line;
	}
	if (strlen(stm_msg) != strlen(buf)) {
		err_msg=stm_msg;
		goto invalid_file_format;
	}

	for (n=0,here=0;n<num_trials;++n) {
		for (i=0;i<num_stimuli;++i,++here) {
			if (!fgets(buf,BUF_LEN,id))
				goto end_of_file;
			++line;
			if (sscanf(buf,stm_str,&inada,&inada,
				&_color[here],&_shape[here],
				&_num[here],&_size[here],&_filled[here],&_pattern[here],
				&_matches[here],&inada) != 10)
				goto unexpected_args;

			BOUNDED(_shape[here],0,num_choices_per_category-1);
			BOUNDED(_num[here],0,num_choices_per_category-1);
			BOUNDED(_size[here],0,num_choices_per_category-1);
			BOUNDED(_filled[here],0,num_choices_per_category-1);
			if(use_many_colors) {
				BOUNDED(_color[here],0,MAX_COLORS-1);
				BOUNDED(_pattern[here],0,MAX_PATTERNS-1);
			}
			else {
				BOUNDED(_color[here],0,num_choices_per_category-1);
				BOUNDED(_pattern[here],0,num_choices_per_category-1);
			}
			_Orig_stim[here] = item_val(here);	// for randomization purposes
		}
	}
	fclose(id);

	calc_matches();

	return 1;

unexpected_args:
	if (verbose) printf("Unexpected args on line %i\n", line);
	fclose(id);
	return 0;

end_of_file:
	puts("File ended too early");
	fclose(id);
	return 0;

invalid_file_format:
	printf("Invalid file format.  Expects as line %i:\n",line);
	puts(err_msg);
	fclose(id);
	return 0;
}

int	read_data(FILE *id,int line)
{
	static	init=0;
	int	n,i;
	int	here;
	int	len;
	int	inada;
	char	Lbuf[BUF_LEN+1];

	if (!init) {
		init=1;
		if (!fgets(Lbuf,BUF_LEN,id))
			goto error;
	}

	for (n=0;n<23;++n,++line) {
		switch(n) {
			case 0: strcpy(user_name,&Lbuf[6]); len=strlen(user_name); user_name[len-1]='\0'; break;
			case 1: strcpy(date_str,&Lbuf[6]); len=strlen(date_str); date_str[len-1]='\0'; break;
			case 2: strcpy(time_str,&Lbuf[6]); len=strlen(time_str); time_str[len-1]='\0'; break;
			case 3: break;
			case 4:
				if (strncmp(Lbuf,"Sex:",4)!=0) {
					n= 12;
					played_before=1;
					goto Parse_Exp_Type;
				}
				if (sscanf(Lbuf,"Sex: %i\n", &sex)!=1)
					goto error;
				played_before=0;
				break;
			case 5: if (sscanf(Lbuf,"Age: %i\n", &age)!=1) goto error; break;
			case 6: if (sscanf(Lbuf,"Years_ed: %i\n", &years_ed)!=1) goto error; break;
			case 7: break;
			case 8: break;
			case 9: if (sscanf(Lbuf,"  Family data: %i %i %i %i %i %i\n",
				&familyH[0],&familyH[1],&familyH[2],&familyH[3],&familyH[4],&familyH[5])!=6)
					goto error;
				break;
			case 10: if (sscanf(Lbuf,"  Briggs data: %i %i %i %i %i %i %i %i %i %i %i %i\n",
				&indivH[0],&indivH[1],&indivH[2],&indivH[3],&indivH[4],&indivH[5],
				&indivH[6],&indivH[7],&indivH[8],&indivH[9],&indivH[10],&indivH[11])!=12)
					goto error;
				break;
			case 11: break;
Parse_Exp_Type:
			case 12: if (sscanf(Lbuf,"Experiment Type: %s",exp_type_str)!=1) goto error; break;
			case 13: strcpy(selfeval_str,&Lbuf[11]); len=strlen(selfeval_str); selfeval_str[len-1]='\0'; break;
			case 14: break;
			case 15: break;
			case 16: if (sscanf(Lbuf,global_str,
				&num_trials, &num_stimuli, &num_categories,&num_choices_per_category,
				&max_same_categories,&ms_btwn_stim,&ms_btwn_trials,&total_ms_on) != 8)
					goto error;
				break;
			case 17: break;
			case 18: break;
			case 19: if (sscanf(Lbuf,color_str,&_palette[0],&_palette[1],&_palette[2],
				&_palette[3],&_palette[4],&_palette[5],&_palette[6]) != 7)
					goto error;
				break;
			case 20: break;
			case 21: break;	// **STIMULI**
			case 22:	if (strcmp(Lbuf,stm_msg)!=0) goto error; break;
		}
		if (!fgets(Lbuf,BUF_LEN,id))
			goto error;
	}

	validate_args();
	if (!init_arrays())
		goto error;

	/** Read STIMULI **/
	for (n=0,here=0;n<num_trials;++n) {
		for (i=0;i<num_stimuli;++i,++here) {
			++line;
			if (sscanf(Lbuf,stm_str,&inada,&inada,
				&_color[here],&_shape[here],
				&_num[here],&_size[here],&_filled[here],&_pattern[here],
				&_matches[here],&inada) != 10)
				goto error;
			if (!fgets(Lbuf,BUF_LEN,id))
				goto error;
		}
	}

	/** Read RESPONSE DATA **/
	for (n=0;n<2;++n,++line)
		if (!fgets(Lbuf,BUF_LEN,id))
			goto error;

	if (strcmp(Lbuf,dat_msg)!=0)
		goto error;

	for (n=0;n<num_trials;++n,++line) {
		if (!fgets(Lbuf,BUF_LEN,id))
			goto error;
	 	if (sscanf(Lbuf,dat_str,
			&inada,&_choice[n],
			&inada,&inada,&inada,&inada,&inada,&inada,&inada,	// item descipt
			&inada,	// item_number
			&_match_min[n],
			&_match_max[n],
			&_time[n]) != 13)
		 		goto error;
	}

	/** Read and discard previous analysis, up to start of appended file **/
	while(1) {
		if (!fgets(Lbuf,BUF_LEN,id))
			return -1;	// since end of file
		if (strncmp(Lbuf,"Name:",5)==0)
			break;
		++line;
	}
	return line;	// so can print error messages in appended file

error:
	printf("Unexpected args on line %i\n", line);
	return -1;
}

void	calc_matches(void)
{
	int	n,i,k;
	int	here;
	int	num_similar;
	int	num_matches;
	int	max,min;
	int	errs=0;

	/** Calc number of matches in case disagrees with what was entered **/
	/** Also calc max/min num matches for each given trial **/
	for (n=0,here=0;n<num_trials;++n) {
		num_similar=0;
		_matches[here]=-1;	// to indicate that the reference position

		for (i=1,++here,max=0,min=num_categories;i<num_stimuli;++i,++here) {
			num_matches=0;
			for (k=0;k<num_categories;++k) {
				if (*GetOption(here,k)==*GetOption(here-i,k))
					++num_matches;
			}
			if (_matches[here] != num_matches) {
//				if (verbose) printf("Num_matches for trial %i says %i.  Should be %i.  Setting it so.\n",
//					n+1,_matches[here],num_matches);
//				++errs;
				_matches[here]=num_matches;
			}
			if (num_matches > num_similar) {
				num_similar=num_matches;
				_most_similar[n]=i;
			}
			if (num_matches<min)
				min=num_matches;
			if (num_matches>max)
				max=num_matches;
		}
		_match_min[n]=min;
		_match_max[n]=max;
	}
	if (errs && verbose) {
		printf("There were %i errors corrected...", errs);
		GetAKey();
	}
}

void store_stimuli(void)
{
	FILE	*id;
	int	n,i;
	int	here;
	char	buf[50];

	/** First write the stimuli for possible re-use **/
	sprintf(buf,"%s.stm",datafile);

	if ((id=fopen(buf,"a")) == (FILE *) 0) {
		if (verbose) printf("unable to open \"%s\" to store stimuli\n",buf);
		return;
	}
	fprintf(id,"Name: %s\n", user_name);
	fprintf(id,"Date: %s\n", _strdate(buf));
	fprintf(id,"Time: %s\n\n", _strtime(buf));

	fprintf(id,global_msg);
	fprintf(id,global_str,num_trials,num_stimuli,num_categories,
		num_choices_per_category,max_same_categories,
		ms_btwn_stim,ms_btwn_trials,total_ms_on);

	fprintf(id,"\n");
	fprintf(id,color_msg);
	fprintf(id,color_str,_palette[0],_palette[1],_palette[2],_palette[3],
		_palette[4],_palette[5],_palette[6]);

	fprintf(id,"\n");
	fprintf(id,stm_msg);

	for (n=0,here=0;n<num_trials;++n) {
		for (i=0;i<num_stimuli;++i,++here) {
			fprintf(id,stm_str,(n+1),i,_color[here],_shape[here],_num[here],
				_size[here],_filled[here],_pattern[here],_matches[here],
				item_val(here));
		}
	}
	PrintCounterBalancePcts(id);

	fprintf(id,"\n\n");
	fclose(id);
}

int	item_val(int here)
{
	int	n;
	int	val=0;
	int	mul_factor;

	for (n=0,mul_factor=1;n<num_categories;++n,mul_factor*=num_choices_per_category) 
		val += (*GetOption(here,n) * mul_factor);

	return val;
}

void store_stimuli_as_txt(void)
{
	FILE	*id;
	int	n,i;
	int	here;
	char	buf[50];
	static	pchar	msg="Trial   Color  Shape   Num  Size  Fill  Pattern\n";
	static	pchar	str="%5s  %6s  %6s  %3i %5.2f %4s  %7s\n";
	static	pchar	pattern_list[] = {"Diags","Arrow","Water","Cross","Vert","Horiz"};
	static	pchar	shape_list[] = {"Square","Circle","Diamnd","Pentgn","Hexagn"};
	static	pchar	color_list[] = {"Red","Blue","Green","Yellow","Cyan","Magnta"};
	static	pchar filled_list[] = {"1.0", "0.8", "0.6", "0.4"};

	/** First write the stimuli for possible re-use **/
	sprintf(buf,"%s.txt",datafile);

	if ((id=fopen(buf,"a")) == (FILE *) 0) {
		if (verbose) printf("unable to open \"%s\" to store stimuli\n",buf);
		return;
	}
	fprintf(id,"Name: %s\n", user_name);
	fprintf(id,"Date: %s\n", _strdate(buf));
	fprintf(id,"Time: %s\n\n", _strtime(buf));
	fprintf(id,msg);

	for (n=0,here=0;n<num_trials;++n) {
		for (i=0;i<num_stimuli;++i,++here) {
			sprintf(buf,"%5i",(n+1));
			fprintf(id,str,
				(i==0) ? buf : "",
				color_list[_color[here]],
				shape_list[_shape[here]],
				_num[here]+1,
				(_size[here] == 0) ? 1. : (1. - ((float) _size[here] / (float) num_choices_per_category)),
				filled_list[_filled[here]],
//				(_filled[here] == 0) ? 1. : (1. - ((float) _filled[here] / (float) num_choices_per_category)),
				(num_categories >= 6) ? pattern_list[_pattern[here]] : "N/A");
		}
	}
	fprintf(id,"\n");
	fclose(id);
}

void	analyze(char *dest, int time_now)
{
	FILE	*id;
	int	n,i;
	int	here;
	int	count;
	char	buf[20];

	/** Now store the results for further analysis **/
	if ((id=fopen(dest,"a")) == (FILE *) 0) {
		if (verbose) printf("unable to open \"%s\" to store results\n",dest);
		return;
	}

	fprintf(id,"Name: %s\n", user_name);
	fprintf(id,"Date: %s\n", (time_now) ? _strdate(buf) : date_str);
	fprintf(id,"Time: %s\n\n", (time_now) ? _strtime(buf) : time_str);

	if (!played_before)
		PrintHandedness(id);

	fprintf(id,"Experiment Type: %s\n", (time_now) ? ExpType[choice_type-1] : exp_type_str);
	fprintf(id,"Self-Eval: %s\n\n",selfeval_str);
	fprintf(id,global_msg);
	fprintf(id,global_str,num_trials,num_stimuli,num_categories,
		num_choices_per_category,max_same_categories,
		ms_btwn_stim,ms_btwn_trials,total_ms_on);

	fprintf(id,"\n");
	fprintf(id,color_msg);
	fprintf(id,color_str,_palette[0],_palette[1],_palette[2],_palette[3],
		_palette[4],_palette[5],_palette[6]);

	fprintf(id,"\n**STIMULI**\n");
	fprintf(id,stm_msg);
	for (n=0,here=0;n<num_trials;++n) {
		for (i=0;i<num_stimuli;++i,++here) {
			fprintf(id,stm_str,(n+1),i,_color[here],_shape[here],_num[here],
				_size[here],_filled[here],_pattern[here],_matches[here],
				item_val(here));
		}
	}

	fprintf(id,"\n**RESPONSE DATA**\n");
	fprintf(id,dat_msg);
	for (n=0,count=0;n<num_trials;++n,count+=num_stimuli) {
		here=count+_choice[n];
		fprintf(id,dat_str,(n+1),_choice[n],
			_color[here],_shape[here],_num[here],
			_size[here],_filled[here],_pattern[here],_matches[here],
			item_val(here),
			_match_min[n],
			_match_max[n],
			_time[n]);
	}
	fprintf(id,"\n");

	PrintCBTscores(id);
	PrintGraphs(id);

	fprintf(id,"\n\n");	// so space for next trial if needed

	fclose(id);
}

void	PrintCBTscores(FILE *id)
{
	/** Originally, these numbers were flipped relative to the graph
		(so the statistics would have same significance, but opposite sign).
		This has been fixed, so that one can glance down to graph to determine
		the nature of the preference **/

#define	ALL_VALID_TRIALS 	0
#define	EVEN_VALID_TRIALS	1
#define	RAND_HALF_VALIDS	2
#define	VALID_NONMATCHES	3
#define	QUARTER1_TRIALS	4
#define	QUARTER2_TRIALS	5
#define	QUARTER3_TRIALS	6
#define	QUARTER4_TRIALS	7	
#define	MAX_TRIAL_GROUPS	8

	int	cycle;
	float	ratio;
	int	max_possible;
	int	min_possible;
	int	total_score;
	int	n,k,count;
 	int	ok_trials;
	long	total_time;
	float	avg_rsp_time;
	char 	*msg;
	int	chose[10];
	int	out_of[10];
	float	Zval[10];
	int	*parr;


	for (cycle=0;cycle<MAX_TRIAL_GROUPS;++cycle) {
		switch(cycle) {
			case ALL_VALID_TRIALS:
				msg="All valid trials";
				break;
			case EVEN_VALID_TRIALS:
				continue;
				msg="Even valid trials";
				break;
			case RAND_HALF_VALIDS:
				continue;
				msg="Random half valid trials";
				break;
			case VALID_NONMATCHES:
				msg="Valid non-match trials";
				if (max_same_categories < num_categories)
					return;	// since ALL_VALID_TRIALS and this will be same
				break;
			case QUARTER1_TRIALS:
				continue;
				msg="Valid trials from Quarter1";
				break;
			case QUARTER2_TRIALS:
				continue;
				msg="Valid trials from Quarter2";
				break;
			case QUARTER3_TRIALS:
				continue;
				msg="Valid trials from Quarter3";
				break;
			case QUARTER4_TRIALS:
				continue;
				msg="Valid trials from Quarter4";
				break;
		}

		ok_trials=0;
		max_possible=0;
		min_possible=0;
		total_score=0;
		total_time=0;

		for (n=0;n<10;++n) {
			chose[n] =0;
			out_of[n]=0;	// how many binary tests for each type
		}

		for (n=0,count=0;n<num_trials;++n,count+=num_stimuli) {
			if (!_choice[n])
				continue;	// since no response made (so invalid trial)

			switch (cycle) {
				case ALL_VALID_TRIALS:
					break;
				case EVEN_VALID_TRIALS:
					if (2*(n/2)!=n)
						continue;	// ignore odd trials (check stability of statistics in subset)
					break;
				case RAND_HALF_VALIDS:
					if (rand()%2)
						continue;	// randomly choose about half of the trials
					break;
				case VALID_NONMATCHES:
					if (_match_max[n]>=max_same_categories)
						continue;	// exact match, so exclude
					break;
				case QUARTER1_TRIALS:
					if (n >= num_trials/4)
						continue;
					break;
				case QUARTER2_TRIALS:
					if (n < num_trials/4 || n >= num_trials/2)
						continue;
					break;
				case QUARTER3_TRIALS:
					if (n < num_trials/2 || n >= 3*(num_trials/4))
						continue;
					break;
				case QUARTER4_TRIALS:
					if (n < 3*(num_trials/4))
						continue;
					break;
			}

			++ok_trials;

			/** Analyze preference for similarity of choice to initial sample **/
			total_score+=_matches[count+_choice[n]];
			max_possible+=_match_max[n];
			min_possible+=_match_min[n];
			total_time+=_time[n];

			/** Analyze preference for certain object type **/
			for (k=0;k<num_categories;++k) {
				parr=GetOption(count,k);
				if (parr[1] == parr[2])
					continue;	// only consider binary choices

				++out_of[k];
				chose[k]+=parr[_choice[n]];
			}
			chose[6]+=(_choice[n]-1);	// so know position preference
			++out_of[6];
		}

		if (cycle == 0) {
			PrintCounterBalancePcts(id);
		}
		
		/** Print results of analyses **/

		if (ok_trials > 0) {
			for (n=0;n<10;++n)
				Zval[n] = Zapprox((out_of[n]-chose[n]),out_of[n]);
			ratio=(float)(total_score-min_possible)/(float)(max_possible-min_possible)*(float)100.;
			avg_rsp_time=(float) ((double) total_time / (float)1000. / (double) ok_trials);

			fprintf(id,"\n%s: %i\n",msg,ok_trials);
			fprintf(id,idat_str,"Chose",(out_of[6]-chose[6]),
				(out_of[0]-chose[0]),(out_of[1]-chose[1]),(out_of[2]-chose[2]),
				(out_of[3]-chose[3]),(out_of[4]-chose[4]),(out_of[5]-chose[5]));
			fprintf(id,idat_str,"Out of",out_of[6],out_of[0],out_of[1],out_of[2],
				out_of[3],out_of[4],out_of[5]);
			fprintf(id,fdat_str,"BinaryZ",Zval[6],Zval[0],Zval[1],Zval[2],Zval[3],Zval[4],Zval[5]);
			fprintf(id,"Score: %i (min=%i,max=%i) = %5.2f%% (avg_rsp_time_sec: %5.3f)\n",
				total_score,min_possible,max_possible,ratio,avg_rsp_time);
		}
	}
}

void	PrintGraphs(FILE *id)
{
	int	n,count,chosen,which;
	char	*msg,*high,*low;
	pint	parr;
	int	cycle,block_size;
	int	*sum_high,*sum_low;
	int	*cbt,*cbt_max,*cbt_min;
	long	*rsp_time;
	int	total_sum_high, total_sum_low;
	char	buf[60];
	char	stat_msg[15];

	sum_high=calloc(analysis_blocks,sizeof(int));
	sum_low=calloc(analysis_blocks,sizeof(int));
	cbt=calloc(analysis_blocks,sizeof(int));
	cbt_max=calloc(analysis_blocks,sizeof(int));
	cbt_min=calloc(analysis_blocks,sizeof(int));
	rsp_time=calloc(analysis_blocks,sizeof(long));

	if (!(sum_high && sum_low && cbt && cbt_max && cbt_min && rsp_time)) {
		if (verbose) printf("No room for sum arrays");
		return;
	}


	block_size=num_trials/analysis_blocks;
	sprintf(stat_msg,"|%%-%is",block_size);

	for (which=0;which<7;++which) {
		switch(which) {
			case 0: msg="SIMILAR"; high="alike"; low="not"; break;
			case 1: msg="POSITION";	high="top"; low="bottom"; break;
			case 2: msg="COLOR"; high="red"; low="blue"; parr=_color; break;
			case 3: msg="SHAPE"; high="square"; low="circle"; parr=_shape; break;
			case 4: msg="NUMBER"; high="one"; low="two"; parr=_num; break;
			case 5: msg="SIZE"; high="large"; low="small"; parr=_size; break;
			case 6: msg="FILLED"; high="solid"; low="open"; parr=_filled; break;
		}

		for (n=0;n<analysis_blocks;++n)
			sum_high[n]=sum_low[n]=0;

		fprintf(id,"\n%8s",high);
		for (n=0,count=0,cycle=-1;n<num_trials;++n,count+=num_stimuli) {
			if (!(n%block_size)){
				++cycle;
				fprintf(id,"|");
			}

			if (!_choice[n]) {
				fprintf(id,"X");
				continue;
			}
			chosen=count+_choice[n];
			switch(which) {
				case 0:
					if (_matches[chosen] == _match_max[n]) {
						++sum_high[cycle];
						fprintf(id,"*");
					}
					else
						fprintf(id," ");

					cbt[cycle]+=_matches[chosen];
					cbt_max[cycle]+=_match_max[n];
					cbt_min[cycle]+=_match_min[n];
					rsp_time[cycle]+=_time[n];
					break;
				case 1:
					if (_choice[n] == 1) {
						++sum_high[cycle];
						fprintf(id,"*");
					}
					else
						fprintf(id," ");
					break;
				default:
					if (parr[chosen] == 1 || parr[count+1]==parr[count+2])
						fprintf(id," ");
					else {
						fprintf(id,"*");
						++sum_high[cycle];
					}
					break;
			}
		}
		for (n=0,total_sum_high=0;n<analysis_blocks;++n)
			total_sum_high+=sum_high[n];
		fprintf(id,"| %3i\n%8s",total_sum_high,msg);

		for (n=0;n<num_trials;++n) {
			if (!(n%block_size))
				fprintf(id,"|");
			fprintf(id,"%i",((n+1)%10));
		}

		fprintf(id,"|\n%8s",low);
		for (n=0,count=0,cycle=-1;n<num_trials;++n,count+=num_stimuli) {
			if (!(n%block_size)) {
				fprintf(id,"|");
				++cycle;
			}

			if (!_choice[n]) {
				fprintf(id,"X");
				continue;
			}
			chosen=count+_choice[n];
			switch(which) {
				case 0:
					if (_matches[chosen] == _match_min[n]) {
						++sum_low[cycle];
						fprintf(id,"*");
					}
					else
						fprintf(id," ");
					break;
				case 1:
					if (_choice[n] == 2) {
						++sum_low[cycle];
						fprintf(id,"*");
					}
					else
						fprintf(id," ");
					break;
				default:
					if (parr[chosen] == 0 || parr[count+1]==parr[count+2])
						fprintf(id," ");
					else {
						++sum_low[cycle];
						fprintf(id,"*");
					}
			}
		}
		for (n=0,total_sum_low=0;n<analysis_blocks;++n)
			total_sum_low+=sum_low[n];
		fprintf(id,"| %3i\n",total_sum_low);

		/** Print statistics for sub-blocks **/
		fprintf(id,"%8s",""); 
		for (n=0;n<analysis_blocks;++n) {
			sprintf(buf,"(%i/%i):%5.2f",sum_high[n],sum_low[n],Zapprox(sum_high[n],(sum_high[n]+sum_low[n])));
			fprintf(id,stat_msg,buf);
		}
		fprintf(id,"| %5.2f\n",Zapprox(total_sum_high,(total_sum_high+total_sum_low)));

		/** Print CBT scores and reaction times for each analysis block **/
		if (which==0) {
			int	Tcbt=0,Tcbt_min=0,Tcbt_max=0;
			long	Trsp_time=0L;
			int	num_valid;

			fprintf(id,"%8s","");
			for (n=0;n<analysis_blocks;++n) {
				sprintf(buf,"CBT=%i (%i,%i)",cbt[n],cbt_min[n],cbt_max[n]);
				fprintf(id,stat_msg,buf);

				Tcbt+=cbt[n];
				Tcbt_min+=cbt_min[n];
				Tcbt_max+=cbt_max[n];
				Trsp_time+=rsp_time[n];
			}
			fprintf(id,"|CBT=%i (%i,%i)\n%8s",Tcbt,Tcbt_min,Tcbt_max,"");
			
			for (n=0;n<analysis_blocks;++n) {
				num_valid=(sum_high[n]+sum_low[n]);
				if (num_valid > 0) {
					sprintf(buf,"=%.1f%% (%lims)",
						(100. * (float) (cbt[n]-cbt_min[n]) / (float) (cbt_max[n]-cbt_min[n])),
						rsp_time[n]/num_valid);
					fprintf(id,stat_msg,buf);
				}
				else {
					fprintf(id,stat_msg,"no trials");
				}
			}
			num_valid=total_sum_low+total_sum_high;
			if (num_valid > 0) {
				fprintf(id,"|=%.1f%% (%lims)\n",
					(100. * (float) (Tcbt-Tcbt_min) / (float) (Tcbt_max-Tcbt_min)),
					Trsp_time/(long)num_valid);
			}
			else {
				fprintf(id,"|no trials\n");
			}
		}
	}
	if (sum_high)
		free(sum_high);
	if (sum_low)
		free(sum_low);
}

void CalcCounterBalancePcts(int low, int high)
{
	int	n,k,j;
	int	here;
	int	*parr;
	int	num_trials_used=0;

	for (n=0;n<10;++n) {
		sumOpts[n] = sumInit[n] = 0;
		avgInit[n] = avgOpts[n] = avgAll[n] = 0.;
		zInit[n] = zOpts[n] = zAll[n] = 0.;
	}

	for (n=0;n<num_trials;++n) {
		here=n*num_stimuli;
		if (_match_min[n] < low || _match_max[n] > high)
			continue;	// avoid ones not in requested range

		++num_trials_used;

		for (k=0;k<num_categories;++k) {
			parr=GetOption(here,k);
			sumInit[k]+=parr[0];
			for (j=1;j<num_stimuli;++j) {
				sumOpts[k]+=parr[j];
			}
		}
	}
	for (n=0;n<num_categories;++n) {
		avgInit[n] = (float) sumInit[n] / (float) num_trials_used;
		avgOpts[n] = (float) sumOpts[n] / (float) (num_trials_used * (num_stimuli-1));
		avgAll[n]  = (float) (sumInit[n]+sumOpts[n]) / (float) (num_trials_used * num_stimuli);
		zInit[n]	= Zapprox(sumInit[n],num_trials_used);
		zOpts[n] = Zapprox(sumOpts[n],(num_trials_used * (num_stimuli-1)));
		zAll[n]  = Zapprox((sumInit[n]+sumOpts[n]),(num_trials_used * num_stimuli));
	}
}

int	TestCounterBalancePcts(void)
{
#define		MAX_OK_ZSCORE	1.0

	int	n,j;

	if (num_stimuli > 3 || num_choices_per_category > 2)
		return 1;

	for (j=0;j<2;++j) {
		if (j==1 && max_same_categories < num_categories)
			break;

		CalcCounterBalancePcts(0,max_same_categories-j);

		for (n=0;n<num_categories;++n) {
			if (zInit[n] > MAX_OK_ZSCORE || zInit[n] < -MAX_OK_ZSCORE) {
//				printf("zInit[%i] was too high (%g)\n", n, zInit[n]);
				return 0;
			}
			if (zOpts[n] > MAX_OK_ZSCORE || zOpts[n] < -MAX_OK_ZSCORE) {
//				printf("zOpts[%i] was too high (%g)\n", n, zOpts[n]);
				return 0;
			}
			if (zAll[n] > MAX_OK_ZSCORE || zAll[n] < -MAX_OK_ZSCORE) {
//				printf("zAll[%i] was too high (%g)\n", n, zAll[n]);
				return 0;
			}
		}
	}
	return 1;
}

void	PrintCounterBalancePcts(FILE *id)
{
	int	n;
	char	*msg;

	for (n=0;n<2;++n) {
		if (n==0) 
			msg="all trials";
		else
			msg="for trials without exact matches";
		if (n==1 && max_same_categories < num_categories)
			break;

		fprintf(id,"Quality of CounterBalancing for %s:\n", msg);
		CalcCounterBalancePcts(0,max_same_categories-n);

		fprintf(id,idat_str,"sumInit",0,
			sumInit[0],sumInit[1],sumInit[2],sumInit[3],sumInit[4],sumInit[5]);
		fprintf(id,idat_str,"sumOpts",0,
			sumOpts[0],sumOpts[1],sumOpts[2],sumOpts[3],sumOpts[4],sumOpts[5]);

		fprintf(id,fdat_str,"avgInit",(float) 0,
			avgInit[0],avgInit[1],avgInit[2],avgInit[3],avgInit[4],avgInit[5]);
		fprintf(id,fdat_str,"zInit",(float) 0,zInit[0],zInit[1],zInit[2],zInit[3],zInit[4],zInit[5]);
		fprintf(id,fdat_str,"avgOpts",(float) 0,
			avgOpts[0],avgOpts[1],avgOpts[2],avgOpts[3],avgOpts[4],avgOpts[5]);
		fprintf(id,fdat_str,"zOpts",(float) 0,zOpts[0],zOpts[1],zOpts[2],zOpts[3],zOpts[4],zOpts[5]);
		fprintf(id,fdat_str,"avgAll",(float) 0,
			avgAll[0],avgAll[1],avgAll[2],avgAll[3],avgAll[4],avgAll[5]);
		fprintf(id,fdat_str,"zAll",(float) 0,zAll[0],zAll[1],zAll[2],zAll[3],zAll[4],zAll[5]);
	}
}


float	Zapprox(int chose, int out_of)
{
	double num,denom;

	if (out_of <= 0 || chose < 0)
		return (float) 0;

	num = (double) chose / (double) out_of - .5;
	denom = sqrt(.25 / (double) out_of);

	if (denom == 0.)
		return (float) 0;

	return (float) (num / denom);
}

void	cleanup(void)
{
	if (_time) free(_time);
	if (_choice) free(_choice);
	if (_most_similar) free(_most_similar);
	if (_match_min) free(_match_min);
	if (_match_max) free(_match_max);

	if (_sameness) free(_sameness);
	if (_stim_copy) free(_stim_copy);
	if (_Orig_stim) free(_Orig_stim);
	if (_items) free(_items);

	if (_shape) free(_shape);
	if (_color) free(_color);
	if (_num) free(_num);
	if (_size) free(_size);
	if (_filled) free(_filled);
	if (_pattern) free(_pattern);
	if (_matches) free(_matches);
	if (_user_keys) free(_user_keys);
}


void draw(int position, int shape, int color, int num, int size, int filled,int pattern)
{
	float	xcent;
	float	ycent;
	float	unit_size;	// size of object: can be absolute or relative
	float	Dy0;
	int	col;

	int	n;
	float	fnum;
	float	pDsize;		// distance between centers of objects
	float	fillpct;
	float	fillsize;
	int	fillcolor;
	int	pct;

	++num;	// since base 0
	fnum=(float)num;
	pDsize=Dsize/fnum;

	xcent=Dx0+Dsize/(float)2.;	// will always center in X
	Dy0=Dysep + ((float) position * (Dysep+Dsize));

	if (position)
		Dy0 += YSEP_INC * Dysep;	// so have extra space between 0 and targets.

	/** Outline box in which things will be drawn **/
	set_pattern(0);
	VGA_rect(Dx0+Dsize/(float)2.,Dy0+Dsize/(float)2.,Dsize,Dsize,1,BASE_COLOR);

	switch(filled) {
		case 0: fillpct = 1; break;
		case 1: fillpct = .8; break;
		case 2: fillpct = .6; break;
		case 3: fillpct = .4; break;
	}

	col=color+FIRST_COLOR;

	if (size == 0)
		pct=9;
	else
		pct=9-(size*10/num_choices_per_category);

	if (num_categories >= 6)
		set_pattern(pattern+1);

	if (use_absolute_size) {
		unit_size = (Dsize/(float)num_choices_per_category)*((float) pct/(float)10.);
	}
	else {	// relative to number of objects presented
		unit_size = (Dsize*((float) pct/fnum/(float)10.));
	}
	fillsize= unit_size*fillpct;

	fillcolor=BASE_COLOR;

	for (n=0,ycent=Dy0+pDsize/(float)2.; n<num ;++n,ycent+=pDsize) {
		switch(shape) {
			case 0: VGA_rect(xcent,ycent,unit_size,unit_size,fillpct,col); break;
			case 1: VGA_ellipse(xcent,ycent,unit_size,unit_size,fillpct,col); break;
			default: VGA_reg_polygon(2*shape,xcent,ycent,unit_size/2,0,1,col); break;
		}
	}	
}

void	set_pattern(int pattern)
{
	switch(pattern) {
		default: _setfillmask(NULL); break;
		case 1: _setfillmask(diagonal); break;
		case 2: _setfillmask(arrows); break;
		case 3: _setfillmask(waterhash); break;
		case 4: _setfillmask(crosshash); break;
		case 5: _setfillmask(vertical); break;
		case 6: _setfillmask(horizontal); break;
	}
}

void page_flip(int option)
{
	static	int	draw=1,disp=0;
	static	int	init=0;

	if (!init) {
		init=1;
		_setvisualpage(disp);
		_setactivepage(draw);
	}

	switch(option) {
		case -1:
			_setactivepage(1);
			_clearscreen(0);
			_setactivepage(0);
			_clearscreen(0);
			_setvisualpage(0);
			init=0;
			break;
		case 1:
			_clearscreen(0);		// fall through
		case 0:
			_setvisualpage(draw);
			_setactivepage(disp);
			disp^=1;
			draw^=1;
			_clearscreen(0);
			break;
	}
}

/************************ ACCESSORY FUNCTIONS ***********************/
/********************************************************************/
/************************* Here is timer stuff **********************/
/********************************************************************/

#define	MAX_MS_TIMERS	5
typedef	struct {
	int	active;
	long	final;
} MS_TIMER_ENTRY;
static	MS_TIMER_ENTRY MSTimers[MAX_MS_TIMERS] = { 0 };

int	MS_TIMERset(int id, long ms)
{
	if (!use_timer)
		return 1;

	if (id < 0 || id >= MAX_MS_TIMERS || ms < 0L) return 0;
	MSTimers[id].final = TIMERms_counter+ms;	// tracks final value
	MSTimers[id].active = 1;
	return 1;
}

long	MS_TIMERcheck(int id)
{
	long	left;

	if (!use_timer)
		return 0;

	if (id < 0 || id >= MAX_MS_TIMERS || !MSTimers[id].active) return 0L;
	left = MSTimers[id].final - TIMERms_counter;
	if (left < 0L) {
		MSTimers[id].active = 0;
		left = 0L;
	}
	return left;
}


/********************************************************************/
/************************* Here is _vga stuff ***********************/
/********************************************************************/
#define	BASE_ASPECT_RATIO	((float)640./(float)480.)
float	aspect_ratio;

void SCREENmode(int mode)
{
	struct _videoconfig	vc;
	static	int	current_mode = MODE_TEXT;

	if (mode == current_mode)
		return;

	switch (mode) {
		case MODE_GRAPHICS:
			if (use_ega) {
				_setvideomode(_ERESCOLOR);
				page_flip(1);
			}
			else {
				_setvideomode(_VRES16COLOR);
			}
			_remapallpalette(_palette);
			_getvideoconfig(&vc);
			XMAX=(float) vc.numxpixels;
			YMAX=(float) vc.numypixels;
			aspect_ratio=(XMAX/YMAX)/BASE_ASPECT_RATIO;
			if (force_aspect_ratio)
				aspect_ratio=force_aspect_ratio;

			Dsize = YMAX /((float) num_stimuli + YSEP_PCT * ((float) num_stimuli+(float)1.+YSEP_INC));
			Dysep = (YSEP_PCT * Dsize);
			Dx0= (XMAX-Dsize)/(float)2.;
			break;
		case MODE_TEXT:
			if (use_ega)	// to clear and reset double buffering
				page_flip(-1);
			_setvideomode(_DEFAULTMODE);
			break;
	}
	current_mode = mode;
}

#define	PI	(float)3.14159265358989323846
#define	TWO_PI	(float)6.28318530717978647692
#define  R_TO_D	((float)180.0 / PI)

int F2Ieven(float x);

int F2Ieven(float x)
{
	int	val = (int) (floor(x + .5));
	if (2*(val/2) != val)
		--val;	// shift down for even?
	return val;
}

#define	XYCOORD	struct _xycoord

int	VGA_reg_polygon(int num_sides, float cent_x, float cent_y,
	float radius, float degrees, int filled, int color)
{
	int     n,ok;
	double  orient;
	double  step_val;
	XYCOORD *points;

	if (num_sides < 3)
		return 0;

	if ((points=(XYCOORD *) malloc((num_sides+1)*sizeof(XYCOORD)))==(XYCOORD *) 0)
		return 0;

	while (degrees > (float) 360)
		degrees -= (float) 360;
	while (degrees < (float) 0)
		degrees += (float) 360;

	step_val = TWO_PI / (double) num_sides;
	orient = (double) degrees / R_TO_D;

	/* Calculate polygon in dva first */
	for (n=0;n<num_sides;++n) {
		points[n].xcoord = F2Ieven(cent_x + ((float) cos(orient) * radius)*aspect_ratio);
		points[n].ycoord = F2Ieven(cent_y - ((float) sin(orient) * radius));
		orient += step_val;
		if (orient > TWO_PI)
			orient -= TWO_PI;
	}
	points[n] = points[0];

	_setcolor(color);
	ok = _polygon(((filled) ? _GFILLINTERIOR : _GBORDER), points, num_sides);

	if (points)
		free((void *) points);
	return ok;
}

int	VGA_polygon(int num_points, pint Xpoints, pint Ypoints,
	int filled, int color)
{
	int	n,ok;
	XYCOORD *points;

	if (num_points < 3)
		return 0;

	if ((points = (XYCOORD *) malloc((num_points+1) * sizeof(XYCOORD))) == (XYCOORD *) 0)
		return 0;

	for (n=0;n<num_points;++n) {
		points[n].xcoord = (int) ((float) Xpoints[n] * aspect_ratio);
		points[n].ycoord = Ypoints[n];
	}
	points[n] = points[0];

	_setcolor(color);
  	ok = _polygon(((filled) ? _GFILLINTERIOR : _GBORDER), points, num_points);

	if (points)
		free((void *) points);
	return ok;
}

int VGA_rect(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color)
{
	int	ulx,uly,dimx,dimy;
	int	ul2x, ul2y, dim2x,dim2y;

	ulx = F2Ieven(cent_x-size_x/2.*aspect_ratio);
	uly = F2Ieven(cent_y-size_y/2.);
	dimx = F2Ieven(size_x * aspect_ratio);
	dimy = F2Ieven(size_y);

	_setcolor(color);
	_rectangle(_GFILLINTERIOR, ulx, uly, ulx+dimx, uly+dimy);
	if (fill_pct < 1) {
		_setcolor(BASE_COLOR);

		dim2x = F2Ieven((float) dimx * fill_pct);
		dim2y = F2Ieven((float) dimy * fill_pct);
		ul2x = ulx + (dimx - dim2x) / 2;
		ul2y = uly + (dimy - dim2y) / 2;
		_rectangle(_GFILLINTERIOR, ul2x, ul2y, ul2x + dim2x, ul2y + dim2y);
	}
	return 1;
}

int VGA_ellipse(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color)
{
	int	ulx,uly,dimx,dimy;
	int	ul2x, ul2y, dim2x,dim2y;

	ulx = F2Ieven(cent_x-size_x/2.*aspect_ratio);
	uly = F2Ieven(cent_y-size_y/2.);
	dimx = F2Ieven(size_x * aspect_ratio);
	dimy = F2Ieven(size_y);

	_setcolor(color);
	_ellipse(_GFILLINTERIOR, ulx, uly, ulx+dimx, uly+dimy);
	if (fill_pct < 1) {
		_setcolor(BASE_COLOR);

		dim2x = F2Ieven((float) dimx * fill_pct);
		dim2y = F2Ieven((float) dimy * fill_pct);
		ul2x = ulx + (dimx - dim2x) / 2;
		ul2y = uly + (dimy - dim2y) / 2;
		_ellipse(_GFILLINTERIOR, ul2x, ul2y, ul2x + dim2x, ul2y + dim2y);
	}
	return 1;
}

long	VGA_color(int color)
{
	switch(color) {
		default: case BLACK: return _BLACK;
		case BLUE: return _BLUE;
		case GREEN: return _GREEN;
		case CYAN: return _CYAN;
		case RED: return _RED;
		case MAGENTA: return _MAGENTA;
		case BROWN: return _BROWN;
		case LIGHTGRAY: return _GRAY;
		case DARKGRAY: return _GRAY;
		case LIGHTBLUE: return _LIGHTBLUE;
		case LIGHTGREEN: return _LIGHTGREEN;
		case LIGHTCYAN: return _LIGHTCYAN;
		case LIGHTRED: return _LIGHTRED;
		case LIGHTMAGENTA: return _LIGHTMAGENTA;
		case YELLOW: return _YELLOW;
		case WHITE: return _WHITE;
	}
}

void	init_palette(void)
{
	int	n;

	for (n=0;n<16;++n){
		_palette[n]=VGA_color(n);
	}
	_palette[BCKG_COLOR]=0x0B0B0BL;	// diffuse gray
	_palette[BASE_COLOR]=0x151515L;	// light gray
	_palette[FIRST_COLOR]=VGA_color(RED);
	_palette[FIRST_COLOR+1]=VGA_color(BLUE);
	_palette[FIRST_COLOR+2]=VGA_color(GREEN);
	_palette[FIRST_COLOR+3]=VGA_color(YELLOW);
	_palette[FIRST_COLOR+4]=VGA_color(CYAN);
	_palette[FIRST_COLOR+5]=VGA_color(MAGENTA);
}

/****** getch AND _bios_keyboard ARE NOT STANDARDIZED SO: ***********/
/******************** THE _bios_keybrd VERSION **********************/

int KeyPressed(void)
{
	// returns 0 if FALSE, !0 if TRUE
	return kbhit();
}

unsigned	GetAKey(void)
{
	while (!kbhit());			// cycling on _KEYBRD_READY prevents CTRL_BREAK!
	return _bios_keybrd(_KEYBRD_READ);
}

/********************************************/
/******** Joystick related routines *********/
/********************************************/
#define JOYSTICK_PORT (0x201)

static	uint	JoyMidX,JoyMidY;
static	uint	JoyLastX=0,JoyLastY=0;
static	int	JoyMask;

int	JoystickInit(void)
{
	/** Assumes auto-centering joystick for now **/
	register uchar portval;
	uint limit=MAX_JOY_DELAY;
	uint	x=0,y=0;

	/* start the timers -- data value of write is unimportant */
	outp(JOYSTICK_PORT, 0x00);

	/* do until interesting bits all drop to zero */
	while (limit) {
		portval = inp(JOYSTICK_PORT);
		x+=portval&1;
		portval >>= 1;
		y+=portval&1;
		limit--;
	}
	JoyMidX=x;
	JoyMidY=y;

	JoyMask=0;
	if (!(x == 0 || x == MAX_JOY_DELAY))
		JoyMask &= 0x1;
	if (!(y == 0 || y == MAX_JOY_DELAY))
		JoyMask &= 0x2;

	return JoyMask;
}

int	JoystickMoved(int *deltaX, int *deltaY)
{
	register uchar portval;
	uint limit=MAX_JOY_DELAY;

	/* start the timers -- data value of write is unimportant */
	outp(JOYSTICK_PORT, 0x00);

	JoyLastX=JoyLastY=0;

	/* do until interesting bits all drop to zero */
	while(((portval=inp(JOYSTICK_PORT)) & JoyMask) && limit) {
		JoyLastX += portval & 1;
		portval >>= 1;
		JoyLastY += portval & 1;
		limit--;
	}
	*deltaX = (int) (JoyLastX - JoyMidX);
	*deltaY = (int) (JoyLastY - JoyMidY);

	if (*deltaX || *deltaY)
		return 1;
	else
		return 0;
}

int	JoystickPressed(int *buttons)
{
	register uchar portval;

	outp(JOYSTICK_PORT, 0x00);		// isn't this unnecessary?
	portval=inp(JOYSTICK_PORT);

	*buttons = (int) ((portval >> 4) ^ 0x0F);

	return *buttons;
}

/********************************************/
/***** mouse Functions *******/
/********************************************/
#define	MOUSE_INTERRUPT_NO	0x33
#define	MOUSE_RESET		0x00	// Always do first: AX = 0 if Fail; BX = #buttons 
#define	MOUSE_STATUS	0x03	// CX=xpos; DX=ypos [(0,0) upper left]; BX see below 
#define	MOUSE_MOTION	0x0B	// motion since last call in mickeys (1/200") returns CX,DX (x,y) 
#define	MOUSE_MICKEY_PIXEL_RATIO	0x0F	// default is 8/8.  CX,DX = mickeys per 8 pixels (so 16/8 takes 16 as arg) 

int	mouseInit(void)
{
	union REGS inregs, outregs;

	inregs.x.ax = MOUSE_RESET;
	int86(MOUSE_INTERRUPT_NO, &inregs, &outregs);
	if (!outregs.x.ax)
		return 0;

	/** Set horz and vert mickeys/pixels ratio so doesn't move as fast **/
	inregs.x.ax = MOUSE_MICKEY_PIXEL_RATIO;
	inregs.x.cx = 16;	// default is 8/8, so trying 16/8 to decrease sensitivity 
	inregs.x.dx = 16;
	int86(MOUSE_INTERRUPT_NO, &inregs, &outregs);

	return 1;

}

int	mousePressed(int *buttons)
{
	static	union REGS inregs, outregs;

	inregs.x.ax = MOUSE_STATUS;
	int86(MOUSE_INTERRUPT_NO, &inregs, &outregs);
	*buttons = outregs.x.bx;	// don't truncate the third button

	if (*buttons)
		return 1;
	else
		return 0;
}

int	mouseMoved(int *deltaX, int *deltaY)
{
	/** Returns the number of mickey's since the last call **/

	static	union REGS inregs, outregs;


	inregs.x.ax = MOUSE_STATUS;
	int86(MOUSE_INTERRUPT_NO, &inregs, &outregs);

	inregs.x.ax = MOUSE_MOTION;
	int86(MOUSE_INTERRUPT_NO, &inregs, &outregs);

	if (outregs.x.cx || outregs.x.dx) {
		*deltaX = (int) outregs.x.cx;
		*deltaY = -((int) outregs.x.dx);	// so that up returns a positive Y
		return 1;
	}
	else
		return 0;
}

/********************************************/
/*********** COLOR TEXT OUTPUT **************/
/********************************************/
#define	PAGE	0
#define	CLIP_TO_BOUNDS(x,min,max)	\
	(((x) < (min)) ? (min): ((x) > (max)) ? (max) : (x))

void	GetCurPos(int *x, int *y)
{
	union	REGS	reg;

	if (*x == 0 || *y == 0){
		reg.h.ah = 3;
		reg.h.bh = PAGE;
		int86(0x10,&reg,&reg);

		*x = (*x) ? *x : (int) ++reg.h.dl;	// Bios stores things as (0,0) -> (79,24)
		*y = (*y) ? *y : (int) ++reg.h.dh;	// But mine is (1,1) -> (80,25), so ++
	}
}

void CurMov(int x, int y)
{
	union	REGS	reg;

	reg.h.ah = 2;
	reg.h.bh = PAGE;
	reg.h.dl = (char) CLIP_TO_BOUNDS(x, 1, 80) - 1;
	reg.h.dh = (char) CLIP_TO_BOUNDS(y, 1, 25) - 1;
	int86(0x10,&reg,&reg);
}

void WriteAttrString(char *string, int x, int y, int attributes)
{
	GetCurPos(&x,&y);
	while (*string != '\0' && x <= 80) {
		WriteAttrChar(*(string++), x++, y, attributes);
	}
	CurMov(x,y);	// will clip x to 80 if == 81
}

void WriteAttrChar(char c, int x, int y, int attributes)
{
	union	REGS	reg;

	if (x < 1 || x > 80 || y < 1 || y > 25)
		return;	// If off screen, don't write instead of overlaying
	CurMov(x, y);
	reg.h.ah = 9;
	reg.h.al = c;
	reg.h.bl = (char) attributes;
	reg.h.bh = PAGE;
	reg.x.cx = 1;	// number of times character is to be written
	int86(0x10,&reg,&reg);
}

/********************************************/
/******** INTERRUPT BINDING ROUTINES ********/
/********************************************/
#define CLKFREQ	1193180L
#define CLKRATE 	0x40		
#define CLKCTL		0x43
#define INTPORT1	0x21
#define maskclock(x)		outp(INTPORT1,(x))
#define	uint	unsigned int
#define	ulong	unsigned long
static	int	CLOCKbound=0;

#define	TIMERdos_clock_unit	55
static	int	TIMERdos_clock_counter=TIMERdos_clock_unit;
static	ulong	*DOStimer		= (ulong *) 0x46c;
static	char	*DOStimer_ofl	= (char	*) 0x470;
static	char	*DOSmotor_cnt	= (char	*) 0x440;
static	char	*DOSmotor_stat	= (char	*) 0x43f;


static	void	(interrupt far *OldInt08)();
static	int clkset(unsigned int ips);

void	CLOCKbind(void (interrupt far *int08)(), int rate)
{
	if (CLOCKbound == rate)
		return;

	_disable();
	OldInt08 = _dos_getvect(0x08);
	if (int08)
		_dos_setvect(0x08,int08);
	_enable();
	clkset(rate);	// any difference if can be interrupted?
	CLOCKbound = rate;
}

void	CLOCKunbind()
{
	if (!CLOCKbound)
		return;

	clkset(0);
	_disable();
	_dos_setvect(0x08,OldInt08);
	_enable();
	CLOCKbound = 0;
}

static	int clkset(unsigned int ips)
{
	long 			lfreq;
	unsigned int	freq,
					highb,
					lowb;

	if (!ips) {
		maskclock(1);
		outp(CLKRATE, 0);
		outp(CLKRATE, 0);
		maskclock(0);
		return 0;	
	}
	lfreq = CLKFREQ / (long) ips;
	if (lfreq > (unsigned) 0xFFFF)
		return -1;
	freq = (unsigned int) lfreq;
	if ((unsigned) (CLKFREQ % ips) >= (ips / 2))
		freq += 1;
	lowb = freq % 256;
	highb = freq / 256;
	maskclock(1);
	outp(CLKRATE, lowb);
	outp(CLKRATE, highb);
	maskclock(0);
	return 0;
}

void	interrupt far CLOCKcounter(void)
{
	++TIMERms_counter;

	/** These are the functions normally performed by the clock interrupt:
		update the DOS clock at the appropriate interval, and spin down the
		floppy drive motor after several ticks **/

	if (--TIMERdos_clock_counter <= 0) {
		TIMERdos_clock_counter = TIMERdos_clock_unit;

		if (++(*DOStimer) == 0x001800B0L) {
			*DOStimer=0L;
			*DOStimer_ofl = 1;	// midnight, so rollover
		}
		if (!(--(*DOSmotor_cnt))) {
			*DOSmotor_stat &= 0xf0;
			outp(0x3f2,0x0c);		// turn off motor;
		}
	}

	outp(0x20,0x20);
}


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

	/** DEBUG: print args
	printf("[%i: %i]", index, argc);
	for (n=-1;n<argc;++n)
		printf(" %s", argv[n]);
	printf("\n");
	**/

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

