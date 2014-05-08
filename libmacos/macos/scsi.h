/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_SCSI_H__
#define __MACOS_SCSI_H__
#include <macos/traps.h>
#include <macos/types.h>

enum {
   op_inc	= 1,	/* transfer data, increment buffer pointer */
   op_no_inc	= 2,	/* transfer data, don't increment pointer */
   op_add	= 3,	/* add long to address */
   op_mode	= 4,	/* move long to address */
   op_loop	= 5,	/* decrement counter and loop if > 0 */
   op_nop	= 6,	/* no operation */
   op_stop	= 7,    /* stop TIB execution */
   op_comp	= 8,	/* compare SCSI data with memory */
};

enum {
	scCommErr	= 2,	/* Communications error, operation timeout */
	scBadParmsErr	= 4,	/* Unrecognized TIB instruction */
	scPhaseErr	= 5,	/* Phase error on the SCSI bus */
	scCompareErr	= 6,	/* Comparison error from scComp instruction */
};

enum {
	scsiErrorBase	= -7936
};

enum {
	scsiBusy	= scsiErrorBase + 49,
};

typedef struct TIB {	/* Transfer Instruction Block */
   short	opcode;	/* operation code */
   int		param1;	/* 1st parameter */
   int		param2;	/* 2nd parameter */
} __attribute__((packed)) TIB_t;

#ifdef __mc68000__
static inline OSErr SCSICmd(void* buffer, short count)
{
	register OSErr ret asm("%%d0");

	asm("	clr.w -(%%sp)\n"
	    "	move.l %1, -(%%sp)\n"
	    "	move.w %2, -(%%sp)\n"
		SCSIDispatch(_SCSICmd)
		: "=d" (ret) : "g" (buffer), "g" (count) : UNPRESERVED_REGS );

	return ret;
}

static inline OSErr SCSIComplete(short *stat, short *message, unsigned long wait)
{
	register OSErr ret asm("%%d0");

	asm("clr.w	-(%%sp)\n"
	    "	move.l	%0, -(%%sp)\n"
	    "	move.l	%1, -(%%sp)\n"
	    "	move.l	%2, -(%%sp)\n"
		SCSIDispatch(_SCSIComplete)
	: "=d" (ret) : "g" (stat), "g" (message), "g" (wait) : UNPRESERVED_REGS );

	return ret;
}

static inline OSErr SCSIGet(void)
{
	register OSErr ret asm("%%d0");

	asm("clr.w   -(%%sp)\n"
		SCSIDispatch(_SCSIGet)
	    : "=d" (ret) :: UNPRESERVED_REGS );

	return ret;
}

static inline OSErr SCSIRead(void *tibPtr)
{
	register OSErr ret asm("%%d0");

	asm("clr.w   -(%%sp)\n"
	    "	move.l	%1, -(%%sp)\n"
		SCSIDispatch(_SCSIRead)
	: "=d" (ret) : "g" (tibPtr) : UNPRESERVED_REGS );

	return ret;
}

static inline OSErr SCSIReset(void)
{
	register OSErr ret asm("%%d0");

	asm("clr.w	-(%%sp)\n"
		SCSIDispatch(_SCSIReset)
	: "=d" (ret)  :: UNPRESERVED_REGS );

	return ret;
}

static inline OSErr SCSISelect(short targetID)
{
	register OSErr ret asm("%%d0");

	asm("move.w	%1, %%d0\n"
	    "	clr.w	-(%%sp)\n"
	    "	move.w	%%d0, -(%%sp)\n"
		SCSIDispatch(_SCSISelect)
	: "=d" (ret) : "g" (targetID) : UNPRESERVED_REGS );

	return ret;
}

static inline OSErr SCSIStat(void)
{
	register OSErr ret asm("%%d0");

	asm("clr.w	-(%%sp)\n"
		SCSIDispatch(_SCSIStat)
	: "=d" (ret)  :: UNPRESERVED_REGS );

	return ret;
}
#endif /* __mc68000__ */
#endif /* __MACOS_SCSI_H__ */
