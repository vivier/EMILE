/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "glue.h"
#include "head.h"

OSErr OpenDriver(ConstStr255Param name, short *drvrRefNum);
OSErr CloseDriver(short refNum);

extern int setserial(short refNum, unsigned int bitrate, unsigned int datasize,
                            int parity, int stopbits);

extern void serial_put(char c);
extern void serial_init(emile_l2_header_t* info);
