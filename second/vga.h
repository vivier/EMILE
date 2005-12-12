/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __VGA_H__
#define __VGA_H__

extern int vga_init();

extern void vga_put(char c);
extern void vga_print(char *s);
extern void vga_cursor_refresh(void);
extern void vga_cursor_on(void);
extern void vga_cursor_off(void);
extern void vga_cursor_save(void);
extern void vga_cursor_restore(void);

extern unsigned long vga_get_videobase();
extern unsigned long vga_get_row_bytes();
extern unsigned long vga_get_depth();
extern unsigned long vga_get_width();
extern unsigned long vga_get_height();
extern unsigned long vga_get_video();

#endif
