/************************************************/
/** THESE ARE THE PLATFORM DEPENDENT FUNCTIONS **/
/************************************************/

extern	short	SCREENmaxX, SCREENmaxY;

#define	INPUT_MOUSE		1
#define	INPUT_JOYSTICK	2

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


#define	TEXT_HIGHLIGHTED	0x70
#define	TEXT_BOLD			0x0f
#define	TEXT_NORMAL			0x07


short	GRAPHICSsetup(short on);
short	INPUTsetup(short type);

short	TIMERsetup(short on);
long	TIMERget_ms(void);
void	TIMERzero_clock(void);
char	*Time2str(void);
char	*Date2str(void);
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

void	WriteAttrString(char *msg, short x, short y, short style);
void	Gprint(short x, short y, char *msg, short color);


/*****************************************/
/** END OF PLATFORM DEPENDENT FUNCTIONS **/
/*****************************************/

