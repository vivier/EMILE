/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#define SCSI_MAX_BLOCKS		128

	.section .rodata

	.align 4
	.global scsi_container

scsi_container:
_unit_id:		.short	0
_block_size:		.short	0
_max_blocks:		.long	SCSI_MAX_BLOCKS
_blocks:		.fill	SCSI_MAX_BLOCKS * 6, 0
