/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

typedef struct emile_config {
	FILE* fd;
	char* header;
	int header_size;
	char* current;
	int current_size;
} emile_config;

enum {
	CONFIG_END,
	CONFIG_PARTITION,
	CONFIG_FIRST_LEVEL,
	CONFIG_SECOND_LEVEL,
	CONFIG_TIMEOUT,
	CONFIG_DEFAULT,
	CONFIG_TITLE,
	CONFIG_KERNEL,
	CONFIG_ARGS,
	CONFIG_INITRD,
};

extern emile_config* emile_config_open(char* name);
extern void emile_config_close(emile_config* config);

extern int emile_config_read_first_entry(emile_config* config);
extern int emile_config_read_next(emile_config* config);

extern char* emile_config_get(emile_config* config, int tag);
