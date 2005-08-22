/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */


enum modifiers_mask {
	modifiers_command	= 0x01,
	modifiers_shift		= 0x02,
	modifiers_capslock	= 0x04,
	modifiers_option	= 0x08,
	modifiers_control	= 0x10,
};

extern void keyboard_get_key(int *modifiers, int *code);
