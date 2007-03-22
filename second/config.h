#include "head.h"

extern int read_config(emile_l2_header_t* info, char **kernel_path, char **command_line, char **ramdisk_path);
extern int read_config_vga(int8_t *conf);
extern int read_config_modem(int8_t *conf, int *bitrate, int *parity, int *datasize, int *stopbits);
extern int read_config_printer(int8_t *conf, int *bitrate, int *parity, int *datasize, int *stopbits);
