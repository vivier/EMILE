.equ	MSR_LG_EE,	0	# Little Endian
.equ	MSR_LG_RI,	1	# Exception
.equ	MSR_LG_PMM,	2	# Performance monitor
.equ	MSR_LG_PX,	2	# Protection Exclusive Mode
.equ	MSR_LG_PE,	3	# Protection Enable
.equ	MSR_LG_DR,	4	# Data Relocate
.equ	MSR_LG_IR,	5	# Instruction Relocate
.equ	MSR_LG_IP,	6	# Exception prefix 0x000/0xFFF
.equ	MSR_LG_FE1,	8	# Floating Exception mode 1
.equ	MSR_LG_DE,	9	# Debug Exception Enable
.equ	MSR_LG_BE,	9	# Branch Trace
.equ	MSR_LG_SE,	10	# Single Step
.equ	MSR_LG_FE0,	11	# Floating Exception mode 0
.equ	MSR_LG_ME,	12	# Machine Check Enable
.equ	MSR_LG_FP,	13	# Floating Point enable
.equ	MSR_LG_PR,	14	# Problem State / Privilege Level
.equ	MSR_LG_EE,	15	# External Interrupt Enable
.equ	MSR_LG_ILE,	16	# Interrupt Little Endian
.equ	MSR_LG_CE,	17	# Critical Interrupt Enable
.equ	MSR_LG_TGPR,	17	# TLB Update registers in use
.equ	MSR_LG_WE,	18	# Wait State Enable
.equ	MSR_LG_POW,	18	# Enable Power Management
.equ	MSR_LG_VEC,	25	# Enable AltiVec
.equ	MSR_LG_HV,	60	# Hypervisor state
.equ	MSR_LG_ISF,	61	# Interrupt 64b mode valid on 630
.equ	MSR_LG_SF,	63	# Enable 64 bit mode

.macro MSR name
	.equ	MSR_\name,	1<<MSR_LG_\name
.endm

MSR	EE
MSR	ME
MSR	IR
MSR	DR

.irp	reg,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	.equ	r\reg,	\reg
.endr

	.equ	DBAT0L, 0x219
	.equ	DBAT0U,	0x218
	.equ	DBAT1L, 0x21B
	.equ	DBAT1U, 0x21A
	.equ	DBAT2L, 0x21D
	.equ	DBAT2U, 0x21C
	.equ	DBAT3L, 0x21F
	.equ	DBAT3U, 0x21E
	.equ	DBAT4L, 0x239
	.equ	DBAT4U, 0x238
	.equ	DBAT5L, 0x23B
	.equ	DBAT5U,	0x23A
	.equ	DBAT6L, 0x23D
	.equ	DBAT6U, 0x23C
	.equ	DBAT7L, 0x23F
	.equ	DBAT7U, 0x23E

	.equ	IBAT0L, 0x211
	.equ	IBAT0U, 0x210
	.equ	IBAT1L, 0x213
	.equ	IBAT1U, 0x212
	.equ	IBAT2L, 0x215
	.equ	IBAT2U, 0x214
	.equ	IBAT3L, 0x217
	.equ	IBAT3U, 0x216
	.equ	IBAT4L, 0x231
	.equ	IBAT4U, 0x230
	.equ	IBAT5L, 0x233
	.equ	IBAT5U, 0x232
	.equ	IBAT6L, 0x235
	.equ	IBAT6U, 0x234
	.equ	IBAT7L, 0x237
	.equ	IBAT7U, 0x236

	.equ	PVR,	0x11F	/* Processor Version Register */
