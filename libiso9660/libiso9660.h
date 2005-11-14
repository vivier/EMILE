/*
 *
 * (c) 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __LIBISO9660_H__
#define __LIBISO9660_H__

#include <unistd.h>
#include <linux/iso_fs.h>

typedef void (*iso9660_read_t)(off_t offset, void* buffer, size_t size);

#define ISO9660_EXTENT_SIZE (2048)

typedef struct iso9660_DIR {
	int extent;
	int len;
	int index;
	unsigned char buffer[ISO9660_EXTENT_SIZE];
} iso9660_DIR;

typedef struct iso9660_FILE {
	int base;			/* first extent of the file */
	int size;			/* size of the file */
	int offset;
	int current;
	unsigned char buffer[ISO9660_EXTENT_SIZE];
} iso9660_FILE;

static inline int isonum_721(char *p)
{
        return ((p[0] & 0xff)
                | ((p[1] & 0xff) << 8));
}

static inline int isonum_723(char *p)
{
        return (isonum_721(p));
}

static inline int isonum_733(char *p)
{
	return ((p[0] & 0xff) | ((p[1] & 0xff) << 8) | 
		((p[2] & 0xff) << 16) | ((p[3] & 0xff) << 24));
}

extern int iso9660_mount(char* name);
extern void iso9660_name(char *buffer, struct iso_directory_record * idr);
extern void iso9660_umount(void);
extern struct iso_directory_record *iso9660_get_root_node(void);
extern iso9660_DIR* iso9660_opendir(char *name);
extern int iso9660_closedir(iso9660_DIR *dir);
extern struct iso_directory_record *iso9660_readdir(iso9660_DIR *dir);
extern int iso9660_is_directory(struct iso_directory_record * idr);
extern struct iso_directory_record* iso9660_get_node(struct iso_directory_record *dirnode, char *path);
extern iso9660_FILE* iso9660_open(char* pathname);
extern ssize_t iso9660_read(iso9660_FILE *file, void *buf, size_t count);
extern void iso9660_close(iso9660_FILE *file);
extern void iso9660_init(iso9660_read_t func);
extern int iso9660_fseek(iso9660_FILE *file, long offset, int whence);

#endif /* __LIBISO9660_H__ */
