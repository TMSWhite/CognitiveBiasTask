/** Routines for allocating a full screen window **/
#include <stdio.h>	// only for printf
#include <stdlib.h>	// only for malloc
#include <math.h>	// for DRAW functions
#include <string.h>

#include <graph.h>
#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <time.h>

#include "utils.h"

/** Local declarations **/
static	int	MouseInit(void);
static	int	MousePressed(int *buttons);
static	int	MouseMoved(int *deltaX, int *deltaY);
static	int	JoystickInit(void);
static	int	JoystickPressed(int *buttons);
static	int	JoystickMoved(int *deltaX, int *deltaY);
static	void	CLOCKbind(void (interrupt far *int08)(), int rate);
static	void	CLOCKunbind();
static	int clkset(unsigned int ips);
static	void	interrupt far CLOCKcounter(void);

static	void	GetCurPos(int *x, int *y);
static	void CurMov(int x, int y);
static	void WriteAttrString(char *string, int x, int y, int attributes);
static	void WriteAttrChar(char c, int x, int y, int attributes);

static	long	VGA_color(int color);

static	short	SCREENmode_current=0;
static	long	RGBcolors[16];
static	long	TIMERms_counter;

#define	h(n)	((char) 0x##n)
static	char	vertical[8] = { h(CC), h(CC), h(CC), h(CC), h(CC), h(CC), h(CC), h(CC) };
static	char	diagonal[8] = { h(99), h(33), h(66), h(CC), h(99), h(33), h(66), h(CC) };
static	char	horizontal[8] = { h(FF), h(00), h(FF), h(00), h(FF), h(00), h(FF), h(00) };
static	char	spotted[8] = { h(00), h(66), h(66), h(00), h(00), h(66), h(66), h(00) };
static	char	arrows[8] = { h(CC), h(66), h(33), h(99), h(99), h(33), h(66), h(CC) };
static	char	crosshash[8] = { h(22), h(55), h(88), h(55), h(22), h(55), h(88), h(55) };
static	char	waterhash[8] = { h(88), h(55), h(22), h(00), h(88), h(55), h(22), h(00) };

/** End of local declarations **/

short	SCREENmaxX, SCREENmaxY;
short	SCREENdouble_buffer=0;
//float	SCREENaspect_ratio;


short GRAPHICSsetup(short on)
{
	if (!on) {
		SCREENmode(MODE_CLEANUP);
	}
	return 1;
}

short	INPUTsetup(short type)
{
	switch(type) {
		case INPUT_MOUSE:
			if (!MouseInit())
				return 0;
		case INPUT_JOYSTICK:
			if (!JoystickInit())
				return 0;
	}
	return 1;
}

short TIMERsetup(short on)
{
	if (on)
	 	CLOCKbind(CLOCKcounter,1000);
	else
		CLOCKunbind();
	return 1;
}

void SCREENmode(short mode)
{
	struct _videoconfig	vc;

	/*
	if (mode == MODE_TEXT)
		mode = MODE_GRAPHICS;
	*/

	if (mode == SCREENmode_current)
		return;

	switch (mode) {
		case MODE_DOUBLE_BUFFER:
			_setvideomode(_ERESCOLOR);
			SCREENpage_flip(1);			
			_remapallpalette(RGBcolors);
			_getvideoconfig(&vc);
			SCREENmaxX = vc.numxpixels;
			SCREENmaxY = vc.numypixels;
//			SCREENaspect_ratio=(float) 1;
			break;
		case MODE_GRAPHICS:
			_setvideomode(_VRES16COLOR);
			_remapallpalette(RGBcolors);
			_getvideoconfig(&vc);
			SCREENmaxX = vc.numxpixels;
			SCREENmaxY = vc.numypixels;
//			SCREENaspect_ratio=(float) 1;
			break;
		case MODE_TEXT:
			if (SCREENdouble_buffer)	// to clear and reset double buffering
				SCREENpage_flip(-1);
			_setvideomode(_DEFAULTMODE);
			break;
		case MODE_CLEANUP:
			if (SCREENdouble_buffer)	// to clear and reset double buffering
				SCREENpage_flip(-1);
			_setvideomode(_DEFAULTMODE);
			break;
	}
	SCREENmode_current = mode;
}

void Cls(void)
{
	_clearscreen(0);
}


long	TIMERget_ms(void)
{
	return TIMERms_counter;
}

void	TIMERzero_clock(void)
{
	TIMERms_counter=0L;
}

char	*Date2str(void)
{
	static	char	buf[30];

	return _strdate(buf);
}

char	*Time2str(void)
{
	static char	buf[30];

	return _strtime(buf);
}

void	Gprint(char *msg, short y, short xpos, short color)
{
	static	short	cx=0, cy=0;
	short	len;
	
	len = strlen(msg);

	if (y)
		cy = y;
	
	switch(xpos) {
		default:
		case TEXT_LEFT:
			cx = 1;
			break;
		case TEXT_RIGHT:
			cx = 80 - len;
			break;
		case TEXT_CENTER:
			cx = (80 - len) / 2;
			break;
		case TEXT_NEXT:
			break;	// use current cx
		case TEXT_BACKSPACE:
			cx -= len;
			*msg = ' ';
			color = WHITE;	// so that properly erases
			break;
	}
	
	switch(SCREENmode_current) {
		case MODE_DOUBLE_BUFFER:
		case MODE_GRAPHICS:
			_settextcolor(color);
			_settextposition(cy,cx);
			_outtext(msg);
			break;
		default:
		case MODE_TEXT:
			WriteAttrString(msg,cx,cy,color);
			break;
	}
	
	if (xpos == TEXT_BACKSPACE) {
		if (MODE_TEXT)
			CurMov(cx,cy);
		else
			_settextposition(cy,cx);
	}
	else
		cx += len;
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


/********************************************************************/
/************************* Here is DRAW stuff ***********************/
/********************************************************************/


#define	PI	(float)3.14159265358989323846
#define	TWO_PI	(float)6.28318530717978647692
#define  R_TO_D	((float)180.0 / PI)

#define	BASE_COLOR	1	// since can have background and base colors
#define	XYCOORD	struct _xycoord

static int F2Ieven(float x);

int F2Ieven(float x)
{
	int	val = (int) (floor(x + .5));
	if (2*(val/2) != val)
		--val;	// shift down for even?
	return val;
}

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
//		points[n].xcoord = F2Ieven(cent_x + ((float) cos(orient) * radius)*SCREENaspect_ratio);
		points[n].xcoord = F2Ieven(cent_x + ((float) cos(orient) * radius));
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


int VGA_rect(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color)
{
	int	ulx,uly,dimx,dimy;
	int	ul2x, ul2y, dim2x,dim2y;

//	ulx = F2Ieven(cent_x-size_x/2.*SCREENaspect_ratio);
	ulx = F2Ieven(cent_x-size_x/2.);
	uly = F2Ieven(cent_y-size_y/2.);
//	dimx = F2Ieven(size_x * SCREENaspect_ratio);
	dimx = F2Ieven(size_x);
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

//	ulx = F2Ieven(cent_x-size_x/2.*SCREENaspect_ratio);
	ulx = F2Ieven(cent_x-size_x/2.);
	uly = F2Ieven(cent_y-size_y/2.);
//	dimx = F2Ieven(size_x * SCREENaspect_ratio);
	dimx = F2Ieven(size_x);
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


void	VGAset_color(int color)
{
	_setcolor(color);
}


void	PATTERNset(int pattern)
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

void SCREENpage_flip(int option)
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

void	PALETTEsetup(long colors[16])
{
	static	short	init=0;
	int	n;

	if (!init) {
		init = 1;

		for (n=0;n<16;++n){
			colors[n]=VGA_color(n);
		}
		colors[BCKG_COLOR]=0x000000L;	// black
//		colors[BCKG_COLOR]=0x0B0B0BL;	// diffuse gray
		colors[BASE_COLOR]=0x151515L;	// light gray
		colors[FIRST_COLOR]=VGA_color(RED);
		colors[FIRST_COLOR+1]=VGA_color(BLUE);
		colors[FIRST_COLOR+2]=VGA_color(GREEN);
		colors[FIRST_COLOR+3]=VGA_color(YELLOW);
		colors[FIRST_COLOR+4]=VGA_color(CYAN);
		colors[FIRST_COLOR+5]=VGA_color(MAGENTA);
	}
	for (n=0;n<16;++n) {
		RGBcolors[n] = colors[n];
	}
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


/*************************************/
/***** Process Events of any sort ****/
/*************************************/

int	EVENTallow(void)
{
	short	buttons, dx, dy;

	if (KeyPressed())
		return EVENT_keyboard;
	if (MousePressed(&buttons) || MouseMoved(&dx,&dy))
		return EVENT_mouse;
	return EVENT_other;	// really a non-event
}

void	EVENTget_mouse(short *x, short *y, short *buttons)
{
	MousePressed(buttons);
	MouseMoved(x,y);
}

void	EVENTflush_all(void)
{
	short	deltaX, deltaY;

	while(KeyPressed())
		GetAKey();

	MouseMoved(&deltaX,&deltaY);	// to re-zero the mouse
	JoystickMoved(&deltaX, &deltaY);	// to re-zero the joystick
}

void	Beep(void)
{
	printf("\a");
}


/*************************/
/** ACCESSORY FUNCTIONS **/
/*************************/


/****** getch AND _bios_keyboard ARE NOT STANDARDIZED SO: ***********/
/******************** THE _bios_keybrd VERSION **********************/

short KeyPressed(void)
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
/***** mouse Functions *******/
/********************************************/
#define	MOUSE_INTERRUPT_NO	0x33
#define	MOUSE_RESET		0x00	// Always do first: AX = 0 if Fail; BX = #buttons 
#define	MOUSE_STATUS	0x03	// CX=xpos; DX=ypos [(0,0) upper left]; BX see below 
#define	MOUSE_MOTION	0x0B	// motion since last call in mickeys (1/200") returns CX,DX (x,y) 
#define	MOUSE_MICKEY_PIXEL_RATIO	0x0F	// default is 8/8.  CX,DX = mickeys per 8 pixels (so 16/8 takes 16 as arg) 

static	short	MOUSEinit=0;

int	MouseInit(void)
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

	MOUSEinit = 1;

	return 1;

}

int	MousePressed(int *buttons)
{
	static	union REGS inregs, outregs;

	if (!MOUSEinit)
		return 0;

	inregs.x.ax = MOUSE_STATUS;
	int86(MOUSE_INTERRUPT_NO, &inregs, &outregs);
	*buttons = outregs.x.bx;	// don't truncate the third button

	if (*buttons)
		return 1;
	else
		return 0;
}

int	MouseMoved(int *deltaX, int *deltaY)
{
	/** Returns the number of mickey's since the last call **/

	static	union REGS inregs, outregs;

	if (!MOUSEinit)
		return 0;


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
void	CLOCKbind(void (interrupt far *int08)(), int rate);
void	CLOCKunbind();
void	interrupt far CLOCKcounter(void);

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

/********************************************/
/******** Joystick related routines *********/
/********************************************/
#define	MIN_MOUSE_MVT		2
#define	MIN_JOY_MVT			2
#define	MAX_JOY_DELAY		(0xFFFF)

#define JOYSTICK_PORT (0x201)

static	uint	JoyMidX,JoyMidY;
static	uint	JoyLastX=0,JoyLastY=0;
static	int	JoyMask;
static	short	JOYSTICKinit=0;

int	JoystickInit(void)
{
	/** Assumes auto-centering joystick for now **/
	uchar portval;
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

	JOYSTICKinit = 1;

	return JoyMask;
}

int	JoystickMoved(int *deltaX, int *deltaY)
{
	uchar portval;
	uint limit=MAX_JOY_DELAY;

	if (!JOYSTICKinit)
		return 0;

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
	uchar portval;

	if (!JOYSTICKinit)
		return 0;

	outp(JOYSTICK_PORT, 0x00);		// isn't this unnecessary?
	portval=inp(JOYSTICK_PORT);

	*buttons = (int) ((portval >> 4) ^ 0x0F);

	return *buttons;
}
