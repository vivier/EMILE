/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.global get_TC040
get_TC040:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%tc,%d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global get_SRP040
get_SRP040:
	link.w	%fp,#0
	movec	%srp, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global get_CRP040
get_CRP040:
	link.w	%fp,#0
	movec	%urp, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global get_ITT0040
get_ITT0040:
	link.w	%fp,#0
	movec	%itt0, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global get_DTT0040
get_DTT0040:
	link.w	%fp,#0
	movec	%dtt0, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global get_ITT1040
get_ITT1040:
	link.w	%fp,#0
	movec	%itt1, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global get_DTT1040
get_DTT1040:
	link.w	%fp,#0
	movec	%dtt1, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts


	.global read_phys040
read_phys040:
	link.w	%fp,#-8

	/* get the address to read */

	move.l	8(%fp),%d0
	move.l	%d0,%a0

	/* save the %TT0 register */

	movec	%dtt0,%d2

	/* compute %TT0 new value */

	andi.l	#0xFF000000, %d0	/* 8 high bits of address */
	ori.l	#0x00008207, %d0	/* Enable, Caching allowed, read access
					 * Ignore Function Code
					 */

	/* disable interrupts */

	move.l	%d1,-(%sp)
	move	%sr,%d1
	ori.w	#0x0700,%sr

	/* set %TT0 with new value */

	movec 	%d0, %dtt0

	/* read real memory */

	move.l	%a0@,%d0

	/* restore %tt0 */

	movec	%d2,%dtt0

	/* restore  interrupts */

	move	%d1, %sr
	move.l	(%sp)+, %d1

	unlk	%fp
	rts
