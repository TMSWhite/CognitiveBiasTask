#include "cbt.h"


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

