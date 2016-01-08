/*
 *	Precise Delay Loops for i386
 *
 *	Copyright (C) 1993 Linus Torvalds
 *	Copyright (C) 1997 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	The __delay function must _NOT_ be inlined as its execution time
 *	depends wildly on alignment on many x86 processors. The additional
 *	jump magic is needed to get the timing stable on all the CPU's
 *	we have to worry about.
 */

#include <linux/sched.h>
#include <linux/delay.h>

#include <asm/delay.h>
#ifdef __SMP__
#include <asm/smp.h>
#endif

int x86_udelay_tsc;
	
/*
 *	Do a udelay using the TSC for any CPU that happens
 *	to have one that we trust.
 */

static void __rdtsc_delay(unsigned long loops)
{
	unsigned long bclock, now;
	
	rdtscl(bclock);
	do
	{
		rdtscl(now);
	}
	while((now-bclock) < loops);
}

/*
 *	Non TSC based delay loop for 386, 486, MediaGX
 */
 
static void __loop_delay(unsigned long loops)
{
	int d0;
	__asm__ __volatile__(
		"\tjmp 1f\n"
		".align 16\n"
		"1:\tjmp 2f\n"
		".align 16\n"
		"2:\tdecl %0\n\tjns 2b"
		:"=&a" (d0)
		:"0" (loops));
}

void __delay(unsigned long loops)
{
	if(x86_udelay_tsc)
		__rdtsc_delay(loops);
	else
		__loop_delay(loops);
}

/*
 *	This could be optimised to avoid the multiply per loop but its a
 *	delay loop so who are we kidding...  (we'd run into nasty 32-bit
 *	issues with delays > 10 ms or HZ > 100 that way)
 */

void __const_udelay(unsigned long xloops)
{
	int d0;
	__asm__("mull %0"
		:"=d" (xloops), "=&a" (d0)
		:"1" (xloops),"0" (current_cpu_data.loops_per_jiffy));
        __delay(xloops * HZ);
}

/*
 *	Do a udelay using the delay/jump loop. This won't work on 
 *	the next intel CPU's and isnt ideal on anything with APM
 */
 

void __udelay(unsigned long usecs)
{
	__const_udelay(usecs * 0x000010c6);  /* 2**32 / 1000000 */
}

