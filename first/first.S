/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

	.cpu 68000

.equ	sector_size, 512
.equ	first_level_size, 2 * sector_size

.include "macos.i"
.include "32bitmode.i"

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

.include "scsi.i"

.else

.include "floppy.i"

.endif

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

	/* test if we are in 32bit mode, otherwise reboot in 32bit mode */

	switch32bitmode

	/* buffer size to store second stage booter */

	get_second_size %d0

	/* Allocate Memory for second stage loader */

	add.l	#4, %d0
	NewPtr
	move.l	%a0, %d0
	bne	malloc_ok
	move.l	#1, %d0
	SysError
malloc_ok:
	add.l	#3, %d0
	and.l	#0xFFFFFFFC.l, %d0

	/* load second stage */

	load_second

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
