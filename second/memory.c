/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * portion from penguin booter
 *
 */

#include <stdio.h>

#include "MMU.h"
#include "memory.h"
#include "lowmem.h"

/* Memory Allocation information */

#define MAX_MEMORY_AREA		4096

typedef struct memory_pool {
	memory_area_t area[MAX_MEMORY_AREA];
	unsigned long area_number;
} memory_pool_t;

memory_pool_t* pool = NULL;

static int memory_find_area_by_addr(unsigned long start)
{
	int i;

	for (i = 0; i < pool->area_number; i++)
	{
		if ( (pool->area[i].address <= start) &&
		     (start < pool->area[i].address + pool->area[i].size) )
		{
			return i;
		}
	}
	return -1;
}

static int memory_find_area_by_size(unsigned long size)
{
	int i;

	for (i = 0; i < pool->area_number; i++)
	{
		if (size <= pool->area[i].size)
		{
			return i;
		}
	}
	return -1;
}

static void memory_remove(unsigned long start, unsigned long end)
{
	int i;

	i = memory_find_area_by_addr(start);
	if (i == -1)
		return;

	/* zone to remove is at start of area */

	if (start == pool->area[i].address)
	{
		if (pool->area[i].size == end - start)
		{
			/* area can be removed */

			pool->area_number--;
			pool->area[i].address = 
					pool->area[pool->area_number].address;
			pool->area[i].size = pool->area[pool->area_number].size;
		}
		else
		{
			/* we guess end < address + size ... */

			pool->area[i].address = end;
			pool->area[i].size -= end - start;
		}

		return;
	}

	/* zone to remove is at end of area */

	if ( end == (pool->area[i].address + pool->area[i].size) )
	{
		/* we guess start >= address */

		pool->area[i].size -= end - start;

		return;
	}


	/* zone is in the middle of an area */

	/* split in two parts an existing area */

	/* second part : end -> original address + orignal size */

	pool->area[pool->area_number].address = end;
	pool->area[pool->area_number].size = pool->area[i].size - (end - start);

	pool->area_number++;

	/* first part : original address -> start */

	pool->area[i].size = start - pool->area[i].address;
}

static void memory_add(unsigned long start, unsigned long end)
{
	int i;

	for (i = 0; i < pool->area_number; i++)
	{
		/* can we add it at end of existing area */

		if ( start == (pool->area[i].address + pool->area[i].size) )
		{
			int j;

			pool->area[i].size += end - start;

			/* perhaps, now, we can merge with following area ? */

			for (j = 0; j < pool->area_number; j++)
			{
				if ( end == pool->area[j].address )
				{
					pool->area[i].size += pool->area[j].size;
					pool->area_number--;
					pool->area[j].address = pool->area[pool->area_number].address;
					pool->area[j].size = pool->area[pool->area_number].size;
					return;
				}
			}

			return;
		}

		/* can we add it at begin of existing area */

		if ( end == pool->area[i].address )
		{
			pool->area[i].address = start;
			pool->area[i].size += end - start;
			return;
		}
	}

	pool->area[pool->area_number].address = start;
	pool->area[pool->area_number].size = end;
	pool->area_number++;
}

void memory_init()
{
	extern char __bootloader_start;
	extern char __bootloader_end;

	/* we are currently using the MMU to have only one linear memory area */

	/* we put memory pool array at end of Memory */

	pool =  (memory_pool_t*) (MemTop - sizeof(memory_pool_t));

	pool->area_number = 0;

	/* add all memory to pool */

	memory_add(0, MemTop);

	/* remove the pool array */

	memory_remove(  (unsigned long)pool, 
			(unsigned long)pool + sizeof(memory_pool_t));

	/* remove the booloader image */

	memory_remove(  (unsigned long)&__bootloader_start, 
			(unsigned long)&__bootloader_end);

	/* remove the final address of the kernel */

	memory_remove(0x0000, 0x180000); /* FIXME: why 0x180000 ? */
}

void *malloc(size_t size)
{
	unsigned long addr;
	int area;

	size = ((size + 3) & ~0x3L) + 4; /* reserve 4 bytes to store size */

	area = memory_find_area_by_size(size);
	if (area == -1)
		return NULL;

	/* remove area from free memory pool */

	addr = pool->area[area].address;
	memory_remove(addr, addr + size);

	*(unsigned long*)addr = size;	/* store size of area in first word */

	return (void*)(addr + 4);
}

void free(void *ptr)
{
	ptr = ptr - 4;

	memory_add((unsigned long)ptr, (unsigned long)ptr + *(unsigned long*)ptr);
}

static void bank_add_mem(memory_map_t* map, unsigned long addr, unsigned long size)
{
	int i;
	int j;

	for (i = 0; i < map->bank_number; i++)
	{
		if (map->bank[i].address + map->bank[i].size == addr)
		{
			map->bank[i].size += size;

			/* can we merge 2 banks */

			for (j = 0; j < map->bank_number; j++)
			{
				if (map->bank[i].address + map->bank[i].size == map->bank[j].address)
				{
					map->bank[i].size += map->bank[j].size;

					/* remove bank */

					map->bank_number--;
					map->bank[j].address = map->bank[map->bank_number].address;
					map->bank[j].size = map->bank[map->bank_number].size;
				}
			}

			return;
		}
		else if (addr + size == map->bank[i].address)
		{
			map->bank[i].address = addr;
			map->bank[i].size += size;

			return;
		}
	}

	/* not found, create new bank */

	if (map->bank_number >= MAX_MEM_MAP_SIZE)
		return;

	map->bank[map->bank_number].address = addr;
	map->bank[map->bank_number].size = size;
	map->bank_number++;
}

void get_memory_map(memory_map_t* map)
{
	unsigned long logical;
	unsigned long physical;
	int ps = get_page_size();

	map->bank_number = 0;
	for (logical = 0; logical < MemTop; logical += ps)
	{
		if (logical2physical(logical, &physical) == 0)
		{
			bank_add_mem(map, physical, ps);
		}
	}
}
