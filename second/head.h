/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __HEAD_H__
#define __HEAD_H__

/*
 * WARNING: remember that m68k is big endian, like powerPC.
 *	    i386 is little-endian
 */

typedef struct emile_l2_header emile_l2_header_t;

struct emile_l2_header {
	u_int32_t	entry;
	u_int32_t	signature;		/* EM02 */
	u_int32_t	kernel_image_offset;
	u_int32_t	kernel_image_size;
	u_int32_t	kernel_size;
	u_int32_t	ramdisk_offset;
	u_int32_t	ramdisk_size;
	int8_t		command_line[256];
	u_int32_t	console_mask;
	u_int32_t	serial0_bitrate;
	int8_t		serial0_datasize;
	int8_t		serial0_parity;
	int8_t		serial0_stopbits;
	int8_t		pad0;
	u_int32_t	serial1_bitrate;
	int8_t		serial1_datasize;
	int8_t		serial1_parity;
	int8_t		serial1_stopbits;
	int8_t		pad1;
}  __attribute__((packed));

#define EMILE_02_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'2')

enum {
	STDOUT_VGA	=	0x00000001,
	STDOUT_SERIAL0	=	0x00000002,
	STDOUT_SERIAL1	=	0x00000004,
	ENABLE_DEBUG	=	0x80000000,
};
#endif /* __HEAD_H__ */
