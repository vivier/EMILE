/*
 * 
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

extern OSErr OpenDriver(ConstStr255Param name, short *drvrRefNum);
OSErr CloseDriver(short refNum);
extern ssize_t write(int fd, const void *buf, size_t count);
#ifdef USE_CLI
extern ssize_t read(int fd, void *buf, size_t count);
extern OSErr SerGetBuf(short refNum, long *count);
#endif
