/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * portion from penguin booter
 *
 */


/* physical memory information */

#define MAX_MEM_MAP_SIZE                26

typedef struct memory_area {
        unsigned long address;
        unsigned long size;
} memory_area_t;

typedef struct memory_map {
        memory_area_t bank[MAX_MEM_MAP_SIZE];
        unsigned long bank_number;
} memory_map_t;

extern void get_memory_map(memory_map_t* map);
