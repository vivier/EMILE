/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * a lot of parts from penguin booter
 */

#include <stdio.h>

#include "console.h"

#include "MMU040.h"

#define GET_TC_ENABLE(TC)	(TC & 0x8000)
#define IS_8K_PAGE(TC)		(TC & 0x4000)

#define GET_TC_PAGE_SIZE(TC)	(IS_8K_PAGE(TC) ? 8192 : 4096)


#define UDT_IS_INVALID(PDT)	(!(PDT & 2))
#define UDT_IS_RESIDENT(PDT)	(PDT & 2)

#define GET_RP_UDT(RP)		(RP & 0x00000003)
#define GET_RP_W(RP)		(RP & 0x00000004)
#define GET_RP_U(RP)		(RP & 0x00000008)
#define GET_RP_ADDR(RP)		(RP & 0xFFFFFE00)

#define GET_TD_4K_ADDR(TD)	(TD & 0xFFFFFF00)
#define GET_TD_8K_ADDR(TD)	(TD & 0xFFFFFF80)

#define TRACE_MMU
#ifdef TRACE_MMU
#define TRACE(format, args...)  if (MMU_trace) printf(format, ##args)
static int MMU_trace = 1;
void MMU040_set_trace(int enable)
{
	MMU_trace = enable;
}
#else
#define TRACE(format, args...)
#endif

int MMU040_logical2physicalAttr(unsigned long logicalAddr, unsigned long *physicalAddr, unsigned long *attr)
{
	int rootIndex;
	int ptrIndex;
	int pageIndex;
	unsigned long TC;
	unsigned long *rootTable, *ptrTable, *pageTable, *pageAddr;
	unsigned long pAttr;
	unsigned long rootEntry, tableEntry, pageEntry;

	TRACE("logical: %08lx ", logicalAddr);

	MMU040_get_TC(&TC);
	
	TRACE("TC: %08lx\n", TC);

	rootIndex = (logicalAddr & 0xFE000000) >> 25;
	ptrIndex  = (logicalAddr & 0x01FC0000) >> 18;
	pageIndex = IS_8K_PAGE(TC) ? (logicalAddr & 0x0003E000) >> 13 :
				     (logicalAddr & 0x0003F000) >> 12;

	TRACE("root idx: %d ptr idx: %d page idx: %d\n", rootIndex, ptrIndex,
							 pageIndex);

	MMU040_get_SRP((unsigned long*)&rootTable);
	TRACE("SRP: %p\n", rootTable);

	rootEntry = rootTable[rootIndex];
	if (UDT_IS_INVALID(rootEntry))
	{
		return -1;
	}

	TRACE("Root Entry: %08lx\n", rootEntry);
	ptrTable = (unsigned long*)GET_RP_ADDR(rootEntry);
	tableEntry = ptrTable[ptrIndex];
	if (UDT_IS_INVALID(tableEntry))
	{
		return -1;
	}
	TRACE("table Entry: %08lx\n", tableEntry);

	pageTable = (unsigned long*) ( IS_8K_PAGE(TC) ? 
				       GET_TD_8K_ADDR(tableEntry) :
				       GET_TD_4K_ADDR(tableEntry) );

	pageEntry = pageTable[pageIndex];
	if (IS_8K_PAGE(TC))
		pageAddr = (unsigned long *) (pageEntry & 0xFFFFE000);
	else
		pageAddr = (unsigned long *) (pageEntry & 0xFFFFF000);

	pAttr = pageEntry & 0x000004FF;

	*physicalAddr = (unsigned long)pageAddr;
	*attr = pAttr;

	TRACE("physical: %08lx\n", *physicalAddr);

	return 0;
}

int MMU040_logical2physical(unsigned long logicalAddr, unsigned long *physicalAddr)
{
	unsigned long attr;

	return MMU040_logical2physicalAttr(logicalAddr, physicalAddr, &attr);
}

unsigned long MMU040_get_page_size(void)
{
	unsigned long TC;

	MMU040_get_TC(&TC);

	return GET_TC_PAGE_SIZE(TC);
}
