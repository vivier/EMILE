#include "head.h"

extern int read_config(emile_l2_header_t* info, char **kernel_path, char **command_line, char **ramdisk_path);
extern int read_config_vga(char *conf);
extern int read_config_modem(char *conf, int *bitrate, int *parity, int *datasize, int *stopbits);
extern int read_config_printer(char *conf, int *bitrate, int *parity, int *datasize, int *stopbits);
