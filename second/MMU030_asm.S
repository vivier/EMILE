/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

	.chip 68030

	.global MMU030_disable_cache
MMU030_disable_cache:
	lea 0x0808, %a0
	nop
	movec %a0, %cacr
	rts

	.global MMU030_get_TC
MMU030_get_TC:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%tc,%a0@
	unlk	%fp
	rts

	.global MMU030_get_SRP
MMU030_get_SRP:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%srp,%a0@
	unlk	%fp
	rts
	
	.global MMU030_get_CRP
MMU030_get_CRP:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%crp,%a0@
	unlk	%fp
	rts

	.global MMU030_get_TT0
MMU030_get_TT0:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%tt0, %a0@
	unlk	%fp
	rts

	.global MMU030_get_TT1
MMU030_get_TT1:
	link.w	%fp,#0
	move.l	8(%fp),%a0
	pmove	%tt1,%a0@
	unlk	%fp
	rts

	.global MMU030_ptest
MMU030_ptest:
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

	.global MMU030_read_phys
MMU030_read_phys:
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

	.global MMU030_write_phys
MMU030_write_phys:
	link.w	%fp,#-8
	move.l	%d2,%sp@-

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

	move.l	%sp@+, %d2
	unlk	%fp
	rts
