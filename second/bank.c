/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * portion from penguin booter
 *
 */

#include "lowmem.h"
#include "MMU.h"
#include "bank.h"

static void bank_add_mem(memory_map_t* map, unsigned long logiAddr, 
			 unsigned long physAddr, unsigned long size)
{
	int i;
	int j;

	for (i = 0; i < map->bank_number; i++)
	{
		if ( (map->bank[i].physAddr <= physAddr) && 
		     (physAddr < map->bank[i].physAddr + map->bank[i].size) )
			return;	/* several logical address to one physical */

		if (map->bank[i].physAddr + map->bank[i].size == physAddr)
		{
			map->bank[i].size += size;

			/* can we merge 2 banks */

			for (j = 0; j < map->bank_number; j++)
			{
				if (map->bank[i].physAddr + map->bank[i].size == map->bank[j].physAddr)
				{
					map->bank[i].size += map->bank[j].size;

					/* remove bank */

					map->bank_number--;
					map->bank[j].physAddr = map->bank[map->bank_number].physAddr;
					map->bank[j].logiAddr = map->bank[map->bank_number].logiAddr;
					map->bank[j].size = map->bank[map->bank_number].size;
					return;
				}
			}

			return;
		}
		else if (physAddr + size == map->bank[i].physAddr)
		{
			map->bank[i].physAddr = physAddr;
			map->bank[i].logiAddr = logiAddr;
			map->bank[i].size += size;

			return;
		}
	}

	/* not found, create new bank */

	if (map->bank_number >= MAX_MEM_MAP_SIZE)
		return;

	map->bank[map->bank_number].physAddr = physAddr;
	map->bank[map->bank_number].logiAddr = logiAddr;
	map->bank[map->bank_number].size = size;
	map->bank_number++;
}

void get_memory_map(memory_map_t* map)
{
	unsigned long logical;
	unsigned long physical;
	int ps = get_page_size();

#ifdef DUMP_MEMMAP
	for (logical = 0; logical < MemTop ; logical += ps)
	{
		printf("%08lx->", logical);
		if (logical2physical(logical, &physical) == 0)
			printf("%08lx ", physical);
		else
			printf("INVALID! ");
	}
	while(1);
#endif
	map->bank_number = 0;
	for (logical = 0; logical < MemTop; logical += ps)
	{
		if (logical2physical(logical, &physical) == 0)
		{
			bank_add_mem(map, logical, physical, ps);
		}
	}
}
