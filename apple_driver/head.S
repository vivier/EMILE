/*
 *
 * (c) 2006 Laurent Vivier <Laurent@lvivier.info>
 *
 * based on http://developer.apple.com/technotes/tn/tn1189.html
 *
 */

	.cpu 68000

.macro SysError
	.short 0xA9C9
.endm

.macro NewPtrClear
	.short 0xA31E
.endm

	.align	4
_start:
primary_entry_point:
	bra.w	primary_setup
	dc.l	0x0001

secondary_entry_point:
	bra.w	secondary_setup

_driver_name:
	.string	".EMILE"
_version:
	dc.w	0x001
_id:
	.string	"EMILE SCSI 0.0.1"

	.string "APPLE_DRIVER for EMILE"
	.string	"(c) 2006 Laurent Vivier <Laurent@lvivier.info>"
	.string "All files are distributed under the terms of GPLv2 license."

	.align	4
primary_setup:
	ori.l	#0x0f000000, %d5

secondary_setup:
	
	movem.l %d1-%a4,%sp@-
	move.l	%d5, %sp@-
	bsr	driver_entry
	add.l	#4, %sp
	movem.l %sp@+, %d1-%a4
	rts

driver_entry:
	bsr	relocate

	bsr	console_init@PLTPC
	move.l  %d5, -(%sp)
	bsr     start@PLTPC

	rts

relocate:
	/* Allocate BSS section */

	move.l	#__bss_size, %d0	/* size */
	/* Alloc and clear memory (needed by GCC 3) */
	NewPtrClear			/* result in %a0 */
	move.l	%a0, %d0
	bne	malloc_ok
	SysError
malloc_ok:
	move.l	%a0,%d3
	move.l	#__bss_start, %d0
	sub.l	%d0,%d3			/* %d3 is the offset to add for BSS */

	/* real address of GOT */

	lea	_GLOBAL_OFFSET_TABLE_-.-8,%a0
	lea	(%pc, %a0), %a0

	/* linked address of GOT */

	move.l	#_GLOBAL_OFFSET_TABLE_, %d0

	/* %d2 is the offset to add for all other sections */

	move.l	%a0,%d2
	sub.l	%d0,%d2

	/* relocate BSS section */

	move.l	#__got_size, %d0
	beq	exit_relocate

got_loop:
	move.l	(%a0), %d1

	cmp.l	#__bss_start.l, %d1
	blt	other_section
	cmp.l	#_end.l, %d1
	bgt	other_section

	/* symbol in section BSS */

	add.l	%d3, %d1
	bra	got_store

other_section:
	add.l	%d2, %d1

got_store:
	move.l	%d1, (%a0)+

	subq.l	#4, %d0
	bgt	got_loop

exit_relocate:
	rts
