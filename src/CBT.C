#include "cbt.h"


/***************** START OF CODE ************/

//#define	WHILE_WAITING	EVENTallow()
#define	WHILE_WAITING

static	short	GRAPHICSmode;

void main(int argc, char **argv)
{
	int	cycle;
	
	if (!GRAPHICSsetup(1))
		return;

	PALETTEsetup(_palette);
	
#ifdef	USING_MAC	// since can't pass command line arguments to Macs!
	{
		static	char *argv[] = { "Mac-CBT", "F", "cbt.cfg" };
		int		argc = 2;
#endif

	if (!parse_args(argc,argv))
		goto shutdown;
		
#ifdef	USING_MAC
	}
#endif

	if (use_ega)
		GRAPHICSmode = MODE_DOUBLE_BUFFER;
	else
		GRAPHICSmode = MODE_GRAPHICS;

	SCREENmode(MODE_TEXT);
	
	if (verbose) {
		char *title="CBT - Neuropsychological Testing";
		char *to_start = "Press a key to begin";
		Cls();
		Gprint(title,12,TEXT_CENTER,TEXT_HIGHLIGHTED);
		Gprint(to_start,14,TEXT_CENTER,TEXT_NORMAL);
		GetAKey();
	}

	if (verbose)
		played_before = GetName();

	if (verbose == VERBOSE_GET_HANDEDNESS && !played_before) {
		GetHandednessInfo();
	}

	/* For multi exps, assumes that you want to use exactly the same
		items each time, potentially in different orders.  To generate new
		items, need to run from scratch several times */
		
	if (use_timer) {
		if (!TIMERsetup(1)) {
			if (verbose) {
				printf("Unable to initialize Timer\n");
				goto shutdown;
			}
		}
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
	
	if (use_timer) {
		if (cleanup_delay)
			MS_TIMERset(0,cleanup_delay);
		else
			MS_TIMERset(0,0);
	}
	
shutdown:

	SCREENmode(MODE_TEXT);
	cleanup();
	
	if (verbose) Gprint("Thank you", 12, TEXT_CENTER, TEXT_NORMAL);
	
	GRAPHICSsetup(0);
	
	if (use_timer) {
		while(MS_TIMERcheck(0) > 0L)
			;
		TIMERsetup(0);
	}
}

void	run_trials(void)
{
	char	buf[80];
	int	n;

	if (!show_instructions()) 
		return;	// allows early abort via ESCAPE

	SCREENmode(GRAPHICSmode);

	/* Must reset response arrays each time new exp is run **/
	for (n=0;n<num_trials;++n){
		_choice[n]=0;	// to indicate that nothing selected
		_time[n]=0L;
	}


	for (trial_num=0;trial_num<num_trials;++trial_num) {
		if (run_trial(0)==-1)	// ESCAPE pressed
			break;
	}
	
	if (!immediacy)
		SCREENmode(MODE_TEXT);


	if (ask_awareness_of_patterns && (choice_type == CHOOSE_PREFERENCE || choice_type == CHOOSE_PREF_CONTROL)) {
		if (GetBool(1,"Were you aware of any conscious pattern(s) to your preferences","ny")) {
			Gprint("Please briefly describe them\n->",2,TEXT_CENTER,TEXT_NORMAL);
			Gets(selfeval_str);
		}
	}

	store_stimuli();
	store_stimuli_as_txt();

	sprintf(buf,"%s.dat",datafile);
	analyze(buf,1);
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
			return 0;
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
	printf("CBT program accepts these command line arguments:\n");
	printf("CBT C [optional color codes]  - lets you calibrate the color saturation\n");
	printf("CBT F config_file             - run experiment(s) according to config_file\n");
	printf("CBT R num_blocks source dest  - recomputes statistics on .DAT file\n");

	GetAKey();

	return 0;
}


int	show_instructions(void)
{
	char	buf[80];
	int	i;
	int	ok=0;

	trial_num=0;	// so all demos do not fill response arrays

	if (immediacy == 1) {
		return 1;
	}

	if (verbose) {
		Cls();
		Gprint("INSTRUCTIONS",1,TEXT_CENTER,TEXT_HIGHLIGHTED);
		Gprint("You will see cards that have different designs on them.  The designs",3,TEXT_LEFT,TEXT_NORMAL);
		if (choice_type == CHOOSE_PREF_CONTROL) {
			Gprint("may vary in several respects.  Of the cards shown, choose the one you",4,TEXT_LEFT,TEXT_NORMAL);
			Gprint("like the most.",5,TEXT_LEFT,TEXT_BOLD);
		}
		else {
			Gprint("may vary in several respects.  After looking at the top card,",4,TEXT_LEFT,TEXT_NORMAL);
			sprintf(buf,"%i new cards will appear below it.  Please choose the one card from the",(num_stimuli-1));
			Gprint(buf,5,TEXT_LEFT,TEXT_NORMAL);
		}

		switch(choice_type) {
			case CHOOSE_PREFERENCE:
//				Gprint("Please choose the card from the new set which you ",5,TEXT_LEFT,TEXT_NORMAL);
				Gprint("new set which you ", 6,TEXT_LEFT,TEXT_NORMAL);
				Gprint("like the best.",0,TEXT_NEXT,TEXT_BOLD);
				demo_msg0 = "you like the best";
				break;
			case CHOOSE_SAME:
				Gprint("new cards which is ", 6, TEXT_LEFT,TEXT_NORMAL);
//				Gprint("Please choose the card from the new set which is ",5,TEXT_LEFT,TEXT_NORMAL);
				Gprint("most similar",0,TEXT_NEXT,TEXT_BOLD);
				Gprint(" to the first card.",0,TEXT_NEXT,TEXT_NORMAL);
				demo_msg0 = "MOST SIMILAR to the one above";
				break;
			case CHOOSE_DIFFERENT:
				Gprint("new cards which is ", 6, TEXT_LEFT,TEXT_NORMAL);
//				Gprint("Please choose the card from the new set which is ",5,TEXT_LEFT,TEXT_NORMAL);
				Gprint("most different",0,TEXT_NEXT,TEXT_BOLD);
				Gprint(" from the first card.",0,TEXT_NEXT,TEXT_NORMAL);
				demo_msg0 = "MOST DIFFERENT from the one above";
				break;
			case CHOOSE_PREF_CONTROL:
				demo_msg0 = "you like the best";
				break;
		}
		if (choice_type == CHOOSE_PREFERENCE || choice_type == CHOOSE_PREF_CONTROL) {
			Gprint("There are no \"correct\" or \"incorrect\" responses.  Your choice",8,TEXT_LEFT,TEXT_NORMAL);
			Gprint("is entirely up to you.",9,TEXT_LEFT,TEXT_NORMAL);
		}
		Gprint("Please try to make your choices ",11,TEXT_LEFT,TEXT_NORMAL);
		if (total_ms_on)
			Gprint("before the cards disappear.", 11, TEXT_NEXT, TEXT_NORMAL);
		else
			Gprint("rather quickly.", 11, TEXT_NEXT, TEXT_NORMAL);

		Gprint("When making selection,",14,TEXT_LEFT,TEXT_NORMAL);
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
		Gprint("  select UPPER card by:  ",15,TEXT_LEFT,TEXT_NORMAL);
		Gprint(demo_top_select0,15,TEXT_NEXT,TEXT_NORMAL);
		Gprint("  select LOWER card by:  ",16,TEXT_LEFT,TEXT_NORMAL);
		Gprint(demo_bottom_select0,16,TEXT_NEXT,TEXT_NORMAL);

		Gprint("Press F1 for graphical help", 25, TEXT_LEFT,TEXT_NORMAL);
		Gprint("Press SPACE to start", 25, TEXT_RIGHT,TEXT_BOLD);

		while (!ok) {
			i = GetAKey();
			if (i == K_F1) {
				SCREENmode(GRAPHICSmode);

				while(run_trial(1)==0)
					;
				SCREENmode(MODE_TEXT);
				return show_instructions();
			}
			if (i == K_ESCAPE)
				return 0;

			if ((char) i == ' ') 
				ok = 1;
		}
	}
	else {
		GetAKey();
	}

	SCREENmode(GRAPHICSmode);

	if (!did_warmup && num_warmup_trials > 0) {
		if (verbose_warmup) {
			Gprint("Press SPACE to try a few practice rounds",12,TEXT_CENTER, WHITE);

			if (SCREENdouble_buffer)
				SCREENpage_flip(0);	// so displayed

			while(1) {
				i = GetAKey();
				if (i == K_ESCAPE)
					break;
				if (i == K_F1) {
					while(run_trial(1)==0)	// show graphical help
						;

				}

				if ((char) i == ' ') 
					break;
			}
		}
		/* Should I allow ms_btwn_trials delay between time press space and first appearance? */
		Cls();
		MS_TIMERset(3,ms_btwn_trials);
		while(MS_TIMERcheck(3))
			WHILE_WAITING;	
						
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

		if (verbose_warmup) {
			if (SCREENdouble_buffer)
				SCREENpage_flip(0);	// so displayed
			else
				Cls();
			Gprint("Great!  Now press SPACE to start experiment",12,TEXT_CENTER, WHITE);
			if (SCREENdouble_buffer)
				SCREENpage_flip(0);	// so displayed

			while(1) {
				i = GetAKey();
				if (i == K_ESCAPE)
					return 0;
				if ((char) i == ' ')
					break;
			}
		}
	}
	/* Should I allow ms_btwn_trials delay between time press space and first appearance? */
	Cls();
	MS_TIMERset(3,ms_btwn_trials);
	while(MS_TIMERcheck(3))
		WHILE_WAITING;
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
	char	buf[80];
	
	num_stimuli=4;
	num_choices_per_category=2;
	num_categories=5;
	SCREENdouble_buffer=0;	// so that don't need to double buffer

	SCREENmode(GRAPHICSmode);

	for (n=0;n<num_stimuli;++n)
		draw(n,0,n,0,0,0,0);

	Long2RGB(_palette[current],&r,&g,&b);
	RGBshow_vals(current, r, g, b);
	Gprint("left/right arrows change current",3,TEXT_LEFT, WHITE);
	Gprint("RGB up/down with keypad",4, TEXT_LEFT, WHITE);
	Gprint("GREY+/- change step size",5, TEXT_LEFT,WHITE);
	Gprint("ESCAPE to save values",6, TEXT_LEFT, WHITE);

	while (1) {
		if (EVENTallow() != EVENT_keyboard)
			continue;
			
		key = GetAKey();
		switch(key) {
			case K_ESCAPE:
				SCREENmode(MODE_TEXT);
				Gprint("Palette values",1,TEXT_LEFT,TEXT_NORMAL);
				for (n=0;n<(num_stimuli+2);++n) {
					sprintf(buf,"color %2i:  %8lxH\n", n, _palette[n]);
					Gprint(buf,(n+2),TEXT_LEFT,TEXT_NORMAL);
				}
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
		PALETTEsetup(_palette);
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
	Gprint(buf,1, TEXT_LEFT,WHITE);

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
	int	tdemo[5][10];
	int	tstim[5];
	int	no_response=0;
	int F1_init=0;
	
	here=trial_num*num_stimuli;

	/** Clear screen and wait a period of time **/
	if (SCREENdouble_buffer)
		SCREENpage_flip(1);
	else
		Cls();


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

	if (!SCREENdouble_buffer)
		while (MS_TIMERcheck(0) > 0L)
			WHILE_WAITING;

	if (choice_type != CHOOSE_PREF_CONTROL) {
		if (demo) {
			draw(0,tdemo[0][0],tdemo[0][1],tdemo[0][2],tdemo[0][3],tdemo[0][4],0);
			if (demo==1) {
				Gprint("First look at this card ->",3, TEXT_LEFT, WHITE);
			}
		}
		else
			draw(0,_shape[here],_color[here],_num[here],_size[here],_filled[here],_pattern[here]);
	}

	if (SCREENdouble_buffer) {
		while (MS_TIMERcheck(0) > 0L)
			WHILE_WAITING;
		SCREENpage_flip(0);
	}

	times[0]=TIMERget_ms();

	/** Wait inter-stimulus interval, then draw choice cards **/
	if (choice_type != CHOOSE_PREF_CONTROL) {
		MS_TIMERset(1,ms_btwn_stim);
	}
	else {
		MS_TIMERset(1,0);	// no first item for CHOICE_PREF
	}

	if (!SCREENdouble_buffer)
		while(MS_TIMERcheck(1) > 0L)
			WHILE_WAITING;

	if (demo) {
		if (SCREENdouble_buffer && choice_type != CHOOSE_PREF_CONTROL) 
			draw(0,tdemo[0][0],tdemo[0][1],tdemo[0][2],tdemo[0][3],tdemo[0][4],0);
		for (n=1;n<num_stimuli;++n)
			draw(n,tdemo[n][0],tdemo[n][1],tdemo[n][2],tdemo[n][3],tdemo[n][4],0);
		if (demo==1) {
			sprintf(buf,demo_msg,(choice_type==CHOOSE_PREF_CONTROL)?"":"then ",demo_msg0);
			Gprint( buf,10,TEXT_CENTER,WHITE);
			sprintf(buf,demo_top_select,demo_top_select0);
			Gprint(buf,12, TEXT_LEFT,WHITE);
			sprintf(buf,demo_bottom_select,demo_bottom_select0);
			Gprint(buf,22, TEXT_LEFT,WHITE);
		}
	}
	else {
		if (SCREENdouble_buffer && choice_type != CHOOSE_PREF_CONTROL)
			n=0;
		else
			n=1;
		for (here+=n;n<num_stimuli;++n,++here)
			draw(n,_shape[here],_color[here],_num[here],_size[here],
				_filled[here],_pattern[here]);
	}

	if (SCREENdouble_buffer) {
		while(MS_TIMERcheck(1) > 0L)
			WHILE_WAITING;
		SCREENpage_flip(0);
	}

	times[1]=TIMERget_ms();
	MS_TIMERset(2,total_ms_on);


	EVENTflush_all();

	/** Wait for reaction **/
	while(1) {
		if (demo == 1) {
			if (!F1_init) {	// give 7 seconds of extra time
				MS_TIMERset(3,7000);
				F1_init = 1;
			}
			if (MS_TIMERcheck(3) == 0L) {
				// if no response by then, tell what needs to be done
				char *msg = "Please make a choice to show that you understand";
				Gprint(msg,15,TEXT_CENTER,LIGHTMAGENTA);
				if (SCREENdouble_buffer)
					SCREENpage_flip(0);
				MS_TIMERset(3,2000);
				while(MS_TIMERcheck(3) > 0L)
					WHILE_WAITING;
				TIMERzero_clock();	// do I want to do this here?
				return 0;	// so know to try this again.
			}
		}
		else if (total_ms_on && MS_TIMERcheck(2) == 0) { // exceeded max allowed time
			no_response=1;
			goto setup_next_trial;
		}
				
		if (EVENTallow() == EVENT_keyboard) {
			/** Allow early abort, but wait appropriate amount of time **/
			key=GetAKey();
			if (key == K_ESCAPE) {
				/* Why do I need to wait? 
				while(MS_TIMERcheck(2) > 0L)
					WHILE_WAITING;
				*/
				TIMERzero_clock();
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
/*
			case INPUT_MOUSE_MVT:
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
*/
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
				if (!(key == K_GREY_PLUS || key == K_ENTER))
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
			Beep();
	}

setup_next_trial:
	times[2]=TIMERget_ms();	// once response is registered
	_time[trial_num]=(times[2]-times[1]);

	while(MS_TIMERcheck(2) > 0L)
		WHILE_WAITING;
		
	TIMERzero_clock();		// is this a good place for this?
	MS_TIMERset(0,ms_btwn_trials);

	/** Show which was most similar **/
	if (mark_correct) {
		if (SCREENdouble_buffer) {
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
			SCREENpage_flip(0);
		}
		else {
			draw(_most_similar[trial_num],0,5,0,1,0,0);
		}
		MS_TIMERset(3,50);
		while(MS_TIMERcheck(3) > 0L)
			WHILE_WAITING;
	}
	return (!no_response);
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

	/*	Only needed when using MAC monitor to run DOS program, so never
	SCREENaspect_ratio = (SCREENmaxX/SCREENmaxY)/BASE_ASPECT_RATIO;
	if (force_aspect_ratio)
		SCREENaspect_ratio=force_aspect_ratio;
	*/

	Dsize = SCREENmaxY /((float) num_stimuli + YSEP_PCT * ((float) num_stimuli+(float)1.+YSEP_INC));
	Dysep = (YSEP_PCT * Dsize);
	Dx0= (SCREENmaxX-Dsize)/(float)2.;

	++num;	// since base 0
	fnum=(float)num;
	pDsize=Dsize/fnum;

	xcent=Dx0+Dsize/(float)2.;	// will always center in X
	Dy0=Dysep + ((float) position * (Dysep+Dsize));

	if (position)
		Dy0 += YSEP_INC * Dysep;	// so have extra space between 0 and targets.

	/** Outline box in which things will be drawn **/
	PATTERNset(0);
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
		PATTERNset(pattern+1);

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

