/* 
 * Fully copied from BootX, (c) Benjamin Herrenschmidt
 * modified for EMILE (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 * This is BootX boostrap. This code is called by BootX in supervisor mode
 * with whatever mappings have been set by MacOS.
 *
 * The unmangler will first move itself up in memory in case it's current location
 * overlaps the destination of the kernel. The boostrap copies itself
 * and jump to the new bootstrap code.
 *
 * Note that all "decisions" regarding this copy are taken by BootX, the stack
 * pointer (r1) is already set to the destination stack for the kernel
 * (the boostrap has no stack).
 *
 * Then, it will copy kernel pages to their destination from a map passed in
 * by BootX. This map is appended to the unmangler and a pointer to it is
 * passed in r9 (pointer to the destination location of the unmangler).
 * This map is actually a kind of "script" with a serie of copy operations
 * to perform (a list of src/dest/size entries).
 *
 * Registers on entry:
 *
 * r2  = 1 on entry by BootX
 * r3  = 0x426f6f58	('BooX')
 * r4  = pointer to boot_infos (in kernel destination)
 * r5  = fb address (for BAT mapping) (unimplemented)
 * r6  = physical address of me
 * r7  = physical address where I must be copied.
 * r8  = size to copy (size of unmangler)
 * r9  = unmangle map (pointer to list of copy operations)
 * r10 = kernel entry in destination
 * r11 = setup BAT mapping for fb (unimplemented)
 *
 * The unmangle map is a list of tuples of 3 longs each: a source address,
 * a destination address, and a size. A size of zero indicates the end of
 * the list.
 *
 * Portions of this code from Gabriel Paubert's PReP bootloader, other
 * portions from the Linux kernel itself (arch/ppc/kernel/head.S)
 *
 */

.include "ppc_regs.i"

#define DISABLE_COPY	0

#ifndef BROKEN_THIRD_PARTY_CARDS
#define BROKEN_THIRD_PARTY_CARDS	1
#endif

#ifdef BROKEN_THIRD_PARTY_CARDS
.macro	__sync
	li	r0, 0
	cmpwi	r0, 0
	bne+	0f
	sync
0:
.endm
#else
.macro	__sync
	sync
.endm
#endif

first_bootstrap:	
	/* Disable interrupts (clear MSR_EE). rlwinm is more "smart"
	   but less readable (our constants are bit masks). */
	mfmsr	r0
	ori		r0,r0,MSR_EE|MSR_ME
	xori	r0,r0,MSR_EE|MSR_ME
	mtmsr	r0
	__sync

	/* Check if MMU was already turned off */
	cmplwi	cr0,r2,0
	beq		already_off

	/* turn the MMU off */
	mfmsr	r0
	ori		r0,r0,MSR_IR|MSR_DR	/* |MSR_IP */
	li		r2,0
	xori	r0,r0,MSR_IR|MSR_DR	/* |MSR_IP */
	mtsrr0	r6
	mtsrr1	r0
	__sync		/* I heard that some 601 will like this one */
	rfi

already_off:
	/* save some parameters */	
	mr		r31,r3
	mr		r30,r4
	mr		r29,r5

	/* flush TLBs, ivalidate BATs */
	bl		flush_tlb
	bl		inval_BATs
	bl		reset_segs
	
#if !DISABLE_COPY
	/* Need to copy myself ? */
	cmpw	r6,r7
	beq		skip_copy
	
	/* Copy ourselves */
	mr		r3,r7
	mr		r4,r6
	mr		r5,r8
	li		r6,0
	bl		copy_and_flush
	
	mr		r6, r7		/* update address of me */
	mr		r3,r31		/* restore parameters */
	mr		r4,r30
	mr		r5,r29
	li		r2,0		/* not necessary since no code here changes it, but... */
	mtlr	r7			/* Jump to copy of me (*) */
	blr
	
/* (*) note that we jump at the beginning of the copy. We could jump
       to skip_copy directly if CW knew how to calculate the offset
       directly in the inline assembler. 
 */
#endif
 
skip_copy:
	
#if !DISABLE_COPY
	/* Call unmangle code */
	bl		unmangle_kernel
#endif
	
	/* Jump to kernel */
	mr		r3,r31			/* restore parameters */
	mr		r4,r30
	li		r5,0			/* clear r5 */
	mtlr	r10
	blr

#if !DISABLE_COPY

/* Unmangle kernel code. This routine will read the array prepared by
 * BootX and follow instructions in it for copying kernel pages until
 * the kernel is fully reconstitued
 */
unmangle_kernel:
 	mflr	r13
unmangle_loop:	
 	lwz		r4,0(r9)
 	lwz		r3,4(r9)
 	lwz		r5,8(r9)
 	li		r6,0
 	addi	r9,r9,12
 	cmpwi	r5,0
 	beq		unmangle_finish
 	bl		copy_and_flush
 	b		unmangle_loop
unmangle_finish:
	mtlr	r13
 	blr

/*
 * Copy routine used to copy a piece of code and flush and invalidate
 * the caches as needed.
 * r3 = dest addr, r4 = source addr, r5 = copy limit, r6 = start offset
 * on exit, r3, r4, r5 are unchanged, r6 is updated to be >= r5.
 */
copy_and_flush:
	addi	r5,r5,-4
	addi	r6,r6,-4
4:
	li	r0,8
	mtctr	r0
3:
	addi	r6,r6,4			/* copy a cache line */
	lwzx	r0,r6,r4
	stwx	r0,r6,r3
	bdnz	3b
	dcbst	r6,r3			/* write it to memory */
	__sync
	icbi	r6,r3			/* flush the icache line */
	cmplw	r6,r5
	blt		4
	isync
	addi	r5,r5,4
	addi	r6,r6,4
	blr

#endif

/* Routine for invalidating all BATs */
inval_BATs:
	li		r3,0
	mfspr	r28,PVR
	rlwinm	r28,r28,16,16,31		/* r28 = 1 for 601, 4 for 604 */
	cmpwi	r28,1
	beq		is_601_2
	mtspr	DBAT0U,r3
	mtspr	DBAT1U,r3
	mtspr	DBAT2U,r3
	mtspr	DBAT3U,r3
is_601_2:
	mtspr	IBAT0U,r3
	mtspr	IBAT0L,r3
mtspr	IBAT1U,r3
	mtspr	IBAT1L,r3
	mtspr	IBAT2U,r3
	mtspr	IBAT2L,r3
	mtspr	IBAT3U,r3
	mtspr	IBAT3L,r3
	blr

/* Resets the segment registers to a known state */
reset_segs:
	li		r0,16		/* load up segment register values */
	mtctr	r0		/* for context 0 */
	lis	r3,0x2000	/* Ku = 1, VSID = 0 */
	li	r4,0
s_loop:
	mtsrin	r3,r4
	addi	r3,r3,1		/* increment VSID */
	addis	r4,r4,0x1000	/* address of next segment */
	bdnz	s_loop
	blr

/* Due to the PPC architecture (and according to the specifications), a
 * series of tlbie which goes through a whole 256 MB segment always flushes 
 * the whole TLB. This is obviously overkill and slow, but who cares ? 
 * It takes about 1 ms on a 200 MHz 603e and works even if residual data 
 * get the number of TLB entries wrong.
 */
flush_tlb:
	lis		r28,0x1000
flush_tlb_loop:
	addic.  r28,r28,-0x1000
	tlbie	r28
	blt		flush_tlb_loop
	
/* tlbsync is not implemented on 601, so use sync which seems to be a superset
 * of tlbsync in all cases and do not bother with CPU dependant code
 */
	__sync
	blr					
