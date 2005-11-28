/*
 *
 * (c) 2004, 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
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
	/* EM01 */

	u_int32_t	entry;
	u_int32_t	signature;

	/* EMO4 addendum: if kernel_image_size == 0,
	 *		  kernel_image_offset is a pointer to a container
	 * EM05 addendum: if kernel_image_size == kernel_size
	 * 		  kernel is not compressed
	 * EM06 addendum: configuration is now in string configuration
	 */

	/* EM06 */

	u_int16_t	conf_size;
	int8_t		configuration[0];

	/*
	 * gestaltID <digit>
	 * modem <bitrate><parity><bits>	parity is n/o/e
	 * printer <bitrate><parity><bits>
	 * kernel <protocol>:<unit>/<path>
	 * 	<protocol> is "iso9660", "container", "block" ...
	 *	<unit> is "(fd0)", "(sd3)", "(sd0,4)",...
	 *	<path> is "boot/vmlinuz-2.2.25", "/install/mac/vmlinuz-2.2.25", 
	 *	"59904", "673280,654848",...
	 * parameters <kernel parameters>
	 * initrd <protocol>:<unit>/<path>
	 * configuration <protocol>:<unit>/<path>
	 */
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
#define EMILE_06_SIGNATURE	(('E'<<24)|('M'<<16)|('0'<<8)|'6')

#define EMILE_COMPAT(a,b)	( ( EMILE_ID(a) == EMILE_ID(b) ) && \
				  ( EMILE_VERSION(a) <= EMILE_VERSION(b) ) )
#endif /* __HEAD_H__ */
