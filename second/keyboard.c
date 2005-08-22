/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "misc.h"
#include "glue.h"
#include "keyboard.h"

#define test_bit(n,m)	(((char*)(m))[(n) / 8] & (1L << (n % 8)))

enum keycode {
	keycode_command		= 0x37,
	keycode_shift		= 0x38,
	keycode_capslock	= 0x39,
	keycode_option		= 0x3A,
	keycode_control		= 0x3B,

	keycode_last		= 0x80
};

void keyboard_get_key(int *modifiers, int *code)
{
	KeyMap keyboard= {0,0,0,0};
	int keycode;

	GetKeys(keyboard);

	/* modifier keys */

	*modifiers = 0;
	if (test_bit(keycode_command, keyboard))
		*modifiers |= modifiers_command;
	if (test_bit(keycode_shift, keyboard))
		*modifiers |= modifiers_shift;
	if (test_bit(keycode_capslock, keyboard))
		*modifiers |= modifiers_capslock;
	if (test_bit(keycode_option, keyboard))
		*modifiers |= modifiers_option;
	if (test_bit(keycode_control, keyboard))
		*modifiers |= modifiers_control;

	/* other keys */

	for (keycode = 0; keycode < keycode_last; keycode++)
	{
		/* modifiers */

		if ( (keycode == keycode_command) ||
		     (keycode == keycode_shift) ||
		     (keycode == keycode_capslock) ||
		     (keycode == keycode_option) ||
		     (keycode == keycode_control) )
			continue;

		/* other keys */

		if (test_bit(keycode, keyboard))
			break;
	}
	*code = keycode;
}
