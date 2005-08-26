/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "lowmem.h"
#include "misc.h"
#include "glue.h"
#include "keyboard.h"

#define test_bit(n,m)	(((char*)(m))[(n) / 8] & (1L << (n % 8)))

typedef struct keyboard_map {
	char character;
	char shift;
	char option;
	char shift_option;
} keyboard_map_t;

static keyboard_map_t symbols[scancode_Last] = {
	{ 'a', 'A', 0, 0 },	/* 00 scancode_A */
	{ 's', 'S', 0, 0 },	/* 01 scancode_S */
	{ 'd', 'D', 0, 0 },	/* 02 scancode_D */
	{ 'f', 'F', 0, 0 },	/* 03 scancode_F */
	{ 'h', 'H', 0, 0 },	/* 04 scancode_H */
	{ 'g', 'G', 0, 0 },	/* 05 scancode_G */
	{ 'z', 'Z', 0, 0 },	/* 06 scancode_Z */
	{ 'x', 'X', 0, 0 },	/* 07 scancode_X */
	{ 'c', 'C', 0, 0 },	/* 08 scancode_C */
	{ 'v', 'V', 0, 0 },	/* 09 scancode_V */
	{ 0, 0, 0, 0 },		/* 0a ? */
	{ 'b', 'B', 0, 0 },	/* 0b scancode_B */
	{ 'q', 'Q', 0, 0 },	/* 0c scancode_Q */
	{ 'w', 'W', 0, 0 },	/* 0d scancode_W */
	{ 'e', 'E', 0, 0 },	/* 0e scancode_E */
	{ 'r', 'R', 0, 0 },	/* 0f scancode_E */
	{ 'y', 'Y', 0, 0 },	/* 10 scancode_Y */
	{ 't', 'T', 0, 0 },	/* 11 scancode_T */
	{ '1', '!', 0, 0 },	/* 12 scancode_1 */
	{ '2', '@', 0, 0 },	/* 13 scancode_2 */
	{ '3', '#', 0, 0 },	/* 14 scancode_3 */
	{ '4', '$', 0, 0 },	/* 15 scancode_4 */
	{ '6', '^', 0, 0 },	/* 16 scancode_6 */
	{ '5', '%', 0, 0 },	/* 17 scancode_5 */
	{ '=', '=', 0, 0 },	/* 18 scancode_Equals */
	{ '9', '(', 0, 0 },	/* 19 scancode_9 */
	{ '7', '&', 0, 0 },	/* 1a scancode_7 */
	{ '-', '-', 0, 0 },	/* 1b scancode_Minus */
	{ '8', '*', 0, 0 },	/* 1c scancode_8 */
	{ '0', ')', 0, 0 },	/* 1d scancode_0 */
	{ ']', '}', 0, 0 },	/* 1e scancode_CloseBrace */
	{ 'o', 'O', 0, 0 },	/* 1f scancode_0 */
	{ 'u', 'U', 0, 0 },	/* 20 scancode_U */
	{ '[', '{', 0, 0 },	/* 21 scancode_OpenBrace */
	{ 'i', 'I', 0, 0 },	/* 22 scancode_I */
	{ 'p', 'P', 0, 0 },	/* 23 scancode_P */
	{ '\r', '\r', 0, 0 },	/* 24 scancode_Return */
	{ 'l', 'L', 0, 0 },	/* 25 scancode_L */
	{ 'j', 'J', 0, 0 },	/* 26 scancode_J */
	{ '\'', '"', 0, 0 },	/* 27 scancode_Quote */
	{ 'k', 'K', 0, 0 },	/* 28 scancode_K */
	{ ';', ':', 0, 0 },	/* 29 scancode_Colon */
	{ '\\', '|', 0, 0 },	/* 2a scancode_Backslash */
	{ ',', '<', 0, 0 },	/* 2b scancode_Comma */
	{ '/', '?', 0, 0 },	/* 2c scancode_Comma */
	{ 'n', 'N', 0, 0 },	/* 2d scancode_D */
	{ 'm', 'M', 0, 0 },	/* 2e scancode_M */
	{ 0, 0, 0, 0 },		/* 2f scancode_Stop */
	{ '\t', '\t', 0, 0 },	/* 30 scancode_Tab */
	{ ' ', ' ', 0, 0 },	/* 31 scancode_Space */
	{ '`', '~', 0, 0 },	/* 32 scancode_Tilde */
	{ '\b', '\b', 0, 0 },	/* 33 scancode_Backspace */
	{ 0, 0, 0, 0 },		/* 34 ? */
	{ 0x1b, 0, 0, 0 },	/* 35 scancode_Escape */
	{ 0, 0, 0, 0 },		/* 36 ? */
	{ 0, 0, 0, 0 },		/* 37 scancode_Command */
	{ 0, 0, 0, 0 },		/* 38 scancode_LeftShift */
	{ 0, 0, 0, 0 },		/* 39 scancode_CapsLock */
	{ 0, 0, 0, 0 },		/* 3a scancode_LeftOption */
	{ 0, 0, 0, 0 },		/* 3b scancode_LeftControl */
	{ 0, 0, 0, 0 },		/* 3c scancode_RightShift */
	{ 0, 0, 0, 0 },		/* 3d scancode_RightOption */
	{ 0, 0, 0, 0 },		/* 3e scancode_RightControl */
	{ 0, 0, 0, 0 },		/* 3f ? */
	{ 0, 0, 0, 0 },		/* 40 ? */
	{ 0, 0, 0, 0 },		/* 41 scancode_KeypadStop */
	{ 0, 0, 0, 0 },		/* 42 ? */
	{ '*', '*', 0, 0 },	/* 43 scancode_KeypadAsterisk */
	{ 0, 0, 0, 0 },		/* 44 ? */
	{ '+', '+', 0, 0 },	/* 45 scancode_KeypadPlus */
	{ 0, 0, 0, 0 },		/* 46 ? */
	{ 0, 0, 0, 0 },		/* 47 scancode_NumLock */
	{ 0, 0, 0, 0 },		/* 48 ? */
	{ 0, 0, 0, 0 },		/* 49 ? */
	{ 0, 0, 0, 0 },		/* 4a ? */
	{ '/', '/', 0, 0 },	/* 4b scancode_KeypadSlash */
	{ '\r', '\r', 0, 0 },	/* 4c scancode_KeypadEnter */
	{ 0, 0, 0, 0 },		/* 4d ? */
	{ '-', '-', 0, 0 },	/* 4e scancode_KeypadSlash */
	{ 0, 0, 0, 0 },		/* 4f ? */
	{ 0, 0, 0, 0 },		/* 50 ? */
	{ '=', '=', 0, 0 },	/* 51 scancode_KeypadEquals */
	{ '0', '0', 0, 0 },	/* 52 scancode_Keypad0 */
	{ '1', '1', 0, 0 },	/* 53 scancode_Keypad1 */
	{ '2', '2', 0, 0 },	/* 54 scancode_Keypad2 */
	{ '3', '3', 0, 0 },	/* 55 scancode_Keypad3 */
	{ '4', '4', 0, 0 },	/* 56 scancode_Keypad4 */
	{ '5', '5', 0, 0 },	/* 57 scancode_Keypad5 */
	{ '6', '6', 0, 0 },	/* 58 scancode_Keypad6 */
	{ '7', '7', 0, 0 },	/* 59 scancode_Keypad7 */
	{ 0, 0, 0, 0 },		/* 5a ? */
	{ '8', '8', 0, 0 },	/* 5b scancode_Keypad8 */
	{ '9', '9', 0, 0 },	/* 5c scancode_Keypad9 */
	{ 0, 0, 0, 0 },		/* 5d ? */
	{ 0, 0, 0, 0 },		/* 5e ? */
	{ 0, 0, 0, 0 },		/* 5f ? */
	{ 0x1b, 0, 0, 0 },		/* 60 scancode_F5 */
	{ 0x1b, 0, 0, 0 },		/* 61 scancode_F6 */
	{ 0x1b, 0, 0, 0 },		/* 62 scancode_F7 */
	{ 0x1b, 0, 0, 0 },		/* 63 scancode_F3 */
	{ 0x1b, 0, 0, 0 },		/* 64 scancode_F8 */
	{ 0x1b, 0, 0, 0 },		/* 65 scancode_F9 */
	{ 0, 0, 0, 0 },		/* 66 ? */
	{ 0x1b, 0, 0, 0 },		/* 67 scancode_F11 */
	{ 0, 0, 0, 0 },		/* 68 ? */
	{ 0, 0, 0, 0 },		/* 69 scancode_PrintScreen */
	{ 0, 0, 0, 0 },		/* 6a ? */
	{ 0, 0, 0, 0 },		/* 6b scancode_ScreenLock */
	{ 0, 0, 0, 0 },		/* 6c ? */
	{ 0x1b, 0, 0, 0 },		/* 6d scancode_F10 */
	{ 0, 0, 0, 0 },		/* 6e ? */
	{ 0x1b, 0, 0, 0 },		/* 6f scancode_F12 */
	{ 0, 0, 0, 0 },		/* 70 ? */
	{ 0, 0, 0, 0 },		/* 71 scancode_Pause */
	{ 0x1b, 0, 0, 0 },		/* 72 scancode_Insert */
	{ 0x1b, 0, 0, 0 },		/* 73 scancode_Home */
	{ 0x1b, 0, 0, 0 },		/* 74 scancode_PageUp */
	{ 0x7f, 0, 0, 0 },		/* 75 scancode_Del */
	{ 0x1b, 0, 0, 0 },		/* 76 scancode_F4 */
	{ 0x1b, 0, 0, 0 },		/* 77 scancode_End */
	{ 0x1b, 0, 0, 0 },		/* 78 scancode_F2 */
	{ 0x1b, 0, 0, 0 },		/* 79 scancode_PageDown */
	{ 0x1b, 0, 0, 0 },		/* 7a scancode_F1 */
	{ 0x1b, 0, 0, 0 },		/* 7b scancode_Left */
	{ 0x1b, 0, 0, 0 },		/* 7c scancode_Right */
	{ 0x1b, 0, 0, 0 },		/* 7d scancode_Down */
	{ 0x1b, 0, 0, 0 },		/* 7e scancode_Up */
	{ 0, 0, 0, 0 },		/* 7f ? */
};

typedef struct char_string {
	scancodes_t code;
	char *string;
} char_string_t;

static char_string_t escape_strings[] = {
	{ scancode_Escape, 0 },
	{ scancode_F5, "[15~" },
	{ scancode_F6, "[17~" },
	{ scancode_F7, "[18~" },
	{ scancode_F3, "OR" },
	{ scancode_F8, "[19~" },
	{ scancode_F9, "[20~" },
	{ scancode_F11, "[23~" },
	{ scancode_F10, "[21~" },
	{ scancode_F12, "[24~" },
	{ scancode_Insert, "[2~" },
	{ scancode_Home, "[1~" },
	{ scancode_PageUp, "[5~" },
	{ scancode_F4, "OS" },
	{ scancode_End, "[4~" },
	{ scancode_F2, "OQ" },
	{ scancode_PageDown, "[6~" },
	{ scancode_F1, "OP" },
	{ scancode_Left, "[D" },
	{ scancode_Right, "[C" },
	{ scancode_Down, "[B" },
	{ scancode_Up, "[A" },
	{ 0, 0 }
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
		return -1;

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

	if (((modifiers & modifier_shift) ||
	     (modifiers & modifier_capslock)) &&
	    (modifiers & modifier_option))
	{
		c = symbols[scancode].shift_option;
	} else if ((modifiers & modifier_shift) || 
		   (modifiers & modifier_capslock))
	{
		c = symbols[scancode].shift;
		c = symbols[scancode].shift;
	} else if (modifiers & modifier_option)
	{
		c = symbols[scancode].option;
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
	int i;

	keyboard_get_scancode(&modifiers, &scancode);

	if (current_keytime != last_KeyTime)
	{
		last_KeyTime = current_keytime;

		c = keyboard_convert_scancode(modifiers, scancode);

		if (c == 0x1b)
		{
			for (i = 0; escape_strings[i].code; i++)
			{
				if (escape_strings[i].code == c)
				{
					buffer_put(0x1b);
					buffer_putstring(
						escape_strings[i].string);
					break;
				}
			}
		}
		else if (c != 0)
			buffer_put(c);
	}

	if (modifiers || (scancode < scancode_Last - 1))
		return 1;

	return 0;
}

int keyboard_keypressed(int timeout)
{
	long time = Ticks + timeout;

	while (Ticks < time)
	{
		if (keyboard_catch())
			return 1;
	}
	return 0;
}

int keyboard_getchar()
{
	keyboard_catch();

	return buffer_get();
}
