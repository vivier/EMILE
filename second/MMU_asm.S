/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.global get_TC
get_TC:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%tc,%a0@
	unlk	%fp
	rts

	.global get_SRP
get_SRP:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%srp,%a0@
	unlk	%fp
	rts
	
	.global get_CRP
get_CRP:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%crp,%a0@
	unlk	%fp
	rts

	.global get_TT0
get_TT0:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%tt0, %a0@
	unlk	%fp
	rts

	.global get_TT1
get_TT1:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%tt1,%a0@
	unlk	%fp
	rts

	.global ptest
ptest:
	link.w	%fp,#-8
	move.l	8(%fp),%a0

	/* test address */

	suba.l	%a1, %a1
	ptestr	#1,(%a0),#7,%a1

	/* get result */

	move.l	12(%fp),%a0
	move.l	%a1,(%a0)

	lea	-8(%fp),%a0
	pmove	%psr,%a0@
	move.w	%a0@, %d0
	unlk	%fp
	rts

.TT0:	.long	0

	.global read_phys
read_phys:
	link.w	%fp,#-8

	/* get the address to read */

	move.l	8(%fp),%d0
	move.l	%d0,%a0

	/* save the %TT0 register */

	lea	.TT0,%a1
	pmove	%tt0,%a1@

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

	move.l	%d0, -8(%fp)
	pmove 	-8(%fp), %tt0

	/* read real memory */

	move.l	%a0@,%d0

	/* restore %tt0 */

	pmove	%a1@,%tt0

	/* restore  interrupts */

	move	%d1, %sr
	move.l	(%sp)+, %d1

	unlk	%fp
	rts

	.global write_phys
write_phys:
	link.w	%fp,#-8

	/* get the address to read */

	move.l	8(%fp),%d0
	move.l	%d0,%a0
	move.l	12(%fp),%d2

	/* save the %TT0 register */

	lea	.TT0,%a1
	pmove	%tt0,%a1@

	/* compute %TT0 new value */

	andi.l	#0xFF000000, %d0	/* 8 high bits of address */
	ori.l	#0x00008007, %d0	/* Enable, Caching allowed, write access
					 * Ignore Function Code
					 */

	/* disable interrupts */

	move.l	%d1,-(%sp)
	move	%sr,%d1
	ori.w	#0x0700,%sr

	/* set %TT0 with new value */

	move.l	%d0, -8(%fp)
	pmove 	-8(%fp), %tt0

	/* write real memory */

	move.l	%d2, %a0@

	/* restore %tt0 */

	pmove	%a1@,%tt0

	/* restore  interrupts */

	move	%d1, %sr
	move.l	(%sp)+, %d1

	unlk	%fp
	rts
