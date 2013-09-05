/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

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

.macro get_second_size register
	lea	ioReqCount(%pc),%a0
	move.l	(%a0), \register
.endm

.macro load_second
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
.endm
