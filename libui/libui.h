/*
 *
 * (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

typedef struct emile_window {
	int l, c;
	int h, w;
} emile_window_t;

typedef struct emile_progressbar {
	emile_window_t win;
	int max;
	int current;
} emile_progressbar_t;

typedef struct emile_list {
	char** item;
	int nb;
	int current;
} emile_list_t;


extern void console_set_cursor_position(int l, int c);
extern void console_video_inverse(void);
extern void console_video_normal(void);
extern void console_select_charset(char c);
extern void console_cursor_off();
extern void console_cursor_on();
extern void console_cursor_save();
extern void console_cursor_restore();
extern int console_getchar(void);
extern int console_keypressed(int timeout);
extern int wait_char;

extern void emile_window(emile_window_t *win);
extern emile_progressbar_t* emile_progressbar_create(emile_window_t *win, int max);
extern void emile_progressbar_value(emile_progressbar_t* bar, int value);
extern void emile_progressbar_delete(emile_progressbar_t* bar);
extern int emile_scrolllist(emile_window_t *win, emile_list_t *list, int timeout);
extern void emile_edit(char *s, int length);
