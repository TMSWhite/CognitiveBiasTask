#include "cbt.h"


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
		
//		BOUNDED(_pattern[here],0,MAX_PATTERNS-1);

		for (i=1,++here,max=0,min=num_categories;i<num_stimuli;++i,++here) {
			num_matches=0;
			for (k=0;k<num_categories;++k) {
				if (*GetOption(here,k)==*GetOption(here-i,k))
					++num_matches;
			}
			
//			BOUNDED(_pattern[here],0,MAX_PATTERNS-1);

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
	fprintf(id,"Date: %s\n", Date2str());
	fprintf(id,"Time: %s\n\n", Time2str());

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
	fprintf(id,"Date: %s\n", Date2str());
	fprintf(id,"Time: %s\n\n", Time2str());
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

	/** Now store the results for further analysis **/
	if ((id=fopen(dest,"a")) == (FILE *) 0) {
		if (verbose) printf("unable to open \"%s\" to store results\n",dest);
		return;
	}

	fprintf(id,"Name: %s\n", user_name);
	fprintf(id,"Date: %s\n", (time_now) ? Date2str() : date_str);
	fprintf(id,"Time: %s\n\n", (time_now) ? Time2str() : time_str);

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
	MSTimers[id].final = TIMERget_ms()+ms;	// tracks final value
	MSTimers[id].active = 1;
	return 1;
}

long	MS_TIMERcheck(int id)
{
	long	left;

	if (!use_timer)
		return 0;

	if (id < 0 || id >= MAX_MS_TIMERS || !MSTimers[id].active) return 0L;
	left = MSTimers[id].final - TIMERget_ms();
	if (left < 0L) {
		MSTimers[id].active = 0;
		left = 0L;
	}
	return left;
}



