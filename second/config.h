/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include "head.h"

extern int read_config(emile_l2_header_t* info, char **kernel_path, char **command_line, char **ramdisk_path);
extern int read_config_vga(emile_l2_header_t* info);
extern int read_config_modem(emile_l2_header_t* info, int *bitrate, int *parity, int *datasize, int *stopbits);
extern int read_config_printer(emile_l2_header_t* info, int *bitrate, int *parity, int *datasize, int *stopbits);
