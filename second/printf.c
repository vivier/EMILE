/*-
 * Copyright (c) 1986, 1988, 1991, 1993
 *      The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this
software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this
software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      @(#)subr_prf.c  8.3 (Berkeley) 1/21/94
 *
 *
 *		Extracted vsprinf & adapted by BenH for MountX, Wed, June 3 1998.
 *		Added support for pascal strings.
 */

#include <stdarg.h>
#include <stdio.h>

#include "misc.h"
#include "console.h"

typedef unsigned long u_long;
typedef unsigned int u_int;
typedef unsigned char u_char;

#define NBBY    8

static char *ksprintn(u_long ul, int base, int *lenp, int prec);

int sprintf(char * s, const char * format, ...)
{
	va_list	params;
	int		r;
	
	va_start(params, format);
	r = vsprintf(s, format, params);
	va_end(params);
	
	return r;
}


/*
 * Scaled down version of sprintf(3).
 */
int
vsprintf(char *buf, const char *cfmt, va_list ap)
{
        register const char *fmt = cfmt;
        register char *p, *bp;
        register int ch, base;
        u_long ul;
        int lflag, tmp, width, pstring;
        int prec, precspec;
        char padc;

        for (bp = buf; ; ) {
                padc = ' ';
                width = 0;
                prec = 0;
                precspec = 0;
                pstring = 0;
                while ((ch = *(u_char *)fmt++) != '%')
                        if ((*bp++ = (char)ch) == '\0')
                                return ((bp - buf) - 1);

                lflag = 0;
reswitch:       switch (ch = *(u_char *)fmt++) {
                case '0':
                        padc = '0';
                        goto reswitch;
                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        for (width = 0;; ++fmt) {
                                width = width * 10 + ch - '0';
                                ch = *fmt;
                                if (ch < '0' || ch > '9')
                                        break;
                        }
                        goto reswitch;
                       
                case '#':
                		pstring = 1;
                		goto reswitch;
                case '*':
                        width = va_arg(ap, int);
                        goto reswitch;
                case '.':
                        precspec = 1;
                        if (*fmt == '*') {
                                prec = va_arg(ap, int);
                                ++fmt;
                                goto reswitch;
                        }
                        for (prec = 0;; ++fmt) {
                                ch = *fmt;
                                if (ch < '0' || ch > '9')
                                        break;
                                prec = prec * 10 + ch - '0';
                        }
                        goto reswitch;
                case 'l':
                        lflag = 1;
                        goto reswitch;
                /* case 'b': ... break; XXX */
                case 'c':
                        *bp++ = (char)va_arg(ap, int);
                        break;
                /* case 'r': ... break; XXX */
                case 's':
                        p = va_arg(ap, char *);
                        if (pstring)
                        {
                        	prec = precspec ? ((prec < *p) ? prec : *p) : *p;
                        	p++;
                        	precspec = 1;
                        }
                        while ((!precspec || (--prec >= 0)) && ((*bp = *p++) != 0))
                                ++bp;
                        break;
                case 'd':
                        ul = (unsigned long)(lflag ? va_arg(ap, long) : va_arg(ap, int));
                        if ((long)ul < 0) {
                                *bp++ = (char)'-';
                                ul = (unsigned long)(-(long)ul);
                        }
                        base = 10;
                        goto number;
                        break;
                case 'o':
                        ul = lflag ? va_arg(ap, u_long) : va_arg(ap,
u_int);
                        base = 8;
                        goto number;
                        break;
                case 'u':
                        ul = lflag ? va_arg(ap, u_long) : va_arg(ap,
u_int);
                        base = 10;
                        goto number;
                        break;
                case 'p':
                        *bp++ = '0';
                        *bp++ = 'x';
                        ul = (u_long)va_arg(ap, void *);
                        base = 16;
                        goto number;
                case 'x':
                case 'X':
                        ul = lflag ? va_arg(ap, u_long) : va_arg(ap,
u_int);
                        base = 16;
number:                 p = ksprintn(ul, base, &tmp, prec);
                        if (width && (width -= tmp) > 0)
                                while (width--)
                                        *bp++ = padc;
                        while ((ch = *p--) != 0)
                                *bp++ = (char)ch;
                        break;
                default:
                        *bp++ = '%';
                        if (lflag)
                                *bp++ = 'l';
                        /* FALLTHROUGH */
                case '%':
                        *bp++ = (char)ch;
                }
        }
}

/*
 * Put a number (base <= 16) in a buffer in reverse order; return an
 * optional length and a pointer to the NULL terminated (preceded?)
 * buffer.
 */
static char *
ksprintn(u_long ul, int base, int *lenp, int prec)
{                                       /* A long in base 8, plus NULL. */
        static char buf[sizeof(long) * NBBY / 3 + 4];
        register char *p;
		int i;
        p = buf;
        for(i=0;i<sizeof(buf);i++)
     		buf[i] = 0;
        do {
                *++p = "0123456789abcdef"[ul % base];
                ul /= base;
        } while ((--prec > 0 || ul != 0) && p < buf + sizeof(buf) - 1);
        if (lenp)
                *lenp = p - buf;
        return (p);
}


static char __printf_buffer[2048];

int
printf(const char * format, ...)
{
	va_list				args;
	int					len;

	va_start(args, format);
	len = vsprintf(__printf_buffer, format, args);
	va_end(args);

	if (len)
		console_putstring(__printf_buffer);

	return len;		
}

#if __GNUC__==3
int puts(const char * s)
{
	console_putstring(s);
	console_putchar('\n');

	return -1;
}

int putchar (int c)
{
	return console_putchar(c);
}
#endif
