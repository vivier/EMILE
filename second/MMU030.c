/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "console.h"

#include "MMU030.h"

#define GET_TC_ENABLE(TC)	(TC & 0x80000000)
#define GET_TC_SRE(TC)		(TC & 0x02000000)
#define GET_TC_FCL(TC)		(TC & 0x01000000)
#define GET_TC_IS(TC)		((TC & 0x000F0000) >> 16)
#define GET_TC_PAGE_SIZE(TC)	(1 << ((TC & 0x00F00000) >> 20))
#define GET_TC_TI(TC)		(TC & 0xFFFF)
#define GET_TC_TIA(TC)		((TC & 0xF000) >> 12)
#define GET_TC_TIB(TC)		((TC & 0x0F00) >> 8)
#define GET_TC_TIC(TC)		((TC & 0x00F0) >> 4)
#define GET_TC_TID(TC)		(TC & 0x000F)

#define RP_LIMIT_MAX		0x7FFF
#define GET_RP_LIMIT(CRP, max, min)	if (CRP[0] & 0x80000000)\
					{\
						min = (CRP[0] >> 16) & 0x7FFF;\
						max = RP_LIMIT_MAX;\
					} else {\
						min = 0;\
						max = (CRP[0] >> 16) & 0x7FFF;\
					}

#define GET_RP_DT(CRP)		(CRP[0] & 0x3)
#define GET_RP_ADDR(CRP)	(CRP[1] & 0xFFFFFFF0)

#define DT_INVALID		0
#define DT_PAGE_DESCRIPTOR	1
#define DT_VALID_4_BYTE		2
#define DT_VALID_8_BYTE		3

#define GET_TD_SF_DT(PD)	(PD & 0x3)
#define GET_TD_SF_WP(PD)	((PD >> 2) & 0x1)
#define GET_TD_SF_U(PD)		((PD >> 3) & 0x1)
#define GET_TD_SF_NEXT(PD)	(PD & 0xFFFFFFF0)
#define GET_TD_SF_ADDR(PD)	(PD & 0xFFFFFF00)

#define GET_TD_LF_LIMIT(PD0, PD1, max, min)	if (PD0 & 0x80000000)\
					{\
						min = (PD0 >> 16) & 0x7FFF;\
						max = RP_LIMIT_MAX;\
					} else {\
						min = 0;\
						max = (PD0 >> 16) & 0x7FFF;\
					}
#define GET_TD_LF_DT(PD0, PD1)		(PD0 & 3)
#define GET_TD_LF_WP(PD0, PD1)		((PD0 >> 2) & 1)
#define GET_TD_LF_U(PD0, PD1)		((PD0 >> 3) & 0x1)
#define GET_TD_LF_M(PD0, PD1)		((PD0 >> 4) & 0x1)
#define GET_TD_LF_CI(PD0, PD1)		((PD0 >> 6) & 0x1)
#define GET_TD_SF_S(PD0, PD1)		((PD0 >> 8) & 0x1)
#define GET_TD_LF_NEXT(PD0, PD1)	(PD1 & 0xFFFFFFF0)
#define GET_TD_LF_ADDR(PD0, PD1)	(PD1 & 0xFFFFFF00)

#define GET_TT_ENABLE(TT)		(TT & 0x8000)
#define GET_TT_BASE(TT)			( (TT >> 24) & 0xFF )
#define GET_TT_MASK(TT)			( (TT >> 16) & 0xFF )

#ifdef TRACE_MMU
#define TRACE(format, args...)	if (MMU_trace) printf(format, ##args)
static int MMU_trace = 0;
void MMU030_set_trace(int enable)
{
	MMU_trace = enable;
}
#else
#define TRACE(format, args...)
#endif

static int isTTSegment(unsigned long addr)
{
	unsigned long TT0;
	unsigned long TT1;
	unsigned long base;
	unsigned long mask;
	unsigned long size;

	addr >>= 24;

	MMU030_get_TT0(&TT0);

	if (GET_TT_ENABLE(TT0))
	{
		mask = GET_TT_MASK(TT0);
		base = GET_TT_BASE(TT0);

		base &= ~mask;
		addr &= ~mask;
		size = (mask << 24) || 0x00FFFFFF;	

		if ( (base <= addr) && (addr <= base + size) )
			return 1;
	}

	MMU030_get_TT1(&TT1);

	if (GET_TT_ENABLE(TT1))
	{
		mask = GET_TT_MASK(TT1);
		base = GET_TT_BASE(TT1);

		base &= ~mask;
		addr &= ~mask;
		size = (mask << 24) || 0x00FFFFFF;	

		if ( (base <= addr) && (addr <= base + size) )
			return 1;
	}

	/* if come here : no Transparent Translation */

	return 0;
}

static int decode_8_PD(unsigned long *pageBase, unsigned long *pageMask,
		unsigned long *attr,
		unsigned long logicalAddr, unsigned long TI, 
		unsigned long PD0, unsigned long PD1);

static int decode_4_PD(unsigned long *pageBase, unsigned long *pageMask,
		unsigned long *attr,
		unsigned long logicalAddr, unsigned long TI, unsigned long PD)
{
	int dt;
	int TIA;
	unsigned long root;
	int index;

	TRACE("PD: %08lx ", PD);

	TIA = GET_TC_TIA(TI);

	dt = GET_TD_SF_DT(PD);

	switch(dt)
	{
		case DT_INVALID:
			TRACE("INVALID\n");
			return -1;

		case DT_PAGE_DESCRIPTOR:
			TRACE("PAGE DESCRIPTOR\n");
			*attr |= ((PD & 0xFF) >> 2);
			*pageBase = GET_TD_SF_ADDR(PD);
			return 0;

		case DT_VALID_4_BYTE:
			*attr |= ((PD & 0x0F) >> 2);
			index = logicalAddr >> (32 - TIA);
			logicalAddr = logicalAddr << TIA;
			*pageMask = (*pageMask) >> TIA;
			root = GET_TD_SF_NEXT(PD);

			TRACE("4-BYTE TIA: %d index: %d\n", TIA, index);
			return decode_4_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4,
					    MMU030_read_phys(root + index * 4));

		case DT_VALID_8_BYTE:
			*attr |= ((PD & 0x0F) >> 2);
			index = logicalAddr >> (32 - TIA);
			*pageMask = (*pageMask) >> TIA;
			root = GET_TD_SF_NEXT(PD);

			TRACE("8-BYTE TIA: %d index: %d\n", TIA, index);
			return decode_8_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4, 
					    MMU030_read_phys(root + index * 8),
					    MMU030_read_phys(root + index * 8 + 4));
	}
	return 0;
}

static int decode_8_PD(unsigned long *pageBase, unsigned long *pageMask,
		unsigned long *attr,
		unsigned long logicalAddr, unsigned long TI, 
		unsigned long PD0, unsigned long PD1)
{
	int dt;
	int TIA;
	unsigned long root;
	int index;
	unsigned long min, max;

	TRACE("PD: %08lx%08lx ", PD0, PD1);

	TIA = GET_TC_TIA(TI);

	dt = GET_TD_LF_DT(PD0, PD1);

	switch(dt)
	{
		case DT_INVALID:
			TRACE("INVALID\n");
			return -1;

		case DT_PAGE_DESCRIPTOR:
			TRACE("PAGE DESCRIPTOR\n");
			*attr |= ((PD0 & 0xFFFF) >> 2);
			*pageBase = GET_TD_LF_ADDR(PD0, PD1);
			return 0;

		case DT_VALID_4_BYTE:
			*attr |= ((PD0 & 0xFFFF) >> 2);
			index = logicalAddr >> (32 - TIA);
			logicalAddr = logicalAddr << TIA;
			*pageMask = (*pageMask) >> TIA;
			root = GET_TD_LF_NEXT(PD0, PD1);
			TRACE("4-BYTE TIA: %d index: %d\n", TIA, index);

			GET_TD_LF_LIMIT(PD0, PD1, max, min);
			if ( (index < min) || (index > max) )
				return -1;

			return decode_4_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4,
					    MMU030_read_phys(root + index * 4));

		case DT_VALID_8_BYTE:
			*attr |= ((PD0 & 0xFFFF) >> 2);
			index = logicalAddr >> (32 - TIA);
			*pageMask = (*pageMask) >> TIA;
			root = GET_TD_LF_NEXT(PD0, PD1);
			TRACE("8-BYTE TIA: %d index: %d\n", TIA, index);

			GET_TD_LF_LIMIT(PD0, PD1, max, min);
			if ( (index < min) || (index > max) )
				return -1;

			return decode_8_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4, 
					    MMU030_read_phys(root + index * 8),
					    MMU030_read_phys(root + index * 8 + 4));
	}
	return 0;
}

int MMU030_logical2physicalAttr(unsigned long logicalAddr, unsigned long *physicalAddr, unsigned long *attr)
{
	unsigned long TC;
	unsigned long CRP[2];
	unsigned long pageBase;
	unsigned long pageMask = 0xFFFFFFFF;
	int TIA;
	int max, min;
	int dt;
	unsigned long root;
	int is;
	int index;
	int ret = -1;

	TRACE("logical: %08lx ", logicalAddr);

	*attr = 0;

	/* test if MMU is enabled */

	MMU030_get_TC(&TC);

	TRACE("TC: %08lx\n", TC);

	if (!GET_TC_ENABLE(TC) || isTTSegment(logicalAddr))
	{
		*physicalAddr = logicalAddr;
		return 0;
	}

	/* analyse CPU root pointer */

	MMU030_get_CRP(CRP);

	TRACE("CRP: %08lx%08lx ", CRP[0], CRP[1]);

	dt = GET_RP_DT(CRP);
	GET_RP_LIMIT(CRP, max, min);

	/* analyse translation control register */

	TIA = GET_TC_TIA(TC);
	is = GET_TC_IS(TC);

	index = (logicalAddr << is) >> (32 - TIA);
	pageMask = pageMask >> (is + TIA);

	if ( (index < min) || (index > max) )
		return -1;
	index = index - min;

	root = GET_RP_ADDR(CRP);

	switch(dt)
	{
		case DT_INVALID:
		case DT_PAGE_DESCRIPTOR:
			TRACE("INVALID");
			ret = -1;
			break;

		case DT_VALID_4_BYTE:

			TRACE("4-BYTE TIA: %d index: %d\n", TIA, index);
			ret = decode_4_PD( &pageBase, &pageMask, attr,
					   logicalAddr << (is + TIA),
					   GET_TC_TI(TC) << 4,
					   MMU030_read_phys(root + index * 4));
			break;

		case DT_VALID_8_BYTE:

			TRACE("8-BYTE TIA: %d index: %d\n", TIA, index);
			ret = decode_8_PD( &pageBase, &pageMask, attr,
					   logicalAddr << (is + TIA),
					   GET_TC_TI(TC) << 4,
					   MMU030_read_phys(root + index * 8), 
					   MMU030_read_phys(root + index * 8 + 4));
			break;
	}

	*physicalAddr = pageBase + (logicalAddr & pageMask);
	TRACE("Base: %08lx Mask: %08lx -> %08lx\n", 
	      pageBase, pageMask, *physicalAddr);

	return ret;
}

int MMU030_logical2physical(unsigned long logicalAddr, unsigned long *physicalAddr)
{
	unsigned long attr;

	return MMU030_logical2physicalAttr(logicalAddr, physicalAddr, &attr);
}

unsigned long MMU030_get_page_size(void)
{
	unsigned long TC;

	MMU030_get_TC(&TC);

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

	MMU030_get_TC(&TC);

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
				unsigned long PD = MMU030_read_phys(root + i * 4);

				dump_4_PD(shift + 4, PD);
			}
			break;

		case DT_VALID_8_BYTE:
			root = GET_TD_SF_NEXT(PD);
			for (i = 0; i < (1 << TIA); i++)
			{
				unsigned long PD0 = MMU030_read_phys(root + i * 8);
				unsigned long PD1 = MMU030_read_phys(root + i * 8 + 4);

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

	MMU030_get_TC(&TC);

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
				unsigned long PD = MMU030_read_phys(root + i * 4);

				dump_4_PD(shift + 4, PD);
			}
			break;

		case DT_VALID_8_BYTE:
			root = GET_TD_LF_NEXT(PD0, PD1);
			for (i = 0; i < max - min; i++)
			{
				unsigned long PD0 = MMU030_read_phys(root + i * 8);
				unsigned long PD1 = MMU030_read_phys(root + i * 8 + 4);

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

	MMU030_get_TC(&TC);

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

	MMU030_get_CRP(CRP);
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
				unsigned long PD = MMU030_read_phys(root + i * 4);

				printf("0x%08lx -> ", (unsigned long)(i + min) << (32 - TIA));
				dump_4_PD(0, PD);
			}
			break;

		case DT_VALID_8_BYTE:
			for (i = 0; i < max - min; i++)
			{
				unsigned long PD0 = MMU030_read_phys(root + i * 8);
				unsigned long PD1 = MMU030_read_phys(root + i * 8 + 4);

				printf("0x%08lx -> ", (unsigned long)(i + min) << (32 - TIA));
				dump_8_PD(0, PD0, PD1);
			}
			break;
	}
}
#endif /* MMU_DUMP */
