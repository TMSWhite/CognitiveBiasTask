#include <stdio.h>
#include <dos.h>
#include <bios.h>

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


