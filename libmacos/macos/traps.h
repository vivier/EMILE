/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef __MACOS_TRAPS_H__
#define __MACOS_TRAPS_H__
#define	_DisposePtr	0xA01F
#define	_FInitQueue	0xA016
#define	_Gestalt	0xA1AD
#define	_GetKeys	0xA976
#define	_InitEvents	0xA06D
#define	_InitFS		0xA06C
#define	_InitGraf	0xA86E
#define	_NewPtr		0xA11E
#define	_NewPtrClear	0xA31E
#define	_PBCloseSync	0xA001
#define	_PBControlSync	0xA004
#define	_GetVolInfo	0xA007
#define	_MountVol	0xA00F
#define	_PBOpenSync	0xA000
#define	_PBReadSync	0xA002
#define	_PBWriteSync	0xA003
#define	_PBStatusSync	0xA005
#define	_DvrRemove	0xA03E
#define	_ReadXPRam	0xA051
#define	_MemoryDispatch	0xA05C
#define	_SlotManager	0xA06E
#define	_DebugUtil	0xA08D
#define	_HWPriv		0xA098
#define	_SCSIDispatch	0xA815
#define	_SysError	0xA9C9
#define	_SetEntries	0xAA3F

/*
 * Inside Macintosh Volume I, I-94: Register Saving Conventions */

#define UNPRESERVED_REGS	"%%a0", "%%a1", "%%a2", "%%d1", "%%d2", "memory"

#define _Trap(a)	#a
#define Trap(a)		"	dc.w	"_Trap(a)"	/* "#a" */\n"

/*
 * XPRam Selectors
 *
 */

#define _ReadLocation		0x000C00E4

#define XPRamSelector(a)	"	move.l	#"#a", %%d0"

#define	XPRam(selector)		\
	XPRamSelector(selector)"	/* "#selector" */\n"	\
	Trap(_ReadXPRam)

/*
 * SlotManager selectors
 *
 */

#define	_SGetCString	0x0003
#define	_SGetTypeSRsrc	0x000C
#define	_SRsrcInfo	0x0016

#define SlotManagerSelector(a)	"	move.l	#"#a", %%d0"

#define	SlotManager(selector)	\
	SlotManagerSelector(selector)"	/* "#selector" */\n"		\
	Trap(_SlotManager)

/*
 * SCSI dispatch selectors
 *
 */

#define	_SCSIReset	0x0000
#define	_SCSIGet	0x0001
#define	_SCSISelect	0x0002
#define	_SCSICmd	0x0003
#define	_SCSIComplete	0x0004
#define	_SCSIRead	0x0005
#define	_SCSIWrite	0x0006
#define _SCSIStat	0x000A

#define SCSIDispatchSelector(a)	"	move.w	#"#a", -(%%sp)"

#define	SCSIDispatch(selector)		\
	SCSIDispatchSelector(selector)"	/* "#selector" */\n"	\
	Trap(_SCSIDispatch)		\
"	move.w	(%%sp)+, %%d0\n"	\
"	ext.l	%%d0\n"

/*
 * HWPriv dispatch selectors
 *
 */

#define _FlushInstructionCache	0x0001
#define _FlushDataCache		0x0003
#define _DisableExtCache	0x0005
#define _FlushExtCache		0x0006
#define _FlushCodeCacheRange	0x0009

#define HWPrivSelector(a)	"	move.l	#"#a", %%d0"

#define	HWPriv(selector)		\
	HWPrivSelector(selector)"	/* "#selector" */\n"		\
	Trap(_HWPriv)

/*
 * Memory Dispatch selectors
 *
 */

#define _HoldMemory		    0x0000
#define _UnholdMemory		    0x0001
#define _LockMemory		    0x0002
#define _UnlockMemory		    0x0003
#define _LockMemoryContiguous	    0x0004
#define _GetPhysical		    0x0005
#define _LockMemoryForOutput	    0x000A
#define _MakeMemoryResident	    0x000B
#define _ReleaseMemoryData	    0x000C
#define _MakeMemoryNonResident	    0x000D
#define _FlushMemory		    0x000E
#define _GetVolumeVirtualMemoryInfo 0x000F

#define MemoryDispatchSelector(a)	"	move.l	#"#a", %%d0"

#define	MemoryDispatch(selector)		\
	MemoryDispatchSelector(selector)"	/* "#selector" */\n"	\
	Trap(_MemoryDispatch)

/*
 * DebugUtil selectors
 *
 */

#define _DebuggerGetMax		0x0000
#define	_DebuggerEnter		0x0001
#define _DebuggerExit		0x0002
#define _GetPageState		0x0004
#define _PageFaultFatal		0x0005
#define _DebuggerLockMemory	0x0006
#define _DebuggerUnlockMemory	0x0007
#define _EnterSupervisorMode	0x0008

#define DebugUtilSelector(a)	"	move.l	#"#a", %%d0"

#define	DebugUtil(selector)		\
	DebugUtilSelector(selector)"	/* "#selector" */\n"		\
	Trap(_DebugUtil)

#endif /* __MACOS_TRAPS_H__ */
