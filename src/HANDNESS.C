#include "cbt.h"

/*******************************************************************/
/************************* Handedness Info *************************/
/*******************************************************************/

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
/**
	union		REGS	reg;

	CurMov(1,y);

	reg.h.ah = 9;
	reg.h.al = ' ';
	reg.h.bl = 0x07;
	reg.h.bh = 0;
	reg.x.cx = 80;
	int86(0x10,&reg,&reg);

	CurMov(1,y);
**/
}

void	Cls(void){}



