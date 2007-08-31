/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

extern char *config_read_word(char *line, char **next);
extern int config_get_next_property(char *configuration, int index, char *name, char *property);
extern int config_get_indexed_property(char *configuration, char *index_name, char *index_property, char *name, char *property);
extern int config_get_property(char *configuration, char *name, char *property);
extern int config_remove_property(char *configuration, char *name);
extern int config_set_property(char *configuration, char *name, char *property);
extern int config_set_indexed_property(char *configuration, char *index_name, char *index_property, char *name, char *property);
extern int config_remove_indexed_property(char *configuration, char *index_name, char *index_property, char *name);
extern int config_find_indexed_property(char *configuration, char *index_name, char *index_property, char *name, char *property);
extern int config_find_entry(char *configuration, char *name, char *property);
extern int config_add_property(char* configuration, char* name, char* property);
