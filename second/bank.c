/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * portion from penguin booter
 *
 */

#include <stdio.h>

#include "lowmem.h"
#include "MMU.h"
#include "bank.h"

memory_map_t memory_map = { { { 0, 0 } }, 0 };

static void bank_add_mem(unsigned long logiAddr, 
			 unsigned long physAddr, unsigned long size)
{
	int i;
	int j;

	for (i = 0; i < memory_map.bank_number; i++)
	{
		if ( (memory_map.bank[i].physAddr + 
				memory_map.bank[i].size == physAddr) && 
		     (memory_map.bank[i].logiAddr + 
				memory_map.bank[i].size == logiAddr) )
		{
			memory_map.bank[i].size += size;

			/* can we merge 2 banks */

			for (j = 0; j < memory_map.bank_number; j++)
			{
				if ( (memory_map.bank[i].physAddr + 
					memory_map.bank[i].size == 
						memory_map.bank[j].physAddr) &&
				     (memory_map.bank[i].logiAddr + 
					memory_map.bank[i].size == 
						memory_map.bank[j].logiAddr) )
				{
					memory_map.bank[i].size += memory_map.bank[j].size;

					/* remove bank */

					memory_map.bank_number--;
					memory_map.bank[j].physAddr = memory_map.bank[memory_map.bank_number].physAddr;
					memory_map.bank[j].logiAddr = memory_map.bank[memory_map.bank_number].logiAddr;
					memory_map.bank[j].size = memory_map.bank[memory_map.bank_number].size;
					return;
				}
			}

			return;
		}
		else if ( (physAddr + size == memory_map.bank[i].physAddr) && 
			  (logiAddr + size == memory_map.bank[i].logiAddr) )
		{
			memory_map.bank[i].physAddr = physAddr;
			memory_map.bank[i].logiAddr = logiAddr;
			memory_map.bank[i].size += size;

			return;
		}
	}

	/* not found, create new bank */

	if (memory_map.bank_number >= MAX_MEM_MAP_SIZE)
		return;

	memory_map.bank[memory_map.bank_number].physAddr = physAddr;
	memory_map.bank[memory_map.bank_number].logiAddr = logiAddr;
	memory_map.bank[memory_map.bank_number].size = size;
	memory_map.bank_number++;
}

void init_memory_map()
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
	memory_map.bank_number = 0;
	logical = 0;
	for (logical = 0; logical < MemTop ; logical += ps)
	{
		if (logical2physical(logical, &physical) == 0)
		{
			bank_add_mem(logical, physical, ps);
		}
	}
}

static int bank_find_by_physical(unsigned long physical)
{
	int i;

	for (i = 0; i < memory_map.bank_number; i++)
	{
		if ( (memory_map.bank[i].physAddr <= physical) &&
		     ( physical < memory_map.bank[i].physAddr + memory_map.bank[i].size) )
			return i;
	}

	return -1;
}

int physical2logical(unsigned long physical, unsigned long *logical)
{
	int bank;

	bank = bank_find_by_physical(physical);

	if (bank == -1)
		return -1;

	if (memory_map.bank[bank].physAddr > memory_map.bank[bank].logiAddr)
		*logical = physical - (memory_map.bank[bank].physAddr - 
					memory_map.bank[bank].logiAddr);
	else
		*logical = physical + (memory_map.bank[bank].logiAddr - 
					memory_map.bank[bank].physAddr);

	return 0;
}

unsigned long bank_mem_avail()
{
	int i;
	unsigned long size = 0;

	for (i = 0; i < memory_map.bank_number; i++)
	{
		size += memory_map.bank[i].size;
	}
	return size;
}

void bank_dump()
{
	int i;
	unsigned long size = 0;

	printf("Physical memory map:\n");
	for (i = 0; i < memory_map.bank_number; i++)
	{
		printf("%d: 0x%08lx -> 0x%08lx mapped at 0x%08lx -> 0x%08lx\n",
		i,
		memory_map.bank[i].physAddr,
		memory_map.bank[i].physAddr + memory_map.bank[i].size,
		memory_map.bank[i].logiAddr,
		memory_map.bank[i].logiAddr + memory_map.bank[i].size);
		size += memory_map.bank[i].size;
	}
	printf("Available Memory: %ld kB\n", size / 1024);
}
