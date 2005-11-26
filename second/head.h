/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __HEAD_H__
#define __HEAD_H__

#define COMMAND_LINE_LENGTH	256

/*
 * WARNING: remember that m68k is big endian, like powerPC.
 *	    i386 is little-endian
 */

typedef struct emile_l2_header emile_l2_header_t;

struct emile_l2_header {
	/* EM01 */

	u_int32_t	entry;
	u_int32_t	signature;
	/* EMO4 addendum: if kernel_image_size == 0,
	 *		  kernel_image_offset is a pointer to a container
	 * EM05 addendum: if kernel_image_size == kernel_size
	 * 		  kernel is not compressed
	 */
	u_int32_t	kernel_image_offset;
	u_int32_t	kernel_image_size;
	u_int32_t	kernel_size;
	u_int32_t	ramdisk_offset;
	u_int32_t	ramdisk_size;
	int8_t		command_line[COMMAND_LINE_LENGTH];

	/* EM02 */

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

	/* EM03 */

	u_int32_t	gestaltID;
}  __attribute__((packed));

#define EMILE_ID_MASK		0xFFF0
#define EMILE_VERSION_MASK	0x000F

#define EMILE_ID(a)		((a) & EMILE_ID_MASK)
#define EMILE_VERSION(a)	((a) & EMILE_VERSION_MASK)

#define EMILE_01_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'1')
#define EMILE_02_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'2')
#define EMILE_03_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'3')
#define EMILE_04_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'4')
#define EMILE_05_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'5')

#define EMILE_COMPAT(a,b)	( ( EMILE_ID(a) == EMILE_ID(b) ) && \
				  ( EMILE_VERSION(a) <= EMILE_VERSION(b) ) )
enum {
	STDOUT_VGA	=	0x00000001,
	STDOUT_SERIAL0	=	0x00000002,
	STDOUT_SERIAL1	=	0x00000004,
	ENABLE_DEBUG	=	0x80000000,
};
#endif /* __HEAD_H__ */
