/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */
                                                                                
extern int load_image(unsigned long offset, unsigned long size, char *image);
extern int load_gzip(unsigned long offset, unsigned long size, char *image);
extern char* load_kernel(char* path, int bootstrap_size, unsigned long *base, unsigned long *entry, unsigned long *size);
