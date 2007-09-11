/*
 * 
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "head.h"

enum {
	SERIAL_MODEM_PORT = 0,
	SERIAL_PRINTER_PORT = 1,

	SERIAL_PORT_NB,
};

extern int serial_is_available(unsigned int port);
extern void serial_put(unsigned int port, char c);
#ifdef USE_CLI
extern int serial_keypressed(unsigned int port);
extern int serial_getchar(unsigned int port);
#endif
extern void serial_init(emile_l2_header_t* info);
