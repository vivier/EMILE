/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * portion from penguin booter
 *
 */

#include <stdio.h>

#include "arch.h"
#include "lowmem.h"
#include "MMU030.h"
#include "MMU040.h"
#include "bank.h"

/* MacOS nanokernel data structures (nubus powerPC only)
 * found in Boot/X, thank you Ben ;-)
 */

#define MACOS_MEMMAP_PTR_ADDR		0x5FFFEFF0
#define MACOS_MEMMAP_SIZE_ADDR		0x5FFFEFF6
#define MACOS_MEMMAP_BANK_0FFSET	48

memory_map_t memory_map;

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

void m68k_init_memory_map()
{
	unsigned long logical;
	unsigned long physical;
	int ps;

	if (mmu_type == gestaltNoMMU)
	{
		bank_add_mem(0, 0, MemTop);
	}
	else if (mmu_type == gestalt68040MMU)
	{
		ps = MMU040_get_page_size();
		memory_map.bank_number = 0;
		logical = 0;
		for (logical = 0; logical < MemTop ; logical += ps)
		{
			if (MMU040_logical2physical(logical, &physical) == 0)
			{
				bank_add_mem(logical, physical, ps);
			}
		}
	}
	else
	{
		ps = MMU030_get_page_size();
		memory_map.bank_number = 0;
		logical = 0;
		for (logical = 0; logical < MemTop ; logical += ps)
		{
			if (MMU030_logical2physical(logical, &physical) == 0)
			{
				bank_add_mem(logical, physical, ps);
			}
		}
	}
}

void ppc_init_memory_map()
{
	/* Nubus powerPC */

	unsigned long *base = *(unsigned long**)MACOS_MEMMAP_PTR_ADDR;
	unsigned long len = *(unsigned short*)MACOS_MEMMAP_SIZE_ADDR;
	int i;

	if (len <= MACOS_MEMMAP_BANK_0FFSET)
		return;

	base += MACOS_MEMMAP_BANK_0FFSET;
	len -= MACOS_MEMMAP_BANK_0FFSET;
	i = 0;

	while(len >= 8)
	{
		unsigned long addr = *(unsigned long*)base;
		unsigned long size = *(unsigned long*)(base+4);

		if (size)
			bank_add_mem(addr, addr, size);

		base += 8;
		len -= 8;
	}
}

void init_memory_map()
{
	if (arch_type == gestaltPowerPC)
		ppc_init_memory_map();
	else
		m68k_init_memory_map();
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

int logical2physical(unsigned long logical, unsigned long *physical)
{
	if ( (mmu_type == gestaltNoMMU) || (mmu_type == gestaltEMMU1) )
	{
		*physical = logical;

		return 0;
	}
	else if (mmu_type == gestalt68040MMU)
	{
		return MMU040_logical2physical(logical, physical);
	}

	return MMU030_logical2physical(logical, physical);
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

#ifdef BANK_DUMP
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
}
#endif
