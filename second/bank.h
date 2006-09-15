/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <malloc.h>

#if defined(USE_MMU030) || defined(USE_MMU040)
#define USE_MMU
#endif

/* physical memory information */

#define MAX_MEM_MAP_SIZE                26
#define KERNEL_BASEADDR			0x3000

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
#ifdef USE_MMU
extern int logical2physical(unsigned long logical, unsigned long *physical);
extern int physical2logical(unsigned long physical, unsigned long *logical);
extern int check_full_in_bank(unsigned long start, unsigned long size);
extern void *malloc_contiguous(size_t size);
#else
#define malloc_contiguous malloc
#define check_full_in_bank(a,b) (1)
#endif
extern void *malloc_top(size_t size);
