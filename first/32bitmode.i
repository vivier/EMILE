.macro	switch32bitmode

	/* is a 32bit aware ROM ? */

	movea.l	ROMBase,%a0
	move.w	8(%a0), %d1		/* read ROM id */

	cmp.w	#0x0178, %d1		/* only 24bit ROM */
	bls.S	bit32_ok

	/* is a 32bit aware processor ? */

	cmp.w	#1, CPUFlag	/* Is 68000 or 68010 */
	bls.S	bit32_ok

	/* test if we are in 32bit mode */

	move.l	#-1, %d0
	StripAddress
	cmp.l	#-1, %d0
	beq.S	bit32_ok

	/* Switch to 32bit mode */

	lea	PRAM_buffer(%pc), %a0	/* where to store data */
	move.w	#1, %d0			/* size of data */
	swap	%d0
	move.w	#0x08A, %d0		/* offset in PRAM */
	ReadXPRam

	lea	PRAM_buffer(%pc), %a0
	or.b	#0x05, (%a0)

	move.w	#1, %d0			/* size of data */
	swap	%d0
	move.w	#0x08A, %d0		/* offset in PRAM */
	WriteXPRam
	
	/* jump to reset function in ROM */

	movea.l	ROMBase,%a0
	jmp	0x90(%a0)

bit32_ok:
.endm
