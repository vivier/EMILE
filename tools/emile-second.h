/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

/*
 * WARNING: remember that m68k is big endian, like powerPC.
 *	    i386 is little-endian
 */

typedef struct emile_l2_header emile_l2_header_t;

struct emile_l2_header {
	u_int32_t	entry;
	u_int32_t	signature;		/* EM01 */
	u_int32_t	kernel_image_offset;
	u_int32_t	kernel_image_size;
	u_int32_t	kernel_size;
	u_int32_t	ramdisk_offset;
	u_int32_t	ramdisk_size;
	int8_t		command_line[256];
};

#define EMILE_001_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'1')
