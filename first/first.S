/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.chip 68000

.equ	sector_size, 512
.equ	first_level_size, 2 * sector_size

.ifdef SCSI_SUPPORT
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
.else /* SCSI_SUPPORT */

/* floppy constants */

.equ	drive_num, 1
.equ	fsFromStart, 1
.equ	sectors_per_track, 18 
.equ	sides, 2
.equ	track_size, sector_size * sectors_per_track
.equ	track_number, 80

.equ	floppy_size, sides * track_size * track_number
.equ	second_level_size, floppy_size - first_level_size

/* floppy macros */

.macro PBReadSync
	.short 0xA002
.endm

.endif /* SCSI_SUPPORT */

.equ	ROMBase, 0x2ae
.equ	SysZone, 0x2a6
.equ	TheZone, 0x2a6

.macro	SetApplBase
	.short 0xa057
.endm

.equ	CPUFlag,	0x012F

.macro StripAddress
	.short 0xA055
.endm

.macro ReadXPRam
	.short 0xA051
.endm

.macro WriteXPRam
	.short 0xA052
.endm

.macro NewPtr
	.short 0xA11E
.endm

.macro SysError
	.short 0xA9C9
.endm

/* Pascal string : length, string */

.macro pString string
pstring_begin_\@:
	.byte	pstring_end_\@ - pstring_string_\@ - 1
pstring_string_\@:
	.string "\string"
pstring_end_\@:
	.fill 16 - (pstring_end_\@ - pstring_begin_\@) , 1, 0
.endm

/******************************************************************************
 *
 * Structure: "Inside Macintosh: Files", p. 2-57
 *
 *****************************************************************************/

begin:
ID:		.short	0x4C4B			/* boot blocks signature */
Entry:		bra	start			/* entry point to bootcode */
Version:	.short	0x4418			/* boot blocks version number */
PageFlags:	.short	0x00			/* used internally */
SysName:	pString "Mac Bootloader"	/* System filename */
ShellName:	pstring "Copyright 2004"	/* Finder filename */
Dbg1Name:	pString "Laurent Vivier"	/* debugger filename */
Dbg2Name:	pString "Distributed   "	/* debugger filename */
ScreenName:	pString "under GNU GPL "	/* name of startup screen */
HelloName:	pString "first level   "	/* name of startup program */
ScrapName:	pString "version 1.2   "	/* name of system scrap file */
CntFCBs:	.short	10			/* number of FCBs to allocate */
CntEvts:	.short	20		    /* number of event queue elements */
Heap128K:	.long	0x00004300	      /* system heap size on 128K Mac */
Heap256K:	.long	0x00008000		/* used internally */
SysHeapSize:	.long	0x00020000	  /* system heap size on all machines */

.ifdef SCSI_SUPPORT

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
.else /* SCSI_SUPPORT */
/******************************************************************************
 *
 * param block used to load second stage from floppy
 *
 *****************************************************************************/

param_block:
	.long	0	/* qLink : next queue entry */
	.short	0	/* qType : queue type */
	.short  0	/* ioTrap : routine trap */
	.long	0	/* ioCmdAddr: routine address */
	.long	0	/* ioCompletion : pointer to completion routine */
	.short	0	/* ioResult : result code */
	.long	0	/* ioNamePtr : pointer to pathname */
	.short	drive_num	/* ioVRefNum : volume specification */
	.short	-5	/* ioRefNum: file reference number */
	.byte	0	/* ioVersNum : version number */
	.byte	0	/* ioPermssn : read/write permission */
	.long	0	/* ioMisc : miscellaneaous */
ioBuffer: 		/* ioBuffer : data buffer */
	.long	0
ioReqCount: 		/* ioReqCount : requested number of bytes */
	.long	second_level_size
	.long	0	/* ioActCount : actual number of bytes */
	.short	fsFromStart	/* ioPosMode : positioning mode and newline char */
ioPosOffset: /* ioPosOffset : positionning offset */
	.long	first_level_size
.endif /* SCSI_SUPPORT */

/******************************************************************************
 *
 * start : load the second stage
 *
 * 	start is called from the boot block header
 *
 *	call PBReadSync() to read blocks from floppy 
 *	as described in param_block
 *
 *****************************************************************************/

	.align	4
start:
	moveal SysZone,%a0
	addal %pc@(SysHeapSize),%a0
	SetApplBase
	movel SysZone,TheZone

	/* is a 32bit aware ROM ? */

	movea.l	ROMBase,%a0
	move.w	8(%a0), %d1		/* read ROM id */

	cmp.w	#0x0178, %d1		/* only 24bit ROM */
	bls.S	bit32_ok

	/* is a 32bit aware processor ? */

	cmp.w	#1, CPUFlags	/* Is 68000 or 68010 */
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

	/* Allocate Memory for second stage loader */

.ifdef SCSI_SUPPORT
	/* buffer size to store second level booter */

	move.l	second_size(%pc), %d0
.else
	lea	ioReqCount(%pc),%a0
	move.l	(%a0), %d0
.endif

	add.l	#4, %d0
	NewPtr
	move.l	%a0, %d0
	bne	malloc_ok
	move.l	#1, %d0
	SysError
malloc_ok:
	add.l	#3, %d0
	and.l	#0xFFFFFFFC.l, %d0

.ifdef SCSI_SUPPORT

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
	move.w	#0, -(%sp)
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
.else
	/* save result in the ParamBlockRec.ioBuffer */

	lea	ioBuffer(%pc),%a0
	move.l	%d0,(%a0)

	/* Now, we load the second stage loader */

	lea	param_block(%pc),%a0
	PBReadSync
	tst.l	%d0
	beq	read_ok
	move.l	#2, %d0
	SysError
read_ok:

	move.l	ioBuffer(%pc),%a0

.endif /* SCSI_SUPPORT */

	/* call second stage bootloader */

	jmp	(%a0)

PRAM_buffer:
	.long	0
end:

/******************************************************************************
 *
 * Filler: the boot block is 2 floppy blocks
 *	  as seen on the disk of utilities of MacOS 7.6, we fill with 0xda
 *
 *****************************************************************************/

.ifdef SCSI_SUPPORT
	.fill first_level_size - (end - begin) - 10, 1, 0xda
container_end:
block_size:	.short	0
unit_id:	.short	0
second_size:	.long	0
max_blocks:	.short	container_end - end
.else
	.fill first_level_size - (end - begin), 1, 0xda
.endif
