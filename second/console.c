/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "misc.h"
#include "glue.h"
#include "console.h"

typedef struct console_handler {

	unsigned char*	video;
	unsigned char*	base;
	unsigned long	row_bytes;	/* in bytes */
	unsigned long	depth;		/* 8, 16 or 32 */
	unsigned long	width;		/* in pixels */
	unsigned long	height;
	
	unsigned long	siz_w, siz_h;
	unsigned long	pos_x, pos_y;
	
} console_handler_t ;

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

static unsigned char bits_depth4[16] = {
	0x00,	/* 0 : 00 -> 00000000 */
	0x0f,	/* 1 : 01 -> 00001111 */
	0xf0,	/* 2 : 10 -> 11110000 */
	0xFF	/* 3 : 11 -> 11111111 */
};

static unsigned char bits_depth8[16] = {
	0x00,	/* 0 : 0 -> 00000000 */
	0xff	/* 0 : 1 -> 11111111 */
};

static console_handler_t console;

extern unsigned char* font_get(int c);

static void
draw_byte_1(unsigned char *glyph, unsigned char *base)
{
	int l;

	for (l = 0 ; l < 16; l++)
	{
		*base = ~(*glyph++);
		base += console.row_bytes;
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

		*base     = bits_depth2[bits >> 4];
		*(base+1) = bits_depth2[bits & 0x0F];

		base += console.row_bytes;
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

		base += console.row_bytes;
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

		base += console.row_bytes;
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

		base += console.row_bytes;
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

		base += console.row_bytes;
	}
}

static void
draw_byte(unsigned char c, unsigned long locX, unsigned long locY)
{
	unsigned char *base;
	unsigned char *glyph;

	glyph =	font_get(c);
	
	base =	console.base + 
		console.row_bytes * locY * 16 + 
		locX * console.depth;

	switch(console.depth)
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
console_scroll()
{
	unsigned long j;
	unsigned long *src;
	unsigned long *dst;
	unsigned long bg32;

	/* move up the screen */

	src = (unsigned long *)(console.base + console.row_bytes * 16);
	dst = (unsigned long *)console.base;

	for (j = 0; j < ((console.siz_h-1)*console.row_bytes<<2); j++)
		*dst++ = *src++;

	/* clear last line */

	if (console.depth <= 8)
		bg32 = 0xffffffff;
	else
		bg32 = 0x00000000;

	for (j = 0; j < (console.row_bytes << 2); j++)
		*dst++ = bg32;
}

void
console_init()
{
	glue_display_properties((unsigned long*)&console.base,
				&console.row_bytes,
				&console.width,
				&console.height,
				&console.depth,
				(unsigned long*)&console.video);

	console.pos_x 	= 0;
	console.pos_y 	= 0;
	console.siz_w	= console.width / 8;
	console.siz_h	= console.height / 16;

	console_clear();
}

void
console_put(char c)
{
	switch(c) {
		case '\r':
			console.pos_x = 0;
			break;
		case '\n':
			console.pos_x = 0;
			console.pos_y++;
			break;
		default:
			draw_byte((unsigned char)c, console.pos_x++, console.pos_y);
			if (console.pos_x >= console.siz_w) {
				console.pos_x = 0;
				console.pos_y++;
			}
	}
	while (console.pos_y >= console.siz_h) {
		console_scroll();
		console.pos_y--;
	}
}

void
console_print(char *s)
{
	while(*s)
		console_put(*(s++));
}

void
console_clear()
{
	int i,j;
	unsigned char bg;
	unsigned long bg32;
	unsigned long *base 	= (unsigned long*)console.base;
	unsigned char *next;

	if (console.depth <= 8)
	{
		bg = 0xff;
		bg32 = 0xffffffff;
	}
	else
	{
		bg = 0x00;
		bg32 = 0x00000000;
	}

	for (j = 0; j < console.height; j++)
	{
		for (i = 0; i < (console.row_bytes >> 2); i++)
			*base++ = bg32;
		next = (unsigned char*)base;
		for (i = i << 2 ; i < console.row_bytes; i++)
			*next++ = bg;
	}

	console.pos_x 	= 0;
	console.pos_y 	= 0;
}

unsigned long console_get_videobase()
{
	return (unsigned long)console.base;
}

unsigned long console_get_row_bytes()
{
	return console.row_bytes;
}

unsigned long console_get_depth()
{
	return console.depth;
}

unsigned long console_get_width()
{
	return console.width;
}

unsigned long console_get_height()
{
	return console.height;
}

unsigned long console_get_video()
{
	return (unsigned long)console.video;
}
