/*
 *
 * (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>

extern char *config_read_word(char *line, char **next);
extern int config_get_next_property(int8_t *configuration, int index, char *name, char *property);
extern int config_get_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name, char *property);
extern int config_get_property(int8_t *configuration, char *name, char *property);
extern int config_remove_property(int8_t *configuration, char *name);
extern int config_set_property(int8_t *configuration, char *name, char *property);
extern int config_set_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name, char *property);
extern int config_remove_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name);
extern int config_find_indexed_property(int8_t *configuration, char *index_name, char *index_property, char *name, char *property);
extern int config_find_entry(int8_t *configuration, char *name, char *property);
extern int config_add_property(int8_t* configuration, char* name, char* property);
