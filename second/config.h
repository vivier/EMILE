/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "head.h"

typedef struct emile_config {
	char *kernel;
	char *command_line;
	char *initrd;
	char *chainloader;
} emile_config_t;

extern int read_config(emile_l2_header_t* info, emile_config_t *config);
