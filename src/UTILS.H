/************************************************/
/** THESE ARE THE PLATFORM DEPENDENT FUNCTIONS **/
/************************************************/

typedef	unsigned char	uchar;
typedef	unsigned int	uint;
typedef	char	*	pchar;
typedef	int	*	pint;
typedef	long	*	plong;
typedef	float	*	pfloat;


extern	short	SCREENmaxX, SCREENmaxY;
extern	short	SCREENdouble_buffer;
//extern	float	SCREENaspect_ratio;

#define	INPUT_MOUSE		1
#define	INPUT_JOYSTICK	2

/** Using palette of max 16 colors.  Setting to EGA type default **/
#define	BCKG_COLOR	0
#define	BASE_COLOR	1
#define	FIRST_COLOR	2

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


#define	TEXT_HIGHLIGHTED	LIGHTMAGENTA
#define	TEXT_BOLD			YELLOW
#define	TEXT_NORMAL			WHITE

#define	TEXT_LEFT		0
#define	TEXT_RIGHT		1
#define	TEXT_CENTER		2
#define	TEXT_NEXT		3
#define	TEXT_BACKSPACE	4

void	Gprint(char *msg, short y, short xpos, short color);
void BlankLine(int y);
void	Beep(void);


short	GRAPHICSsetup(short on);
short	INPUTsetup(short type);

short	TIMERsetup(short on);
long	TIMERget_ms(void);
void	TIMERzero_clock(void);
char	*Time2str(void);
char	*Date2str(void);

#define	MODE_DOUBLE_BUFFER	2
#define	MODE_GRAPHICS		1
#define	MODE_TEXT			0
#define	MODE_CLEANUP		-1

void	SCREENmode(short graphics);
int		VGA_reg_polygon(int num_sides, float cent_x, float cent_y,float radius, float degrees, int filled, int color);
int		VGA_rect(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color);
int		VGA_ellipse(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color);
void	VGAset_color(int color);
void 	Cls(void);

void	PALETTEsetup(long colors[16]);
void	PALETTEapply(short num, long *colors);
void 	SCREENpage_flip(int option);
void	PATTERNset(int pattern);

#define	EVENT_quit			0
#define	EVENT_other			1
#define	EVENT_keyboard		2
#define	EVENT_mouse			3

#define	EVENT_shift_mask	1
#define	EVENT_ctrl_mask	2
#define	EVENT_alt_mask		4

int	EVENTallow(void);
void	EVENTget_mouse(short *x, short *y, short *buttons);
void	EVENTflush_all(void);
short	KeyPressed(void);
unsigned GetAKey(void);


/*****************************************/
/** END OF PLATFORM DEPENDENT FUNCTIONS **/
/*****************************************/


