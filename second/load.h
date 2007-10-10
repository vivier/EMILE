/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */
                                                                                
extern char* load_kernel(char* path, int bootstrap_size, unsigned long *base, unsigned long *entry, unsigned long *size);
extern char* load_ramdisk(char* path, unsigned long *ramdisk_size);
extern char* load_chainloader(char *path);
