/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "console.h"
#include "uncompress.h"
#include "misc.h"

/*
 * gzip declarations
 */

#define OF(args)  args

typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;

#define WSIZE 0x8000            /* Window size must be at least 32k, */
                                /* and a power of two */

static long bytes_out = 0;

static uch *inbuf;           /* input buffer */
static uch window[WSIZE];    /* Sliding window buffer */

static unsigned inptr;   /* index of next byte to be processed in inbuf */
static unsigned outcnt = 0;  /* bytes in output buffer */

#define get_byte()  (inbuf[inptr++])

static uch *output_data;
static unsigned long output_ptr = 0;

#define Assert(cond,msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)

#define memzero(s, n)     memset ((s), 0, (n))

static void gzip_mark(void **ptr)
{
}

static void gzip_release(void **ptr)
{
}

static void flush_window(void);

#define STATIC
#include "inflate.c"

static void flush_window(void)
{
    ulg c = crc;         /* temporary variable */
    unsigned n;
    uch *in, *out, ch;
   
    in = window;
    out = output_data + output_ptr;
    for (n = 0; n < outcnt; n++) {
	ch = *out++ = *in++;
	c = crc_32_tab[((int)c ^ ch) & 0xff] ^ (c >> 8);
    }
    crc = c;
    bytes_out += (ulg)outcnt;
    output_ptr += (ulg)outcnt;
    outcnt = 0;

    putchar('.');
}

unsigned long uncompress(char* buf, char* image)
{
	output_data = buf;
        inbuf = (uch*)image;
        inptr = 0;

	makecrc();
	printf("Uncompressing kernel to %p", buf);
	gunzip();

	return output_ptr;
}
