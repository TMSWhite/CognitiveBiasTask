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



