/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.chip 68040

	.global MMU040_disable_cache
MMU040_disable_cache:
	moveq.l	#0, %d0
	nop
	cpusha	%bc
        movec	%d0, %cacr
	cinva	%bc
	rts

	.global MMU040_get_TC
MMU040_get_TC:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%tc,%d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_get_SRP
MMU040_get_SRP:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%srp, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_get_CRP
MMU040_get_CRP:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%urp, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_get_ITT0
MMU040_get_ITT0:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%itt0, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_get_DTT0
MMU040_get_DTT0:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%dtt0, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_get_ITT1
MMU040_get_ITT1:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%itt1, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_get_DTT1
MMU040_get_DTT1:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	movec	%dtt1, %d0
	move.l	%d0,%a0@
	unlk	%fp
	rts

	.global MMU040_ptest
MMU040_ptest:
	link.w	%fp,#0
	move.l	8(%fp),%a0

	/* test address */

	ptestw	(%a0)

	movec	%mmusr,%d0
	unlk	%fp
	rts


	.global MMU040_read_phys
MMU040_read_phys:
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
