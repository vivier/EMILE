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
	unsigned long	entry;
	unsigned long	signature;		/* EM01 */
	unsigned long	kernel_image_offset;
	unsigned long	kernel_image_size;
	unsigned long	kernel_size;
	unsigned long	ramdisk_offset;
	unsigned long	ramdisk_size;
	char		command_line[256];
};

#define EMILE_001_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'1')
