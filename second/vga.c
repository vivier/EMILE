/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "misc.h"
#include "glue.h"
#include "vga.h"

typedef struct vga_handler {

	unsigned char*	video;
	unsigned char*	base;
	unsigned long	row_bytes;	/* in bytes */
	unsigned long	depth;		/* 4, 8, 16 or 32 */
	unsigned long	width;		/* in pixels */
	unsigned long	height;
	
	unsigned long	siz_w, siz_h;
	unsigned long	pos_x, pos_y;
	
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

static vga_handler_t vga;

extern unsigned char* font_get(int c);

static void
draw_byte_1(unsigned char *glyph, unsigned char *base)
{
	int l;

	for (l = 0 ; l < 16; l++)
	{
		*base = ~(*glyph++);
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
		bits = ~(*glyph++);

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
		bits = ~(*glyph++);

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
		bits = ~(*glyph++);

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

	glyph =	font_get(c);
	
	base =	vga.base + vga.row_bytes * locY * 16 + locX * vga.depth;

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

static void vga_clear();

static void
vga_scroll()
{
	unsigned long j;
	unsigned long *src;
	unsigned long *dst;
	unsigned long bg32;

	/* move up the screen */

	src = (unsigned long *)(vga.base + vga.row_bytes * 16);
	dst = (unsigned long *)vga.base;

	for (j = 0; j < ((vga.siz_h-1)*vga.row_bytes<<2); j++)
		*dst++ = *src++;

	/* clear last line */

	if (vga.depth <= 8)
		bg32 = 0xffffffff;
	else
		bg32 = 0x00000000;

	for (j = 0; j < (vga.row_bytes << 2); j++)
		*dst++ = bg32;
}

void
vga_init()
{
	glue_display_properties((unsigned long*)&vga.base,
				&vga.row_bytes,
				&vga.width,
				&vga.height,
				&vga.depth,
				(unsigned long*)&vga.video);
	vga.pos_x 	= 0;
	vga.pos_y 	= 0;
	vga.siz_w	= vga.width / 8;
	vga.siz_h	= vga.height / 16;

	vga_clear();
}

void
vga_put(char c)
{
	switch(c) {
		case '\r':
			vga.pos_x = 0;
			break;
		case '\n':
			vga.pos_x = 0;
			vga.pos_y++;
			break;
		default:
			draw_byte((unsigned char)c, vga.pos_x++, vga.pos_y);
			if (vga.pos_x >= vga.siz_w) {
				vga.pos_x = 0;
				vga.pos_y++;
			}
	}
	while (vga.pos_y >= vga.siz_h) {
		vga_scroll();
		vga.pos_y--;
	}
}

static void
vga_clear()
{
	int i,j;
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

	for (j = 0; j < vga.height; j++)
	{
		base = (unsigned long*)(vga.base + vga.row_bytes * j);

		for (i = 0; i < (vga.row_bytes >> 2); i++)
			*base++ = bg32;
		next = (unsigned char*)base;
		for (i = i << 2 ; i < vga.row_bytes; i++)
			*next++ = bg;
	}

	vga.pos_x 	= 0;
	vga.pos_y 	= 0;
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
