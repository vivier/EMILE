/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>

#include "console.h"
#include "uncompress.h"
#include "misc.h"

extern char _kernel_start;
extern char _kernel_end;

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

static unsigned insize;  /* valid bytes in inbuf */
static unsigned inptr;   /* index of next byte to be processed in inbuf */
static unsigned outcnt = 0;  /* bytes in output buffer */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())

static uch *output_data;
static unsigned long output_ptr = 0;

#define Assert(cond,msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)

#define memzero(s, n)     memset ((s), 0, (n))

static int fill_inbuf(void)
{
        inbuf = &_kernel_start;
        insize = &_kernel_end - &_kernel_start;
        inptr = 1;
        return inbuf[0];
}

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

    console_put('.');
}

unsigned long uncompress(char* buf)
{
	output_data = buf;
	makecrc();
	printf("Uncompressing kernel to %p", buf);
	gunzip();

	return (unsigned long)output_data - (unsigned long)buf;
}
