/*
 *
 * (c) 2005-2007 Laurent Vivier <Laurent@Vivier.EU>
 * 	    from BootX, (c) Benjamin Herrenschmidt
 *
 */

.equ	PC,  0x00FC
.equ	GPR, 0x0100
.equ	FPR, 0x0200

#define pc	%sp@(PC)
#define gpr(a)	%sp@(GPR + 8 * a + 4)
#define fpr0(a)	%sp@(FPR + 8 * a)
#define fpr1(a)	%sp@(FPR + 8 * a + 4)

.macro copy_PC
	move.l	%a1@+, pc
.endm

.macro copy_GPR from=0, to=31
	move.l	%a1@+, gpr(\from)
.if \to-\from
	copy_GPR "(\from+1)",\to
.endif
.endm

.macro copy_FPR from=0, to=31
	move.l	%a1@+, fpr0(\from)
	move.l	%a1@+, fpr1(\from)
.if \to-\from
	copy_FPR "(\from+1)",\to
.endif
.endm

.macro _EnterPPC
	move.l	#0x47617279, %a0 /* 'Gary' */
	move.l	#0x05051956, %a1
	move.l	#0x0000C000, %d0
	moveq	#0, %d2
	reset

	move.l	%sp,-(%sp)
0:
	dc.w	0xFE03
	beq	0b
.endm

	.align 4

	.global switch_to_PPC
switch_to_PPC:
	link.w  %fp,#0

	/* get PPC registers values */

	 move.l  8(%fp), %a1

	/* prepare stack */

	move.l	%sp, %d0
	andi.l	#0xFFFFFC00, %d0
	move.l	%d0, %sp
	move.l	#0x00BF, %d0
loop:
	clr.l	-(%sp)
	dbra	%d0, loop

	/* Initialize PPC registers */

	copy_PC
	copy_GPR
	copy_FPR

	/* Switch to PPC */

	_EnterPPC

	unlk	%fp
	rts
