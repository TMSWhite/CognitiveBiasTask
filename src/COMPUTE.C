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
