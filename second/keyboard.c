/*
 *
 * (c) 2005,2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <sys/types.h>

#include <macos/lowmem.h>
#include <macos/events.h>

#include "misc.h"
#include "keyboard.h"
#include "vga.h"

#define test_bit(n,m)	(((char*)(m))[(n) / 8] & (1L << (n % 8)))

typedef struct keyboard_map {
	char character;
	char shift;
	char control;
} keyboard_map_t;

static keyboard_map_t symbols[scancode_Last] = {
	{ 'a', 'A', 0x01 },	/* 00 scancode_A */
	{ 's', 'S', 0x13 },	/* 01 scancode_S */
	{ 'd', 'D', 0x04 },	/* 02 scancode_D */
	{ 'f', 'F', 0x06 },	/* 03 scancode_F */
	{ 'h', 'H', 0x08 },	/* 04 scancode_H */
	{ 'g', 'G', 0x07 },	/* 05 scancode_G */
	{ 'z', 'Z', 0x1a },	/* 06 scancode_Z */
	{ 'x', 'X', 0x18 },	/* 07 scancode_X */
	{ 'c', 'C', 0x03 },	/* 08 scancode_C */
	{ 'v', 'V', 0x16 },	/* 09 scancode_V */
	{ 0, 0, 0 },		/* 0a ? */
	{ 'b', 'B', 0x02 },	/* 0b scancode_B */
	{ 'q', 'Q', 0x11 },	/* 0c scancode_Q */
	{ 'w', 'W', 0x17 },	/* 0d scancode_W */
	{ 'e', 'E', 0x05 },	/* 0e scancode_E */
	{ 'r', 'R', 0x12 },	/* 0f scancode_R */
	{ 'y', 'Y', 0x19 },	/* 10 scancode_Y */
	{ 't', 'T', 0x14 },	/* 11 scancode_T */
	{ '1', '!', 0 },	/* 12 scancode_1 */
	{ '2', '@', 0 },	/* 13 scancode_2 */
	{ '3', '#', 0 },	/* 14 scancode_3 */
	{ '4', '$', 0 },	/* 15 scancode_4 */
	{ '6', '^', 0x1e },	/* 16 scancode_6 */
	{ '5', '%', 0 },	/* 17 scancode_5 */
	{ '=', '=', 0 },	/* 18 scancode_Equals */
	{ '9', '(', 0 },	/* 19 scancode_9 */
	{ '7', '&', 0 },	/* 1a scancode_7 */
	{ '-', '_', 0x1f },	/* 1b scancode_Minus */
	{ '8', '*', 0 },	/* 1c scancode_8 */
	{ '0', ')', 0 },	/* 1d scancode_0 */
	{ ']', '}', 0x1d },	/* 1e scancode_CloseBrace */
	{ 'o', 'O', 0x0f },	/* 1f scancode_O */
	{ 'u', 'U', 0x15 },	/* 20 scancode_U */
	{ '[', '{', 0x1b },	/* 21 scancode_OpenBrace */
	{ 'i', 'I', 0x09 },	/* 22 scancode_I */
	{ 'p', 'P', 0x10 },	/* 23 scancode_P */
	{ '\r', '\r', 0 },	/* 24 scancode_Return */
	{ 'l', 'L', 0x0c },	/* 25 scancode_L */
	{ 'j', 'J', 0x0a },	/* 26 scancode_J */
	{ '\'', '"', 0 },	/* 27 scancode_Quote */
	{ 'k', 'K', 0x0b },	/* 28 scancode_K */
	{ ';', ':', 0 },	/* 29 scancode_Colon */
	{ '\\', '|', 0x1c },	/* 2a scancode_Backslash */
	{ ',', '<', 0 },	/* 2b scancode_Comma */
	{ '/', '?', 0 },	/* 2c scancode_Comma */
	{ 'n', 'N', 0x0e },	/* 2d scancode_N */
	{ 'm', 'M', 0x0d },	/* 2e scancode_M */
	{ '.', '>', 0 },	/* 2f scancode_Stop */
	{ '\t', '\t', 0 },	/* 30 scancode_Tab */
	{ ' ', ' ', 0 },	/* 31 scancode_Space */
	{ '`', '~', 0 },	/* 32 scancode_Tilde */
	{ '\b', '\b', 0 },	/* 33 scancode_Backspace */
	{ 0, 0, 0 },		/* 34 ? */
	{ 0x1b, 0, 0 },		/* 35 scancode_Escape */
	{ 0, 0, 0 },		/* 36 ? */
	{ 0, 0, 0 },		/* 37 scancode_Command */
	{ 0, 0, 0 },		/* 38 scancode_LeftShift */
	{ 0, 0, 0 },		/* 39 scancode_CapsLock */
	{ 0, 0, 0 },		/* 3a scancode_LeftOption */
	{ 0, 0, 0 },		/* 3b scancode_LeftControl */
	{ 0, 0, 0 },		/* 3c scancode_RightShift */
	{ 0, 0, 0 },		/* 3d scancode_RightOption */
	{ 0, 0, 0 },		/* 3e scancode_RightControl */
	{ 0, 0, 0 },		/* 3f ? */
	{ 0, 0, 0 },		/* 40 ? */
	{ '.', '.', 0 },	/* 41 scancode_KeypadStop */
	{ 0, 0, 0 },		/* 42 ? */
	{ '*', '*', 0 },	/* 43 scancode_KeypadAsterisk */
	{ 0, 0, 0 },		/* 44 ? */
	{ '+', '+', 0 },	/* 45 scancode_KeypadPlus */
	{ 0, 0, 0 },		/* 46 ? */
	{ 0, 0, 0 },		/* 47 scancode_NumLock */
	{ 0, 0, 0 },		/* 48 ? */
	{ 0, 0, 0 },		/* 49 ? */
	{ 0, 0, 0 },		/* 4a ? */
	{ '/', '/', 0 },	/* 4b scancode_KeypadSlash */
	{ '\r', '\r', 0 },	/* 4c scancode_KeypadEnter */
	{ 0, 0, 0 },		/* 4d ? */
	{ '-', '-', 0 },	/* 4e scancode_KeypadSlash */
	{ 0, 0, 0 },		/* 4f ? */
	{ 0, 0, 0 },		/* 50 ? */
	{ '=', '=', 0 },	/* 51 scancode_KeypadEquals */
	{ '0', '0', 0 },	/* 52 scancode_Keypad0 */
	{ '1', '1', 0 },	/* 53 scancode_Keypad1 */
	{ '2', '2', 0 },	/* 54 scancode_Keypad2 */
	{ '3', '3', 0 },	/* 55 scancode_Keypad3 */
	{ '4', '4', 0 },	/* 56 scancode_Keypad4 */
	{ '5', '5', 0 },	/* 57 scancode_Keypad5 */
	{ '6', '6', 0 },	/* 58 scancode_Keypad6 */
	{ '7', '7', 0 },	/* 59 scancode_Keypad7 */
	{ 0, 0, 0 },		/* 5a ? */
	{ '8', '8', 0 },	/* 5b scancode_Keypad8 */
	{ '9', '9', 0 },	/* 5c scancode_Keypad9 */
	{ 0, 0, 0 },		/* 5d ? */
	{ 0, 0, 0 },		/* 5e ? */
	{ 0, 0, 0 },		/* 5f ? */
	{ 0x1b, 0, 0 },		/* 60 scancode_F5 */
	{ 0x1b, 0, 0 },		/* 61 scancode_F6 */
	{ 0x1b, 0, 0 },		/* 62 scancode_F7 */
	{ 0x1b, 0, 0 },		/* 63 scancode_F3 */
	{ 0x1b, 0, 0 },		/* 64 scancode_F8 */
	{ 0x1b, 0, 0 },		/* 65 scancode_F9 */
	{ 0, 0, 0 },		/* 66 ? */
	{ 0x1b, 0, 0 },		/* 67 scancode_F11 */
	{ 0, 0, 0 },		/* 68 ? */
	{ 0, 0, 0 },		/* 69 scancode_PrintScreen */
	{ 0, 0, 0 },		/* 6a ? */
	{ 0, 0, 0 },		/* 6b scancode_ScreenLock */
	{ 0, 0, 0 },		/* 6c ? */
	{ 0x1b, 0, 0 },		/* 6d scancode_F10 */
	{ 0, 0, 0 },		/* 6e ? */
	{ 0x1b, 0, 0 },		/* 6f scancode_F12 */
	{ 0, 0, 0 },		/* 70 ? */
	{ 0, 0, 0 },		/* 71 scancode_Pause */
	{ 0x1b, 0, 0 },		/* 72 scancode_Insert */
	{ 0x1b, 0, 0 },		/* 73 scancode_Home */
	{ 0x1b, 0, 0 },		/* 74 scancode_PageUp */
	{ 0x1b, 0, 0 },		/* 75 scancode_Del */
	{ 0x1b, 0, 0 },		/* 76 scancode_F4 */
	{ 0x1b, 0, 0 },		/* 77 scancode_End */
	{ 0x1b, 0, 0 },		/* 78 scancode_F2 */
	{ 0x1b, 0, 0 },		/* 79 scancode_PageDown */
	{ 0x1b, 0, 0 },		/* 7a scancode_F1 */
	{ 0x1b, 0, 0 },		/* 7b scancode_Left */
	{ 0x1b, 0, 0 },		/* 7c scancode_Right */
	{ 0x1b, 0, 0 },		/* 7d scancode_Down */
	{ 0x1b, 0, 0 },		/* 7e scancode_Up */
	{ 0, 0, 0 },		/* 7f ? */
};

#define BUFFER_SIZE	64
static int head = 0;
static int length = 0;
static char buffer[BUFFER_SIZE];

static inline void buffer_put(char c)
{
	buffer[(head + length) % BUFFER_SIZE] = c;
	length++;
}

static inline int buffer_get(void)
{
	int c;

	if (length == 0)
		return 0;

	c = buffer[head];
	head = (head + 1) % BUFFER_SIZE;
	length--;

	return c;
}

static inline void buffer_putstring(char *s)
{
	if (!s)
		return;

	while(*s)
		buffer_put(*s++);
}

void keyboard_get_scancode(int *modifiers, int *code)
{
	KeyMap keyboard= {0,0,0,0};
	int scancode;

	GetKeys(keyboard);

	/* modifier keys */

	*modifiers = 0;
	if (test_bit(scancode_Command, keyboard))
		*modifiers |= modifier_command;
	if (test_bit(scancode_LeftShift, keyboard) ||
	    test_bit(scancode_RightShift, keyboard))
		*modifiers |= modifier_shift;
	if (test_bit(scancode_CapsLock, keyboard))
		*modifiers |= modifier_capslock;
	if (test_bit(scancode_LeftOption, keyboard) ||
	    test_bit(scancode_RightOption, keyboard))
		*modifiers |= modifier_option;
	if (test_bit(scancode_LeftControl, keyboard) ||
	    test_bit(scancode_RightControl, keyboard))
		*modifiers |= modifier_control;

	/* other keys */

	for (scancode = 0; scancode < scancode_Last; scancode++)
	{
		/* modifiers */

		if ( (scancode == scancode_Command) ||
		     (scancode == scancode_LeftShift) ||
		     (scancode == scancode_RightShift) ||
		     (scancode == scancode_CapsLock) ||
		     (scancode == scancode_LeftOption) ||
		     (scancode == scancode_RightOption) ||
		     (scancode == scancode_LeftControl) ||
		     (scancode == scancode_RightControl) )
			continue;

		/* other keys */

		if (test_bit(scancode, keyboard))
			break;
	}
	*code = scancode;
}

int keyboard_convert_scancode(int modifiers, int scancode)
{
	int c;

	if ((modifiers & modifier_shift) || 
	    (modifiers & modifier_capslock))
	{
		c = symbols[scancode].shift;
	} else if (modifiers & modifier_control)
	{
		c = symbols[scancode].control;
	} else
	{
		c = symbols[scancode].character;
	}

	return c;
}

static int keyboard_catch()
{
	static long last_KeyTime = -1;
	long current_keytime = KeyTime;
	int modifiers, scancode;
	int c;

	keyboard_get_scancode(&modifiers, &scancode);

	if (current_keytime != last_KeyTime)
	{
		last_KeyTime = current_keytime;

		c = keyboard_convert_scancode(modifiers, scancode);

		if (c == 0x1b)
		{
			buffer_put(0x1b);
			switch(scancode)
			{
				case scancode_F5:
					buffer_putstring("[15~");
					break;
				case scancode_F6:
					buffer_putstring("[17~");
					break;
				case scancode_F7:
					buffer_putstring("[18~");
					break;
				case scancode_F3:
					buffer_putstring("OR");
					break;
				case scancode_F8:
					buffer_putstring("[19~");
					break;
				case scancode_F9:
					buffer_putstring("[20~");
					break;
				case scancode_F11:
					buffer_putstring("[23~");
					break;
				case scancode_F10:
					buffer_putstring("[21~");
					break;
				case scancode_F12:
					buffer_putstring("[24~");
					break;
				case scancode_Insert:
					buffer_putstring("[2~");
					break;
				case scancode_Home:
					buffer_putstring("[1~");
					break;
				case scancode_PageUp:
					buffer_putstring("[5~");
					break;
				case scancode_Del:
					buffer_putstring("[3~");
					break;
				case scancode_F4:
					buffer_putstring("OS");
					break;
				case scancode_End:
					buffer_putstring("[4~");
					break;
				case scancode_F2:
					buffer_putstring("OQ");
					break;
				case scancode_PageDown:
					buffer_putstring("[6~");
					break;
				case scancode_F1:
					buffer_putstring("OP");
					break;
				case scancode_Left:
					buffer_putstring("[D");
					break;
				case scancode_Right:
					buffer_putstring("[C");
					break;
				case scancode_Down:
					buffer_putstring("[B");
					break;
				case scancode_Up:
					buffer_putstring("[A");
					break;
			}
		}
		else if (c != 0)
			buffer_put(c);
	}

	vga_cursor_refresh();

	if (modifiers || (scancode < scancode_Last - 1))
		return 1;

	return 0;
}

int keyboard_keypressed()
{
	return keyboard_catch();
}

int keyboard_getchar()
{
	keyboard_catch();

	return buffer_get();
}

void keyboard_inject(char *s)
{
	buffer_putstring(s);
}
