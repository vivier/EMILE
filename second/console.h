/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "misc.h"
#include "head.h"

extern void console_init(emile_l2_header_t* info);

extern void putstring(const char *s);

#endif
