/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

/* physical memory information */

#define MAX_MEM_MAP_SIZE                26

typedef struct memory_bank {
        unsigned long physAddr;
        unsigned long logiAddr;
        unsigned long size;
} memory_bank_t;

typedef struct memory_map {
        memory_bank_t bank[MAX_MEM_MAP_SIZE];
        unsigned long bank_number;
} memory_map_t;

extern memory_map_t memory_map;

extern void init_memory_map();
extern unsigned long bank_mem_avail();
extern void bank_dump();
