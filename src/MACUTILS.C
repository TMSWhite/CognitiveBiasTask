/** Routines for allocating a full screen window **/
#include <stdio.h>	// only for printf
#include <stdlib.h>	// only for malloc
#include <math.h>	// for DRAW functions

#include <CursorDevices.h>
#include <QuickTimeComponents.h>
#include <Movies.h>
#include <Fonts.h>
#include <Types.h>
#include <string.h>	// for strlen

#include "utils.h"

#define	USING_MAC


/* Prototypes for internally used routines */
#define	TEXTFONT	geneva
#define	TEXTFACE	normal
#define	X_STEP	(SCREENmaxX / 80)
#define	Y_STEP	(SCREENmaxY / 25)

static ComponentInstance	GetComponent(OSType type, OSType subtype);
static char	*OSType2str(OSType type);
static void HideMenuBar (void);
static void ShowMenuBar (void);


static ComponentInstance	msClock;
static	short		mBarHigh;
static	WindowPtr	myWindow;
static	TimeRecord	Time0;
static	short	SCREENmode_current=0;

/* Globally accessible routines */

short	SCREENmaxX, SCREENmaxY;


short GRAPHICSsetup(short on)
{	
	if (on) {
		InitGraf(&qd.thePort);
		InitFonts();
		InitWindows();
		InitMenus();
		TEInit();
		InitDialogs(NULL);
		InitCursor();
		FlushEvents(everyEvent, 0);
	}
	else {
		SCREENmode(0);
		SCREENmode(-1);
	}
	SCREENmaxX = (qd.screenBits.bounds.right - qd.screenBits.bounds.left);
	SCREENmaxY = (qd.screenBits.bounds.bottom - qd.screenBits.bounds.top);	
	
	return 1;
}

short	INPUTsetup(short type)
{
	switch(type) {
		case INPUT_MOUSE:
			return 1;
		case INPUT_JOYSTICK:
			return 0;
	}
	return 1;
}




short TIMERsetup(short on)
{
	if (on) {
		if (msClock = GetComponent('clok','mill')) {
			TIMERzero_clock();
			return 1;
		} 
		else if (msClock = GetComponent('clok','micr')) {
			TIMERzero_clock();
			return 1;
		}
		else
			return 0;
	}
	else {
		if (msClock)
			CloseComponent(msClock);
	}
	return 1;
}

void SCREENmode(short graphics)
{
	static short	init=0;
	static RGBColor	black;
	static	RgnHandle	fullRgn, oldGrayRgn;
	Rect	bounds;
	
	if (graphics == SCREENmode_current)
		return;
		
	SCREENmode_current = graphics;
	
	
	if (!init) {
		short	divisor;
		short	x;
		char	*msg = "This is a test message.  What size characters are desired?";
		init = 1;
		  
		black.red = black.green = black.blue = 0;
		
		// Set up the window 
		
		bounds = qd.screenBits.bounds;
		
		myWindow = NewCWindow(nil, &bounds, "\p",
			 FALSE, plainDBox, (WindowPtr)-1, false, 0L);
		SetPort(myWindow);
		RGBForeColor(&black);
		RGBBackColor(&black);
		
		TextFont(TEXTFONT);
		TextFace(TEXTFACE);
		
		for (divisor = 25; 1; ++divisor) {
			TextSize(SCREENmaxY / divisor);
			x = TextWidth(msg,0,strlen(msg));
			if (x <= (strlen(msg) * X_STEP))
				break;
		} 		
		
		// Attempt at drawing to menu bar region 
		fullRgn = NewRgn();		
		RectRgn(fullRgn, &bounds);		
	}
	
	switch(graphics) {
		case 1:
			HideMenuBar();
			
			// This is somewhat illegal, but allows drawing to the menubar region 
			oldGrayRgn = GetGrayRgn();
			LMSetGrayRgn(fullRgn);
			
			HideCursor();
						
			SelectWindow(myWindow);
			ShowWindow(myWindow);
			EraseRect(&myWindow->portRect);
			
			break;
		case 0:
			HideWindow(myWindow);
			
			// Restore the menubar region
			LMSetGrayRgn(oldGrayRgn);
			
			ShowCursor();
					
			ShowMenuBar();
			
			break;
		case -1:
			DisposeRgn(fullRgn);	// no longer need this region
			DisposeWindow(myWindow);
			break;
	}
}


void Cls(void)
{
	if (SCREENmode_current == 1)
		EraseRect(&myWindow->portRect);
	else
		printf("\n\n");
}


long	TIMERget_ms(void)
{
	if (msClock) {
		TimeRecord	time;								
		
		ClockGetTime(msClock,&time);
		SubtractTime(&time,&Time0);
		ConvertTimeScale(&time,1000L);

		return time.value.lo;
	} 
	return 0L;
}

void	TIMERzero_clock(void)
{
	if (msClock) {
		ClockGetTime(msClock,&Time0);
	}
}

char	*Date2str(void)
{
	DateTimeRec	d;
	static	char	buf[30];
	
	GetTime(&d);
	
	sprintf(buf,"%d/%d/%d", d.month, d.day, d.year);
	return buf;
}

char	*Time2str(void)
{
	DateTimeRec d;
	static char	buf[30];
	
	GetTime(&d);

	sprintf(buf,"%d:%d:%d", d.hour, d.minute, d.second);
	return buf;
}


void	Gprint(char *msg, short y, short xpos, short color)
{
	static	short	cx=0, cy=0;
	short	len;
	
	len = TextWidth(msg,0,strlen(msg));
	
	if (SCREENmode_current != 1)
		SCREENmode(1);
		
	VGAset_color(color);
	
	if (y)
		cy = y * Y_STEP;
		
	switch(xpos) {
		default:
		case TEXT_LEFT:
			cx = 1;
			break;
		case TEXT_RIGHT:
			cx = SCREENmaxX - len;
			break;
		case TEXT_CENTER:
			cx = (SCREENmaxX - len) / 2;
			break;
		case TEXT_NEXT:
			break;	// use current cx
		case TEXT_BACKSPACE:
			cx -= len;
			VGAset_color(BLACK);	// so that backspace works
			break;
	}
	MoveTo(cx, cy);
	DrawText(msg,0,strlen(msg));
	
	if (xpos != TEXT_BACKSPACE)
		cx += len;
	
	EVENTallow();
}

/************************/
/* Internal subroutines */
/************************/

ComponentInstance	GetComponent(OSType type, OSType subtype)
{
	ComponentInstance comp;
	ComponentDescription compD;
	Component acomp = 0;
	
	comp = OpenDefaultComponent(type,subtype);
	
	return comp;
	
	if (comp)
		return comp;
		
	compD.componentType = type;
	compD.componentSubType = (OSType) 0;
	compD.componentManufacturer = (OSType) 0;
	compD.componentFlags = 0;
	compD.componentFlagsMask = 0;	
	
	printf("Unable to open component %s:%s\n", OSType2str(type), OSType2str(subtype));
	printf("Available are:\n");
	
	while(acomp = FindNextComponent(acomp,&compD)) {
		ComponentDescription cd;
		
		GetComponentInfo(acomp,&cd,nil,nil,nil);
		printf("  %s:%s\n", OSType2str(cd.componentType), OSType2str(cd.componentSubType));
	}	
	return comp;
}

char	*OSType2str(OSType type)
{
	char	*ctype;
	short	n;
	char	*ptype;
	
	ctype = malloc(5);
	
	ptype = (char *) &type;
	for (n=0;n<4;++n)
		ctype[n] = ptype[n];
		
	ctype[4] = '\0';
	return ctype;
}



//--------------------------------------------------------------  HideMenuBar
void HideMenuBar (void)
{
			// This function hides the menu bar by
			// calling LMSetMBarHeight() and then painting
			// over the menu bar with black.

	Rect			theRect;
	RgnHandle	worldRgn, menuBarRgn;
	GrafPtr		wasPort, tempPort;
	
	if (LMGetMBarHeight() != 0)	// only hide if not hidden
	{
			// First, we¹re going to save a pointer to the old
			// port and create a temporary ³scratch port² in 
			// which to define our regions and paint over the
			// menu bar.

		GetPort(&wasPort);		// remember old port
		tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
		OpenPort(tempPort);		// create temporary new port
		SetPort((GrafPtr)tempPort);

			// We get a copy of the height of the menu bar and
			// save it off in the global mBarHigh (a short).

		mBarHigh = LMGetMBarHeight();

			// Now we tell the Mac the menu bar is gone.

		LMSetMBarHeight(0);		// sets low-mem global to 0
		
			// Now we need to get a region that describes the
			// familiar rounded rectangle of the main monitor.
			// In cases with multiple monitors, the global
			// qd.screenBits will be the one with the menu bar...
			// The one whose region we¹re concerned with 
			// imitating.

		theRect = (**GetGrayRgn()).rgnBBox;
		UnionRect(&theRect, &qd.screenBits.bounds, &theRect);
		worldRgn = NewRgn();		// init new region
		OpenRgn();				// open region for defining
		FrameRoundRect(&theRect, 16, 16);
		CloseRgn(worldRgn);		// just defined the screen 
		
			// Although the region we¹ve just defined represents
			// the entire monitor with the menu bar, we¹re going
			// to trim this down to a region that represents
			// just the menu bar -- the top strip of the screen.

		theRect = qd.screenBits.bounds;
		theRect.bottom = theRect.top + mBarHigh;
		menuBarRgn = NewRgn();		// define yet another region
		RectRgn(menuBarRgn, &theRect);
		
			// Now we have worldRgn = the whole screen and
			// menuBarRgn = just the top strip where the menu bar
			// is. By finding their intersection, we manufacture
			// an exact shape of the menu bar -- rounded corners
			// only on the top.

		SectRgn(worldRgn, menuBarRgn, menuBarRgn);
		DisposeRgn(worldRgn);		// no longer need this region
		
			// Now that we have our menu bar region, we need to 
			// add it to our temporary ports¹ visRgn. In this
			// way we can use QuickDraw to paint over it.

		UnionRgn(tempPort->visRgn, 
				menuBarRgn, 
				tempPort->visRgn);
				
		DisposeRgn(menuBarRgn);	// no longer need this region
		
		PaintRect(&theRect);		// paint over the menu bar
		
		ClosePort(tempPort);		// clean up
		SetPort((GrafPtr)wasPort);
	}
}


//--------------------------------------------------------------  ShowMenuBar
// Showing the menu bar is, unfortunately, just as complex (only in reverse):

void ShowMenuBar (void)
{
	Rect			theRect;
	GrafPtr		wasPort, tempPort;
	RgnHandle	worldRgn, menuBarRgn;
	
	if (LMGetMBarHeight() == 0)	// only do if menu bar hidden
	{
		GetPort(&wasPort);		// do the port thing again
		tempPort = (GrafPtr)NewPtrClear(sizeof(GrafPort));
		OpenPort(tempPort);
		SetPort((GrafPtr)tempPort);
		
			// Restore the menu bar height in low-mem global

		LMSetMBarHeight(mBarHigh);
		
			// Create a screen region again (with rounded corners)

		theRect = (**GetGrayRgn()).rgnBBox;
		UnionRect(&theRect, &qd.screenBits.bounds, &theRect);
		worldRgn = NewRgn();
		OpenRgn();
		FrameRoundRect(&theRect, 16, 16);
		CloseRgn(worldRgn);
		
			// Create a rectangular menu bar region again

		theRect = qd.screenBits.bounds;
		theRect.bottom = theRect.top + mBarHigh;
		menuBarRgn = NewRgn();
		RectRgn(menuBarRgn, &theRect);
		
			// Intersect the 2 regions to arrive at a rounded
			// menu bar region again. 

		SectRgn(worldRgn, menuBarRgn, menuBarRgn);	
		DisposeRgn(worldRgn);

			// Once again we add it to our tempPort¹s visRgn

		UnionRgn(tempPort->visRgn, 
				menuBarRgn, 
				tempPort->visRgn);

			// But this time we ³subtract² the menu bar region
			// from the tempPort¹s visRgn -- in this way, the
			// menu bar is removed and thus can¹t be drawn over.

		DiffRgn(tempPort->visRgn, menuBarRgn, tempPort->visRgn);
		DisposeRgn(menuBarRgn);
		
		ClosePort(tempPort);	// clean up
		SetPort((GrafPtr)wasPort);
		
			// At this point, all is well and good, but the Mac
			// hasn¹t got around to drawing the menu bar. We need
			// to tell it to do so.

		DrawMenuBar();
	}
}

/********************************************************************/
/************************* Here is DRAW stuff ***********************/
/********************************************************************/


#define	PI	(float)3.14159265358989323846
#define	TWO_PI	(float)6.28318530717978647692
#define  R_TO_D	((float)180.0 / PI)

#define	BASE_COLOR	1	// since can have background and base colors

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
	Point	p;
	PolyHandle	poly;
	
	if (num_sides < 3)
		return 0;

	while (degrees > (float) 360)
		degrees -= (float) 360;
	while (degrees < (float) 0)
		degrees += (float) 360;

	step_val = TWO_PI / (double) num_sides;
	orient = (double) degrees / R_TO_D;

	/* Calculate polygon in dva first */
	poly = OpenPoly();
	
	for (n=0;n<num_sides;++n) {
		p.h = F2Ieven(cent_x + ((float) cos(orient) * radius));
		p.v = F2Ieven(cent_y - ((float) sin(orient) * radius));
		orient += step_val;
		if (orient > TWO_PI)
			orient -= TWO_PI;
			
		if (n == 0)
			MoveTo(p.h,p.v);
		else
			LineTo(p.h,p.v);
	}
	ClosePoly();

	VGAset_color(color);
	
	if (filled)
		PaintPoly(poly);
	else
		FramePoly(poly);
		
	return 1;
}


int VGA_rect(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color)
{
	int	ulx,uly,dimx,dimy;
	int	ul2x, ul2y, dim2x,dim2y;
	Rect r;

	ulx = F2Ieven(cent_x-size_x/2.);
	uly = F2Ieven(cent_y-size_y/2.);
	dimx = F2Ieven(size_x);
	dimy = F2Ieven(size_y);

	VGAset_color(color);
	SetRect(&r,ulx, uly, ulx+dimx, uly+dimy);
	PaintRect(&r);
	if (fill_pct < 1) {
		VGAset_color(BASE_COLOR);

		dim2x = F2Ieven((float) dimx * fill_pct);
		dim2y = F2Ieven((float) dimy * fill_pct);
		ul2x = ulx + (dimx - dim2x) / 2;
		ul2y = uly + (dimy - dim2y) / 2;
		
		SetRect(&r, ul2x, ul2y, ul2x+dim2x, ul2y+dim2y);
		PaintRect(&r);
	}
	return 1;
}

int VGA_ellipse(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color)
{
	int	ulx,uly,dimx,dimy;
	int	ul2x, ul2y, dim2x,dim2y;
	Rect	r;

	ulx = F2Ieven(cent_x-size_x/2.);
	uly = F2Ieven(cent_y-size_y/2.);
	dimx = F2Ieven(size_x);
	dimy = F2Ieven(size_y);

	VGAset_color(color);
	
	SetRect(&r,ulx,uly,ulx+dimx,uly+dimy);
	PaintOval(&r);
	if (fill_pct < 1) {
		VGAset_color(BASE_COLOR);

		dim2x = F2Ieven((float) dimx * fill_pct);
		dim2y = F2Ieven((float) dimy * fill_pct);
		ul2x = ulx + (dimx - dim2x) / 2;
		ul2y = uly + (dimy - dim2y) / 2;
		SetRect(&r,ul2x,ul2y,ul2x+dim2x,ul2y+dim2y);
		PaintOval(&r);
	}
	return 1;
}

static short VGArgb[16][3]= { 10000, 10000, 10000, // diffuse gray
			                  32000, 32000, 32000, // light gray
			                  32000,	0,	0,		// red
			                  	0,		0,	32000,	// blue
			                  	0,	32000,	0,		// green
			                 65535,56824, 5866,  // yellow			              
			                 	0, 32000, 32000,	// cyan
			                 32000, 0,32000, 	// magenta
			                 5000, 5000, 5000,  // darkgray
			                 0,	0,	65535,  // lightblue
			                 0,	65535, 0,  // lightgreen
			                 0,	65535,	65535,   // lightcyan
			                 65535,	0,	0,  // lightred
			                 65535, 0, 65535,  // lightmagenta
			                 65535,56824, 5866,  // yellow			              
			                 65535,65535,65535,   // white                        
			                };

void	VGAset_color(int color)
{
	RGBColor	col;
	
	col.red = VGArgb[color][0];
	col.green = VGArgb[color][1];
	col.blue = VGArgb[color][2];
	
	RGBForeColor(&col);
}


void	PATTERNset(int pattern)
{
#ifdef	USING_DOS
	switch(pattern) {
		default: _setfillmask(NULL); break;
		case 1: _setfillmask(diagonal); break;
		case 2: _setfillmask(arrows); break;
		case 3: _setfillmask(waterhash); break;
		case 4: _setfillmask(crosshash); break;
		case 5: _setfillmask(vertical); break;
		case 6: _setfillmask(horizontal); break;
	}
#endif
}

void SCREENpage_flip(int option)
{
}


void	PALETTEsetup(long colors[16])
{
	/* If using 24 bit color, can't simply change the LUT, so limited utility */
#define	MAX_NIBBLE	0x3F	// max for DOS machines
#define	SCALEFACTOR		(65535 / MAX_NIBBLE)

	static	short	init=0;
	long	color;
	short	n;
	
	if (!init)  {
		init = 1;
		
		/* Do return transform */
		for (n=0;n<16;++n) {
			colors[n] = ((VGArgb[n][0] / SCALEFACTOR) << 8) |
				((VGArgb[n][1] / SCALEFACTOR) << 4) |
				((VGArgb[n][2] / SCALEFACTOR));
		}
		return;
	}
	
	for (n=0;n<16;++n) {
		color = colors[n];
		VGArgb[n][0]= (long) (color & 0xFF) * SCALEFACTOR;
		VGArgb[n][1] = (long) ((color >> 4) & 0xFF) * SCALEFACTOR;
		VGArgb[n][2] = (long) ((color >> 8) & 0xFF) * SCALEFACTOR;
	}
}


/*************************************/
/***** Process Events of any sort ****/
/*************************************/


static	EventRecord	lastEvent = { 0 };

int	EVENTallow(void)
{
	EventRecord	theEvent;
	short	whichPart;
	WindowPtr	whichWindow;
	long	whatSelection;
	
	if( WaitNextEvent( everyEvent, &theEvent, 0L, ( RgnHandle )nil ) == true )
	{
		switch( theEvent.what )
		{
			case mouseDown:
			
			/*We must find out what kind of mouse down this was.*/
			whichPart = FindWindow(theEvent.where, &whichWindow);
			switch ( whichPart )
			{
				case inMenuBar:
					/*Click in menu bar. Let the system call MenuSelect track it.*/
					whatSelection = MenuSelect(theEvent.where);
					// DoMenuSelection(whatSelection);	/*Our own routine for handling menu selections*/
					break;
				case inSysWindow:
					/*Click in some window that isn't ours*/
					SystemClick(&theEvent, whichWindow);
					break;
				case inGoAway:
					if ( (TrackGoAway(whichWindow, theEvent.where)) )
						return;
					break;
				case inDrag:
					/*Drag a window*/
					if ( (whichWindow != FrontWindow ()) && ((theEvent.modifiers & cmdKey) == 0) )
						SelectWindow(whichWindow);
					DragWindow(whichWindow, theEvent.where, &qd.screenBits.bounds);
					break;
				case inGrow: 
					/*Ignored - we don't resize*/
					break;
				case inContent:
					/*Click in the window.*/
					if ( (whichWindow != FrontWindow ()) )
						SelectWindow(whichWindow);
					else  {
						lastEvent = theEvent;
						return EVENT_mouse;
					}
					break;

			}; /*case whichPart*/
			break; /*mouseDown*/

				break;
			case mouseUp:
				break;
			case autoKey:
			case keyDown:
				// check to see if the user has hit cmd-q (to signal a quit)
				if ( ( theEvent.modifiers & cmdKey ) && ( ( theEvent.message & charCodeMask ) == 'q' ) )
				{
						return EVENT_quit;
				}
				lastEvent = theEvent;
				return EVENT_keyboard;
				break;
			case updateEvt:
				BeginUpdate(FrontWindow());
				EndUpdate(FrontWindow());
	//			BeginUpdate(myWindow);
	//			EndUpdate(myWindow);
				break;
			case diskEvt:
			{
				Point	diskInitPt = {40,40};

				/*Handle bad disk insertions*/
				if (HiWord(theEvent.message) != noErr)
				{
					DILoad();
					DIBadMount(diskInitPt, theEvent.message);
					DIUnload();
				}
			}
				break;
			default:
				break;
		}
	}

	return EVENT_other;
}

void	EVENTget_kbd(char *modifiers, short *key);
void	EVENTget_kbd(char *modifiers, short *key)
{
	char	mod=0;
	
	if (lastEvent.modifiers & shiftKey)
		mod |= EVENT_shift_mask;
	if (lastEvent.modifiers & controlKey)
		mod |= EVENT_ctrl_mask;
	if (lastEvent.modifiers & optionKey)
		mod |= EVENT_alt_mask;
//	if (lastEvent.modifiers & cmdKey)
//		mod |= EVENT_apple_mask;
		
	*modifiers = mod;
	*key = (short) (lastEvent.message & (charCodeMask | keyCodeMask));
}

unsigned GetAKey(void)
{
	unsigned	rsp;
	
	while(!(lastEvent.what == keyDown || lastEvent.what == autoKey))
		EVENTallow();
	
	rsp = (unsigned) (lastEvent.message & (charCodeMask | keyCodeMask));
	
	lastEvent.what = nullEvent;

	return rsp;
}

short	KeyPressed(void)
{
	if (EVENTallow() == EVENT_keyboard)
		return 1;
}

void	EVENTget_mouse(short *x, short *y, short *buttons)
{	
	*x = lastEvent.where.h;
	*y = lastEvent.where.v;
	*buttons = (lastEvent.modifiers & btnState) ? 1 : 0;
}

void	EVENTflush_all(void)
{
	FlushEvents(everyEvent, 0);
}
