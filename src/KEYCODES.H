
#ifdef USING_DOS

#define K_ESCAPE        0x011b
#define K_LEFTARROW     0x4b00
#define K_RIGHTARROW    0x4d00
#define K_UPARROW       0x4800
#define K_DOWNARROW     0x5000
#define K_PAGEUP        0x4900
#define K_PAGEDN        0x5100
#define K_HOME          0x4700
#define K_END           0x4f00
#define K_GREY_PLUS		0x4e2b
#define K_GREY_MINUS		0x4a2d
#define K_F1				0x3b00
#define K_F10				0x4400
#define K_ENTER			0x1c0d
#define	K_RETURN		0x1c0d
#define K_BACKSPACE     0x0e08

#define	BASE_ASPECT_RATIO	((float) 640/(float) 480)


#endif

#ifdef	USING_MAC

#define K_ESCAPE        0x351b

#define K_LEFTARROW     0x7b1c
#define K_RIGHTARROW    0x7c1d
#define K_UPARROW       0x7e1e
#define K_DOWNARROW     0x7d1f

/*  Macs don't have the keypad substitute for the arrow keys *
#define K_LEFTARROW     0x5634
#define K_RIGHTARROW    0x5836
#define K_UPARROW       0x5b38
#define K_DOWNARROW     0x5432
*/

#define K_PAGEUP        0x5c39
#define K_PAGEDN        0x5533
#define K_HOME          0x5937
#define K_END           0x5331
#define K_GREY_PLUS		0x452b
#define K_GREY_MINUS	0x4e2d
#define K_F1			0x7a10
#define K_F10			0x6d10
#define K_ENTER			0x4c03
#define	K_RETURN		0x240d
#define	K_BACKSPACE		0x3308

#define	BASE_ASPECT_RATIO	((float) SCREENmaxX / (float) SCREENmaxY)	// Macs use square pixels by default


#endif


