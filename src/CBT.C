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

