/* 
 * linux/arch/arm/boot/compressed/head-sa1100.S
 * 
 * Copyright (C) 1999 Nicolas Pitre <nico@cam.org>
 * 
 * SA1100 specific tweaks.  This is merged with head.S by the linker.
 */

#include <linux/config.h>


		.section        ".start", #alloc, #execinstr

#ifndef CONFIG_ARCH_SA1100
#error What am I doing here...
#endif

#ifdef CONFIG_SA1100_BRUTUS
@ need to enter SVC mode
#define angel_SWIreason_EnterSVC 0x17   /* from arm.h, in angel source */
#define angel_SWI_ARM (0xEF123456 & 0xffffff)
		mov	r0, #angel_SWIreason_EnterSVC
		swi	#angel_SWI_ARM

		@ turn off interrupts to prevent the angel from running
		mrs	r0, cpsr
		orr	r0, r0, #0xc0
		msr	cpsr, r0
#endif

#ifdef CONFIG_SA1100_VICTOR
		@ Copy cmdline to 0xc0000000
		mov	r1, #0xc0000000
		cmp	r0, #0
		moveq	r2, #0
1:		ldrneb	r2, [r0], #1
		cmpne	r2, #0
		strb	r2, [r1], #1
		bne	1b
#endif

		@ Data cache might be active.
		@ Be sure to flush kernel binary out of the cache,
		@ whatever state it is, before it is turned off.
		@ This is done by fetching through currently executed
		@ memory to be sure we hit the same cache.
		bic	r2, pc, #0x1f
		add	r3, r2, #0x4000		@ 16 kb is quite enough...
1:		ldr	r0, [r2], #32
		teq	r2, r3
		bne	1b
		mcr	p15, 0, r0, c7, c10, 4	@ drain WB
		mcr	p15, 0, r0, c7, c7, 0	@ flush I & D caches

		@ disabling MMU, enabling I cache
		mrc	p15, 0, r0, c1, c0, 0	@ read control reg
		bic	r0, r0, #0x0d		@ clear WB, DC, MMU
		orr	r0, r0, #0x1000		@ set Icache
		mcr	p15, 0, r0, c1, c0, 0

		@ set registers for entry
		mov	r0, #0
		mov	r1, #16


