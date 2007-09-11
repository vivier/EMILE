/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#ifndef __VGA_H__
#define __VGA_H__

extern int vga_init(emile_l2_header_t* info);

extern void vga_put(char c);
extern void vga_print(char *s);
extern void vga_cursor_refresh(void);

extern unsigned long vga_get_videobase();
extern unsigned long vga_get_row_bytes();
extern unsigned long vga_get_depth();
extern unsigned long vga_get_width();
extern unsigned long vga_get_height();
extern unsigned long vga_get_video();
extern int vga_is_available(void);

#endif
