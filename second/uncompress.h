/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

typedef unsigned char (*uncompress_get_byte_t)(unsigned long offset);

extern unsigned long uncompress(char* buf, uncompress_get_byte_t feeder);
