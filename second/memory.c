/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * portion from penguin booter
 *
 */

#include <stdio.h>

#include "bank.h"
#include "memory.h"
#include "console.h"
#include "glue.h"

void *malloc(size_t size)
{
	return NewPtr(size);
}

void free(void *ptr)
{
	DisposePtr(ptr);
}
