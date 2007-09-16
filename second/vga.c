/*
 * 
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <macos/lowmem.h>
#include <macos/quickdraw.h>
#include <libmacos.h>

#include "misc.h"
#include "vga.h"
#include "keyboard.h"
#include "head.h"
#include "config.h"

static QDGlobals qd;

#define CHARSET_0	0
#define CHARSET_A	1
#define CHARSET_B	2

static unsigned char translation[][256] = {
	/* VT100 Graphics to IBM CP-437 0 */
    {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
	0x28, 0x29, 0x2a, 0x1a, 0x1b, 0x18, 0x19, 0x2f, 
	0xdb, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0xa0, 
	0x00, 0xb1, 0x00, 0x00, 0x00, 0x00, 0xb0, 0xb1, 
	0xb0, 0x00, 0xd9, 0xbf, 0xda, 0xc0, 0xc5, 0x00, 
	0x00, 0xc4, 0x00, 0x00, 0xc3, 0xb4, 0xc1, 0xc2, 
	0xb3, 0xf3, 0xf2, 0xe3, 0x00, 0x9c, 0xb7, 0x7f, 
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
	0xa0, 0xa1, 0x9b, 0x9c, 0xa4, 0x9d, 0xa6, 0x15, 
	0xa8, 0xa9, 0xa6, 0xab, 0xaa, 0xad, 0xae, 0xaf, 
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0x14, 0xb7, 
	0xb8, 0xb9, 0xa7, 0xaf, 0xac, 0xab, 0xbe, 0xa8, 
	0xc0, 0xc1, 0xc2, 0xc3, 0x8e, 0x8f, 0x92, 0x80, 
	0xc8, 0x90, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 
	0xd0, 0xa5, 0xd2, 0xd3, 0xd4, 0xd5, 0x99, 0xd7, 
	0xd8, 0xd9, 0xda, 0xdb, 0x9a, 0xdd, 0xde, 0xdf, 
	0x85, 0xa0, 0x83, 0xe3, 0x84, 0x86, 0x91, 0x87, 
	0x8a, 0x82, 0x88, 0x89, 0x8d, 0xa1, 0x8c, 0x8b, 
	0xf0, 0xa4, 0x95, 0xa2, 0x93, 0xf5, 0x94, 0xf6, 
	0xf8, 0x97, 0xa3, 0x96, 0x81, 0xfd, 0xfe, 0x98, 
    },
	/* UK ASCII to IBM CP-437 A */
    {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
	0x5f, 0xa1, 0x9b, 0x7d, 0xa4, 0x9d, 0xa6, 0x15, 
	0xa8, 0xa9, 0xa6, 0xab, 0xaa, 0xad, 0xae, 0xaf, 
	0x66, 0x67, 0xb2, 0xb3, 0xb4, 0xb5, 0x14, 0x7e, 
	0xb8, 0xb9, 0xa7, 0xaf, 0xac, 0xab, 0xbe, 0xa8, 
	0xc0, 0xc1, 0xc2, 0xc3, 0x8e, 0x8f, 0x92, 0x80, 
	0xc8, 0x90, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 
	0xd0, 0xa5, 0xd2, 0xd3, 0xd4, 0xd5, 0x99, 0xd7, 
	0xd8, 0xd9, 0xda, 0xdb, 0x9a, 0xdd, 0xde, 0xdf, 
	0x85, 0xa0, 0x83, 0xe3, 0x84, 0x86, 0x91, 0x87, 
	0x8a, 0x82, 0x88, 0x89, 0x8d, 0xa1, 0x8c, 0x8b, 
	0xf0, 0xa4, 0x95, 0xa2, 0x93, 0xf5, 0x94, 0xf6, 
	0xf8, 0x97, 0xa3, 0x96, 0x81, 0xfd, 0xfe, 0x98, 
    },
	/* US ASCII to IBM CP-437 B */
    {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 
    }
};

#define VT100_STACK_SIZE	16

typedef struct vga_handler {

	unsigned int 	enabled;

	unsigned char*	video;
	unsigned char*	base;
	unsigned long	row_bytes;	/* in bytes */
	unsigned long	depth;		/* 4, 8, 16 or 32 */
	unsigned long	width;		/* in pixels */
	unsigned long	height;
	
	unsigned long	siz_w, siz_h;
	unsigned long	pos_x, pos_y;
	unsigned char	mask;

	/* vt100 emulation */

	int 		escape_level;
	char 		escape_stack[VT100_STACK_SIZE];
	unsigned long	saved_x, saved_y;
	int		charset;
	
} vga_handler_t ;

static unsigned char bits_depth2[16] = {
	0x00,	/* 0  : 0000 -> 00000000 */
	0x03,	/* 1  : 0001 -> 00000011 */
	0x0c,	/* 2  : 0010 -> 00001100 */
	0x0f,	/* 3  : 0011 -> 00001111 */
	0x30,	/* 4  : 0100 -> 00110000 */
	0x33,	/* 5  : 0101 -> 00110011 */
	0x3c,	/* 6  : 0110 -> 00111100 */
	0x3f,	/* 7  : 0111 -> 00111111 */
	0xc0,	/* 8  : 1000 -> 11000000 */
	0xc3,	/* 9  : 1001 -> 11000011 */
	0xcc,	/* 10 : 1010 -> 11001100 */
	0xcf,	/* 11 : 1011 -> 11001111 */
	0xf0,	/* 12 : 1100 -> 11110000 */
	0xf3,	/* 13 : 1101 -> 11110011 */
	0xfc,	/* 14 : 1110 -> 11111100 */
	0xff	/* 15 : 1111 -> 11111111 */
};

static unsigned char bits_depth4[4] = {
	0x00,	/* 0 : 00 -> 00000000 */
	0x0f,	/* 1 : 01 -> 00001111 */
	0xf0,	/* 2 : 10 -> 11110000 */
	0xFF	/* 3 : 11 -> 11111111 */
};

static unsigned char bits_depth8[2] = {
	0x00,	/* 0 : 0 -> 00000000 */
	0xff	/* 0 : 1 -> 11111111 */
};

static vga_handler_t vga =
{
	.enabled = 0,

	.video = 0,
	.base = 0,
	.row_bytes = 0,
	.depth = 0,
	.width = 0,
	.height = 0,
	.siz_w = 0,
	.siz_h = 0,
	.pos_x = 0,
	.pos_y = 0,
	.mask = 0,
};

static unsigned long cursor_save_x, cursor_save_y;

#define CURSOR_POS	0
#define CURSOR_HIGH	16

static int cursor_on = 0;
static int cursor_state = 0;

extern unsigned char* font_get(int c);

static void
draw_cursor(void)
{
	int l,w;
	unsigned char *base;
	unsigned long x_base;
	unsigned long y_base;
						        
	y_base = vga.row_bytes * vga.pos_y * 16;
	x_base = vga.pos_x * vga.depth;

	base =  vga.base + y_base + x_base;

	base += CURSOR_POS * vga.row_bytes;
	for (l = 0 ; l < CURSOR_HIGH ; l++)
	{
		for (w = 0; w < vga.depth; w++)
			base[w] ^= 0xFF;
		base += vga.row_bytes;
	}
}

static void vga_cursor(int state)
{
	if (cursor_state != state)
	{
		draw_cursor();
		cursor_state = state;
	}
}

void vga_cursor_refresh(void)
{
	if (!cursor_on)
		return;

	if (Ticks % 60 < 30)
		vga_cursor(0);
	else
		vga_cursor(1);
}

static void vga_cursor_on(void)
{
	cursor_on = 1;
	vga_cursor_refresh();
}

static void vga_cursor_off(void)
{
	cursor_on = 0;
	vga_cursor(0);
}

void vga_cursor_save(void)
{
	cursor_save_x = vga.pos_x;
	cursor_save_y = vga.pos_y;
}

void vga_cursor_restore(void)
{
	vga_cursor(0);
	vga.pos_x = cursor_save_x;
	vga.pos_y = cursor_save_y;
	vga_cursor(1);
}

static void vga_set_video_mode(int m)
{
	if (m)	/* inverse */
		vga.mask = 0x00;
	else	/* normal */
		vga.mask = 0xFF;
}

static void
draw_byte_1(unsigned char *glyph, unsigned char *base)
{
	int l;

	for (l = 0 ; l < 16; l++)
	{
		*base = vga.mask ^ (*glyph++);
		base += vga.row_bytes;
	}
}

static void
draw_byte_2(unsigned char *glyph, unsigned char *base)
{
	int l;
	int bits;

	for (l = 0 ; l < 16; l++)
	{
		bits = vga.mask ^ (*glyph++);

		base[1] = bits_depth2[bits & 0x0F];
		bits = bits >> 4;
		base[0] = bits_depth2[bits & 0x0F];

		base += vga.row_bytes;
	}
}

static void
draw_byte_4(unsigned char *glyph, unsigned char *base)
{
	int l;
	int bits;

	for (l = 0 ; l < 16; l++)
	{
		bits = vga.mask ^ (*glyph++);

		base[3] = bits_depth4[bits & 0x03];
		bits = bits >> 2;

		base[2] = bits_depth4[bits & 0x03];
		bits = bits >> 2;

		base[1] = bits_depth4[bits & 0x03];
		bits = bits >> 2;

		base[0] = bits_depth4[bits & 0x03];

		base += vga.row_bytes;
	}
}

static void
draw_byte_8(unsigned char *glyph, unsigned char *base)
{
	int l;
	int bits;

	for (l = 0 ; l < 16; l++)
	{
		bits = vga.mask ^ (*glyph++);

		base[7] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[6] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[5] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[4] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[3] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[2] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[1] = bits_depth8[bits & 0x01];
		bits = bits >> 1;
		base[0] = bits_depth8[bits & 0x01];

		base += vga.row_bytes;
	}
}

static void
draw_byte_16(unsigned char *glyph, unsigned char *base)
{
	int l;
	int bits;

	for (l = 0 ; l < 16; l++)
	{
		bits = *glyph++;

		base[15] = base[14] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[13] = base[12] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[11] = base[10] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[9] = base[8]   = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[7] = base[6]   = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[5] = base[4]   = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[3] = base[2]   = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[1] = base[0]   = bits_depth8[bits & 0x01];

		base += vga.row_bytes;
	}
}

static void
draw_byte_24(unsigned char *glyph, unsigned char *base)
{
	int l;
	int bits;

	for (l = 0 ; l < 16; l++)
	{
		bits = *glyph++;

		base[23] = base[22] = base[21] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[20] = base[19] = base[18] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[17] = base[16] = base[15] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[14] = base[13] = base[12] = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[11] = base[10] = base[9]  = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[8]  = base[7]  = base[6]  = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[5]  = base[4]  = base[3]  = bits_depth8[bits & 0x01];
		bits = bits >> 1;

		base[2]  = base[1]  = base[0]  = bits_depth8[bits & 0x01];

		base += vga.row_bytes;
	}
}

static void
draw_byte(unsigned char c, unsigned long locX, unsigned long locY)
{
	unsigned char *base;
	unsigned char *glyph;
	unsigned long x_base;
	unsigned long y_base;

	glyph =	font_get(c);
	
	y_base = vga.row_bytes * locY * 16;
	x_base = locX * vga.depth;

	base =	vga.base + y_base + x_base;

	switch(vga.depth)
	{
		case 1:
			draw_byte_1(glyph, base);
			break;
		case 2:
			draw_byte_2(glyph, base);
			break;
		case 4:
			draw_byte_4(glyph, base);
			break;
		case 8:
			draw_byte_8(glyph, base);
			break;
		case 16:
			draw_byte_16(glyph, base);
			break;
		case 24:
			draw_byte_24(glyph, base);
			break;
	}
}

static void
vga_scroll()
{
	unsigned long j;
	unsigned long i;
	unsigned long *src;
	unsigned long *dst;
	unsigned long bg32;

	/* move up the screen */

	src = (unsigned long *)(vga.base + (vga.row_bytes << 4));
	dst = (unsigned long *)vga.base;

	for (j = 0; j < vga.siz_h - 1; j++)
		for (i = 0; i < (vga.row_bytes<<2); i++)
			*dst++ = *src++;

	/* clear last line */

	if (vga.depth <= 8)
		bg32 = 0xffffffff;
	else
		bg32 = 0x00000000;

	for (j = 0; j < (vga.row_bytes << 2); j++)
		*dst++ = bg32;
}

static void vga_clear();

int
vga_init(char *mode)
{
	GDHandle hdl;
	volatile PixMapPtr pm;

	InitGraf(&qd.thePort);

	hdl = LMGetMainDevice();

	if ( (hdl == (GDHandle)0xAAAAAAAA) || (hdl == NULL) || 
	     ((**hdl).gdPMap == NULL) || ((*(**hdl).gdPMap)->baseAddr == NULL) )
	{
		vga.base = qd.screenBits.baseAddr;
		vga.row_bytes = qd.screenBits.rowBytes;
		vga.width = qd.screenBits.bounds.right - 
				qd.screenBits.bounds.left;
		vga.height = qd.screenBits.bounds.bottom - 
					qd.screenBits.bounds.top;
		vga.depth = 1;
	} else {
		pm = *(**hdl).gdPMap;

		vga.video = (unsigned char *)pm->baseAddr;
		vga.row_bytes =  pm->rowBytes & 0x3fff;
		vga.width = pm->bounds.right - pm->bounds.left;
		vga.height = pm->bounds.bottom - pm->bounds.top;
		vga.depth = pm->pixelSize;
		if (vga.depth == 15)
			vga.depth = 16;
	}

	vga.base = vga.video + pm->bounds.top * vga.row_bytes + pm->bounds.left * (vga.depth >> 3);
	
	vga.pos_x 	= 0;
	vga.pos_y 	= 0;
	vga.siz_w	= vga.width / 8;
	vga.siz_h	= vga.height / 16;
	vga.mask	= 0xFF;
	vga.escape_level= 0;
	vga.charset	= CHARSET_B;

	vga_cursor(0);
	vga_clear();

	if (strcmp(mode, "none") != 0)
		vga.enabled = 1;

	return 0;
}

static void
vga_clear()
{
	int i,j;
	unsigned long row;
	unsigned char bg;
	unsigned long bg32;
	unsigned long *base;
	unsigned char *next;

	if (vga.depth <= 8)
	{
		bg = 0xff;
		bg32 = 0xffffffff;
	}
	else
	{
		bg = 0x00;
		bg32 = 0x00000000;
	}

	for (j = 0, row = 0; j < vga.height; j++)
	{
		base = (unsigned long*)(vga.base + row);
		row += vga.row_bytes;

		for (i = 0; i < (vga.row_bytes >> 2); i++)
			*base++ = bg32;
		next = (unsigned char*)base;
		for (i = i << 2 ; i < vga.row_bytes; i++)
			*next++ = bg;
	}

	vga.pos_x 	= 0;
	vga.pos_y 	= 0;

	vga_cursor_refresh();
}

void
vga_put(char c)
{
	int tmp_x, tmp_y;
	char *end;

	if (!vga.enabled)
		return;

	vga_cursor(0);

	/* VT100 EMULATION */

	if (vga.escape_level)
	{
		vga.escape_stack[vga.escape_level - 1] = c;
		vga.escape_stack[vga.escape_level] = 0;
		vga.escape_level++;

		switch(vga.escape_stack[0])
		{
		case '7':	/* cursor save */
			vga.saved_x = vga.pos_x;
			vga.saved_y = vga.pos_y;
			goto exit_escape;

		case '8':	/* cursor restore */
			vga.pos_x = vga.saved_x;
			vga.pos_y = vga.saved_y;
			goto exit_escape;

		case '(':	/* character set selection */
			if (vga.escape_level <= 2)
				goto exit;

			if (vga.escape_stack[1] == '0')
				vga.charset = CHARSET_0;
			else if (vga.escape_stack[1] == 'A')
				vga.charset = CHARSET_A;
			else if (vga.escape_stack[1] == 'B')
				vga.charset = CHARSET_B;

			goto exit_escape;

		case '[':
			if (vga.escape_level <= 2)
				goto exit;
			
			/* Control Sequence Introducer (CSI) ends
			 * with a letter
			 */
			switch(c)
			{
			case 'J':	/* clear screen */
				if (strcmp("[2J", vga.escape_stack) == 0)
				{
					vga_clear();
					goto exit_escape;
				}
				break;

			case 'l':	/* hide cursor */
				if (strcmp("[?25l", vga.escape_stack) == 0)
				{
					vga_cursor_off();
					goto exit_escape;
				}
				break;

			case 'h':	/* show cursor */
				if (strcmp("[?25h", vga.escape_stack) == 0)
				{
					vga_cursor_on();
					goto exit_escape;
				}
				break;

			case 'H':	/* set cursor position */
				tmp_y = strtol(vga.escape_stack + 1, &end, 10);
				if (tmp_y > vga.siz_h)
					tmp_y = vga.siz_h;
				else if (tmp_y < 1)
					tmp_y = 1;
				if (*end == ';')
				{
					tmp_x = strtol(end + 1, &end, 10);
					if (tmp_x > vga.siz_w)
						tmp_x = vga.siz_w;
					else if (tmp_x < 1)
						tmp_x = 1;
					if (*end == 'H')
					{
						vga.pos_x = tmp_x - 1;
						vga.pos_y = tmp_y - 1;
						goto exit_escape;
					}
				}
				break;

			case 'm':	/* set video mode */
				if (strcmp("[7m", vga.escape_stack) == 0)
				{
					/* inverse */
					vga_set_video_mode(1);
					goto exit_escape;
				}
				else if (strcmp("[27m", vga.escape_stack) == 0)
				{
					/* normal */
					vga_set_video_mode(0);
					goto exit_escape;
				}
				break;

			case 'n':	/* get cursor position */
				if(strcmp("[6n", vga.escape_stack) == 0)
				{
					char buf[16];

					sprintf(buf, "\033[%ld;%ldR", vga.pos_y + 1, vga.pos_x + 1);
					keyboard_inject(buf);
					goto exit_escape;
				}
				break;

			default:
				goto exit;
			}
		}
	}

	vga.escape_level = 0;
	switch(c) {
		case '\r':
			vga.pos_x = 0;
			break;
		case '\n':
			vga.pos_x = 0;
			vga.pos_y++;
			break;
		case '\b':
			if (vga.pos_x > 0)
				vga.pos_x--;
			else if (vga.pos_y > 0)
			{
				vga.pos_y--;
				vga.pos_x = vga.siz_w - 1;
			}
			break;
		case '\033':	/* ESCAPE */
			vga.escape_level = 1;
			break;
		default:
			draw_byte(translation[vga.charset][(unsigned char)c],
				  vga.pos_x++, vga.pos_y);
			if (vga.pos_x >= vga.siz_w) {
				vga.pos_x = 0;
				vga.pos_y++;
			}
	}
	while (vga.pos_y >= vga.siz_h) {
		vga_scroll();
		vga.pos_y--;
	}

exit:
	vga_cursor_refresh();
	return;

exit_escape:
	vga.escape_level = 0;
	vga_cursor_refresh();
}

unsigned long vga_get_videobase()
{
	return (unsigned long)vga.base;
}

unsigned long vga_get_row_bytes()
{
	return vga.row_bytes;
}

unsigned long vga_get_depth()
{
	return vga.depth;
}

unsigned long vga_get_width()
{
	return vga.width;
}

unsigned long vga_get_height()
{
	return vga.height;
}

unsigned long vga_get_video()
{
	return (unsigned long)vga.video;
}

int vga_is_available(void)
{
	return vga.enabled;
}
