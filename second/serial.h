/*
 * 
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

OSErr OpenDriver(ConstStr255Param name, short *drvrRefNum);
OSErr CloseDriver(short refNum);

extern int setserial(short refNum, unsigned int bitrate, unsigned int datasize,
                            int parity, int stopbits);
