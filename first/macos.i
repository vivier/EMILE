/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

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

.macro NewPtrClear
	.short 0xA31E
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
