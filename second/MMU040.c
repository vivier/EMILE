/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "console.h"

#include "MMU040.h"

#define GET_TC_PAGE_SIZE(TC)	(1 << ((TC & 0x00F00000) >> 20))

int MMU040_logical2physicalAttr(unsigned long logicalAddr, unsigned long *physicalAddr, unsigned long *attr)
{
	*physicalAddr = logicalAddr;
	*attr = 0;
	return 0;
}

int MMU040_logical2physical(unsigned long logicalAddr, unsigned long *physicalAddr)
{
	*physicalAddr = logicalAddr;
	return 0;
}

unsigned long MMU040_get_page_size(void)
{
	unsigned long TC;

	get_TC040(&TC);

	return GET_TC_PAGE_SIZE(TC);
}

#ifdef MMU_DUMP
static void dump_8_PD(int shift, unsigned long PD0, unsigned long PD1);

static void dump_4_PD(int shift, unsigned long PD)
{
	int dt;
	int i;
	int TIA;
	unsigned long TC;
	unsigned long root;

	if (shift > 8)
		printf("ERROR ! shift > 8 ");

	get_TC030(&TC);

	TIA = (GET_TC_TI(TC) >> (8 - shift)) & 0x000F;

	dt = GET_TD_SF_DT(PD);

	switch(dt)
	{
		case DT_INVALID:
			printf("INVALID!, ");
			break;
	
		case DT_PAGE_DESCRIPTOR:
			printf("0x%08lx (%d), ", GET_TD_SF_ADDR(PD), shift);
			break;

		case DT_VALID_4_BYTE:
			root = GET_TD_SF_NEXT(PD);
			for (i = 0; i < (1 << TIA); i++)
			{
				unsigned long PD = read_phys030(root + i * 4);

				dump_4_PD(shift + 4, PD);
			}
			break;

		case DT_VALID_8_BYTE:
			root = GET_TD_SF_NEXT(PD);
			for (i = 0; i < (1 << TIA); i++)
			{
				unsigned long PD0 = read_phys030(root + i * 8);
				unsigned long PD1 = read_phys030(root + i * 8 + 4);

				dump_8_PD(shift + 4, PD0, PD1);
			}
			break;

		default:
			printf("ERROR !! dt = %d ", dt);
			break;
	}
}

static void dump_8_PD(int shift, unsigned long PD0, unsigned long PD1)
{
	int dt;
	int max, min;
	int i;
	int TIA;
	unsigned long TC;
	unsigned long root;

	GET_TD_LF_LIMIT(PD0, PD1, max, min);

	if (shift > 8)
		printf("ERROR ! shift > 8 ");

	get_TC030(&TC);

	TIA = (GET_TC_TI(TC) >> (8 - shift)) & 0x000F;
	max = max > (1 << TIA) ? (1 << TIA) : max;
	if (max - min < 0)
		printf("ERROR ! max(%d) - min(%d) < 0, TIA = %d, ", max, min, TIA);

	dt = GET_TD_LF_DT(PD0, PD1);

	switch(dt)
	{
		case DT_INVALID:
			printf("INVALID!, ");
			break;
	
		case DT_PAGE_DESCRIPTOR:
			printf("0x%08lx (%d), ", GET_TD_LF_ADDR(PD0, PD1), shift);
			break;

		case DT_VALID_4_BYTE:
			root = GET_TD_LF_NEXT(PD0, PD1);
			for (i = 0; i < max - min; i++)
			{
				unsigned long PD = read_phys030(root + i * 4);

				dump_4_PD(shift + 4, PD);
			}
			break;

		case DT_VALID_8_BYTE:
			root = GET_TD_LF_NEXT(PD0, PD1);
			for (i = 0; i < max - min; i++)
			{
				unsigned long PD0 = read_phys030(root + i * 8);
				unsigned long PD1 = read_phys030(root + i * 8 + 4);

				dump_8_PD(shift + 4, PD0, PD1);
			}
			break;

		default:
			printf("ERROR !! dt = %d ", dt);
			break;
	}
}

void MMU030_dump_table()
{
	unsigned long root;
	unsigned long TC;
	unsigned long CRP[2];
	int max, min;
	int dt;
	int i;
	int TIA, TIB, TIC, TID;

	get_TC030(&TC);

	printf("TC: 0x%08lx\n", TC);
	if (GET_TC_ENABLE(TC))
		printf("    Enable\n");
	if (GET_TC_SRE(TC))
		printf("    Supervisor Root Pointer Enable\n");
	if (GET_TC_FCL(TC))
		printf("    Function Code lookup Enable\n");
	printf("Page Size: %d, Initial Shift: %ld\n", 
		GET_TC_PAGE_SIZE(TC), GET_TC_IS(TC));
	TIA = GET_TC_TIA(TC);
	TIB = GET_TC_TIB(TC);
	TIC = GET_TC_TIC(TC);
	TID = GET_TC_TID(TC);
	printf("TIA: %d TIB: %d TIC: %d TID: %d\n", TIA, TIB, TIC, TID);

	/* dump table */

	get_CRP030(CRP);
	printf("Root Pointer: 0x%08lx%08lx\n", CRP[0], CRP[1]);

	dt = GET_RP_DT(CRP);
	GET_RP_LIMIT(CRP, max, min);
	max = max > (1 << TIA) ? (1 << TIA) : max;

	root = GET_RP_ADDR(CRP);
	printf("SRP: 0x%08lx\n", root);
	switch(dt)
	{
		case DT_INVALID:
		case DT_PAGE_DESCRIPTOR:
			break;

		case DT_VALID_4_BYTE:
			for (i = 0; i < max - min; i++)
			{
				unsigned long PD = read_phys030(root + i * 4);

				printf("0x%08lx -> ", (unsigned long)(i + min) << (32 - TIA));
				dump_4_PD(0, PD);
			}
			break;

		case DT_VALID_8_BYTE:
			for (i = 0; i < max - min; i++)
			{
				unsigned long PD0 = read_phys030(root + i * 8);
				unsigned long PD1 = read_phys030(root + i * 8 + 4);

				printf("0x%08lx -> ", (unsigned long)(i + min) << (32 - TIA));
				dump_8_PD(0, PD0, PD1);
			}
			break;
	}
}
#endif /* MMU_DUMP */
