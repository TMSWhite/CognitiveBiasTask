#include <graph.h>


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

	++num;	// since base 0
	fnum=(float)num;
	pDsize=Dsize/fnum;

	xcent=Dx0+Dsize/(float)2.;	// will always center in X
	Dy0=Dysep + ((float) position * (Dysep+Dsize));

	if (position)
		Dy0 += YSEP_INC * Dysep;	// so have extra space between 0 and targets.

	/** Outline box in which things will be drawn **/
	set_pattern(0);
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
		set_pattern(pattern+1);

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

void	set_pattern(int pattern)
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

void page_flip(int option)
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

/********************************************************************/
/************************* Here is _vga stuff ***********************/
/********************************************************************/
#define	BASE_ASPECT_RATIO	((float)640./(float)480.)
float	aspect_ratio;

void SCREENmode(int mode)
{
	struct _videoconfig	vc;
	static	int	current_mode = MODE_TEXT;

	if (mode == current_mode)
		return;

	switch (mode) {
		case MODE_GRAPHICS:
			if (use_ega) {
				_setvideomode(_ERESCOLOR);
				page_flip(1);
			}
			else {
				_setvideomode(_VRES16COLOR);
			}
			_remapallpalette(_palette);
			_getvideoconfig(&vc);
			XMAX=(float) vc.numxpixels;
			YMAX=(float) vc.numypixels;
			aspect_ratio=(XMAX/YMAX)/BASE_ASPECT_RATIO;
			if (force_aspect_ratio)
				aspect_ratio=force_aspect_ratio;

			Dsize = YMAX /((float) num_stimuli + YSEP_PCT * ((float) num_stimuli+(float)1.+YSEP_INC));
			Dysep = (YSEP_PCT * Dsize);
			Dx0= (XMAX-Dsize)/(float)2.;
			break;
		case MODE_TEXT:
			if (use_ega)	// to clear and reset double buffering
				page_flip(-1);
			_setvideomode(_DEFAULTMODE);
			break;
	}
	current_mode = mode;
}

#define	PI	(float)3.14159265358989323846
#define	TWO_PI	(float)6.28318530717978647692
#define  R_TO_D	((float)180.0 / PI)

int F2Ieven(float x);

int F2Ieven(float x)
{
	int	val = (int) (floor(x + .5));
	if (2*(val/2) != val)
		--val;	// shift down for even?
	return val;
}

#define	XYCOORD	struct _xycoord

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
		points[n].xcoord = F2Ieven(cent_x + ((float) cos(orient) * radius)*aspect_ratio);
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

int	VGA_polygon(int num_points, pint Xpoints, pint Ypoints,
	int filled, int color)
{
	int	n,ok;
	XYCOORD *points;

	if (num_points < 3)
		return 0;

	if ((points = (XYCOORD *) malloc((num_points+1) * sizeof(XYCOORD))) == (XYCOORD *) 0)
		return 0;

	for (n=0;n<num_points;++n) {
		points[n].xcoord = (int) ((float) Xpoints[n] * aspect_ratio);
		points[n].ycoord = Ypoints[n];
	}
	points[n] = points[0];

	_setcolor(color);
  	ok = _polygon(((filled) ? _GFILLINTERIOR : _GBORDER), points, num_points);

	if (points)
		free((void *) points);
	return ok;
}

int VGA_rect(float cent_x, float cent_y, float size_x, float size_y, float fill_pct, int color)
{
	int	ulx,uly,dimx,dimy;
	int	ul2x, ul2y, dim2x,dim2y;

	ulx = F2Ieven(cent_x-size_x/2.*aspect_ratio);
	uly = F2Ieven(cent_y-size_y/2.);
	dimx = F2Ieven(size_x * aspect_ratio);
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

	ulx = F2Ieven(cent_x-size_x/2.*aspect_ratio);
	uly = F2Ieven(cent_y-size_y/2.);
	dimx = F2Ieven(size_x * aspect_ratio);
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

void	init_palette(void)
{
	int	n;

	for (n=0;n<16;++n){
		_palette[n]=VGA_color(n);
	}
	_palette[BCKG_COLOR]=0x0B0B0BL;	// diffuse gray
	_palette[BASE_COLOR]=0x151515L;	// light gray
	_palette[FIRST_COLOR]=VGA_color(RED);
	_palette[FIRST_COLOR+1]=VGA_color(BLUE);
	_palette[FIRST_COLOR+2]=VGA_color(GREEN);
	_palette[FIRST_COLOR+3]=VGA_color(YELLOW);
	_palette[FIRST_COLOR+4]=VGA_color(CYAN);
	_palette[FIRST_COLOR+5]=VGA_color(MAGENTA);
}


