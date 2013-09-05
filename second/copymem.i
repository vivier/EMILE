/*
 *
 * (c) 2004 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

.macro	copymem	src, dst, size
	cmpa.l	\src, \dst
	bgt.s	1f
0:
	move.b	(\src)+,(\dst)+
	subq.l	#1,\size
	bge.s	0b

	bra	4f

1:
	adda.l	\size, \src
	adda.l	\size, \dst
2:
	move.b	-(\src),-(\dst)
	subq.l	#1,\size
	bge.s	2b
4:
.endm
