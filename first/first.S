/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

.include "config.h"

.equ	drive_num, 1
.equ	fsFromStart, 1
.equ	sector_size, 512
.equ	sectors_per_track, 18 
.equ	sides, 2
.equ	track_size, sector_size * sectors_per_track
.equ	track_number, 80

.equ	floppy_size, sides * track_size * track_number
.equ	first_level_size, 2 * sector_size
.equ	second_level_size, floppy_size - first_level_size

.macro PBEject
	.short 0xA017
.endm

.macro PBReadSync
	.short 0xA002
.endm

/* Pascal string : length, string */

.macro pString string
pstring_begin_\@:
	.byte	pstring_end_\@ - pstring_string_\@
pstring_string_\@:
	.string "\string"
pstring_end_\@:
	.fill 16 - (pstring_end_\@ - pstring_begin_\@) , 1, 0
.endm

/* Structure: "Inside Macintosh: Files", p. 2-57 */

begin:
	.short	0x4C4B			/* boot blocks signature */
	bra	start			/* entry point to bootcode */
	.short	0x4418			/* boot blocks version number */
	.short	0x00			/* used internally */
	pString "Mac Bootloader"	/* System filename */
	pstring "Copyright 2004"	/* Finder filename */
	pString "Laurent Vivier"	/* debugger filename */
	pString "Distributed   "	/* debugger filename */
	pString "under GNU GPL "	/* name of startup screen */
	pString "first level   "	/* name of startup program */
	pString "version 1.0   "	/* name of system scrap file */
	.short	10			/* number of FCBs to allocate */
	.short	20			/* number of event queue elements */
	.long	0x00004300		/* system heap size on 128K Mac */
	.long	0x00000000		/* used internally */
	.long	0x00200000		/* system heap size on all machines */


start:
	move.l	#base_address, %d0

	/* save result in the ParamBlockRec.ioBuffer */

	lea	ioBuffer,%a0
	move.l	%d0,(%a0)

	/* Now, we load the second stage loader */

	lea	param_block,%a0
	PBReadSync

	/* call second stage bootloader */

	move.l	ioBuffer,%a0
	jmp	(%a0)

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

end:
.fill 1024 - (end - begin), 1, 0xda
