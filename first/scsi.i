/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

/* SCSI constants */

.equ	_SCSIGet,	0x0001
.equ	_SCSISelect,	0x0002
.equ	_SCSICmd,	0x0003
.equ	_SCSIComplete,	0x0004
.equ	_SCSIRead,	0x0005

.equ	COMPLETION_TIMEOUT, 300

/* SCSI macros */

.macro	SCSIDispatch selector
	move.w	#\selector, -(%sp)
	dc.w	0xA815			/* _SCSIDispatch */
	move.w	(%sp)+, %d0
.endm

.macro	get_second_size register
	/* buffer size to store second level booter */

	move.l	second_size(%pc), \register
.endm

.macro	load_second
	lea	container_end(%pc), %a6

	lea	TIB(%pc), %a0			/* TIB */
	move.l	%d0, TIB_buffer(%a0)
	lea	PRAM_buffer(%pc), %a0
	move.l	%d0, (%a0)
	
scsi_loop:
	/* prepare CDB */

	lea	CDB(%pc), %a0
	move.w	-(%a6), %d2
	beq	exit_scsi
	move.w	%d2, CDB_nb_blocks(%a0)
	move.l	-(%a6), CDB_offset(%a0)

	/* compute # of bytes to transfer = block size * # of blocks */

	move.w	block_size(%pc), %d1
	mulu	%d2, %d1

	/* prepare TIB */

	lea	TIB(%pc), %a0			/* TIB */
	move.l	%d1, TIB_size(%a0)

	/* SCSI sequence */

	/* SCSIGet */

	clr.w	-(%sp)
	SCSIDispatch(_SCSIGet)

	/* SCSISelect */

	clr.w	-(%sp)
	move.w	unit_id(%pc), -(%sp)
	SCSIDispatch(_SCSISelect)

	/* SCSICmd */

	clr.w	-(%sp)
	pea	CDB(%pc)
	move.w	#10, -(%sp)
	SCSIDispatch(_SCSICmd)

	/* SCSIRead */

	clr.w	-(%sp)
	pea	TIB(%pc)
	SCSIDispatch(_SCSIRead)

	/* SCSIComplete	*/

	clr.w	-(%sp)
	pea	stat(%pc)
	pea	message(%pc)
	move.l	#COMPLETION_TIMEOUT, -(%sp)
	SCSIDispatch(_SCSIComplete)
	bra	scsi_loop
exit_scsi:
	
	lea	PRAM_buffer(%pc), %a0
	move.l	(%a0), %a0
.endm

.equ	READ_10, 0x28
.equ	CDB_offset, 2
.equ	CDB_nb_blocks, 7

	.align	4
CDB:
	.byte	READ_10
	.byte	0
	.long	0		/* offset to read, big-endian, like m68k */
	.byte	0
	.short	0	/* number of blocks to read, big-endian */
	.byte	0

.equ	op_inc, 1
.equ	op_no_inc, 2
.equ	op_stop, 7

.equ	TIB_buffer, 2
.equ	TIB_size, 6
	.align	4
TIB:
	.short	op_inc
	.long	0
	.long	0
	.short	op_stop
	.long	0
	.long	0

/* SCSI complete result */

stat:	.short 0
message:	.short 0
