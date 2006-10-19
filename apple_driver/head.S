/*
 *
 * (c) 2006 Laurent Vivier <Laurent@lvivier.info>
 *
 * based on http://developer.apple.com/technotes/tn/tn1189.html
 *
 */

	.chip 68000
	.align	4

	.global	_start
_start:
primary_entry_point:
	bra.w	primary_setup
	dc.l	0x0001

secondary_entry_point:
	bra.w	secondary_setup

_driver_name:
	.string	".DebianCD"
_version:
	dc.w	0x001
_id:
	.string	"Debian SCSI 0.0.1"

	.string "Dummy AppleDriver for Debian boot CD"
	.string	"(c) 2006 Laurent Vivier <Laurent@lvivier.info>"
	.string "All files are distributed under the terms of GPLv2 license."

	.align	4
primary_setup:
	ori.l	#0x0f000000, %d5

secondary_setup:
	
	movem.l %d1-%a4,%sp@-
	move.l	%d5, %sp@-
	jsr	%pc@(driver_entry)
	add.l	#4, %sp
	movem.l %sp@+, %d1-%a4
	rts

driver_entry:
	moveq.l	#0, %d0
	rts
