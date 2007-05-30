/*
 * 
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "head.h"

extern int setserial(short refNum, unsigned int bitrate, unsigned int datasize,
                            int parity, int stopbits);

extern void serial_put(char c);
#ifdef USE_CLI
extern int serial_keypressed(void);
extern int serial_getchar(void);
#endif
extern void serial_init(emile_l2_header_t* info);
