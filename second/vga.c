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

QDGlobals qd;

#define VT100_STACK_SIZE	16

typedef struct vga_handler {

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
vga_init()
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

	vga_cursor(0);
	vga_clear();

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
				tmp_x = strtol(vga.escape_stack + 1, &end, 10);
				if (*end == ';')
				{
					tmp_y = strtol(end + 1, &end, 10);
					if (*end == 'H')
					{
						vga.pos_x = tmp_x;
						vga.pos_y = tmp_y;
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
