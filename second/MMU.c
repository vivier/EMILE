/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include "MMU.h"

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
#define GET_TD_SF_NEXT(PD)	(PD & 0xFFFFFFFC)
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
#define GET_TD_LF_NEXT(PD0, PD1)	(PD1 & 0xFFFFFFFC)
#define GET_TD_LF_ADDR(PD0, PD1)	(PD1 & 0xFFFFFF00)

static int decode_8_PD(unsigned long *pageBase, unsigned long *pageMask,
		unsigned long *attr,
		unsigned long logicalAddr, unsigned long TI, 
		unsigned long PD0, unsigned long PD1);

static int decode_4_PD(unsigned long *pageBase, unsigned long *pageMask,
		unsigned long *attr,
		unsigned long logicalAddr, unsigned long TI, unsigned long PD)
{
	int dt;
	unsigned long TC;
	int TIA;
	unsigned long *root;
	int index;


	get_TC(&TC);

	TIA = GET_TC_TIA(TC);

	dt = GET_TD_SF_DT(PD);

	switch(dt)
	{
		case DT_INVALID:
			return -1;

		case DT_PAGE_DESCRIPTOR:
			*attr |= ((PD & 0xFF) >> 2);
			*pageBase = GET_TD_SF_ADDR(PD);
			return 0;

		case DT_VALID_4_BYTE:
			*attr |= ((PD & 0x0F) >> 2);
			index = logicalAddr >> (32 - TIA);
			logicalAddr = logicalAddr << TIA;
			*pageMask = (*pageMask) >> TIA;
			root = (unsigned long*)GET_TD_SF_NEXT(PD);

			return decode_4_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4,
					    read_phys(root + index));

		case DT_VALID_8_BYTE:
			*attr |= ((PD & 0x0F) >> 2);
			index = logicalAddr >> (32 - TIA);
			*pageMask = (*pageMask) >> TIA;
			root = (unsigned long*)GET_TD_SF_NEXT(PD);

			return decode_8_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4, 
					    read_phys(root + index),
					    read_phys(root + index + 1));
	}
	return 0;
}

static int decode_8_PD(unsigned long *pageBase, unsigned long *pageMask,
		unsigned long *attr,
		unsigned long logicalAddr, unsigned long TI, 
		unsigned long PD0, unsigned long PD1)
{
	int dt;
	unsigned long TC;
	int TIA;
	unsigned long *root;
	int index;


	get_TC(&TC);

	TIA = GET_TC_TIA(TC);

	dt = GET_TD_LF_DT(PD0, PD1);

	switch(dt)
	{
		case DT_INVALID:
			return -1;

		case DT_PAGE_DESCRIPTOR:
			*attr |= ((PD0 & 0xFFFF) >> 2);
			*pageBase = GET_TD_LF_ADDR(PD0, PD1);
			return 0;

		case DT_VALID_4_BYTE:
			*attr |= ((PD0 & 0xFFFF) >> 2);
			index = logicalAddr >> (32 - TIA);
			logicalAddr = logicalAddr << TIA;
			*pageMask = (*pageMask) >> TIA;
			root = (unsigned long*)GET_TD_LF_NEXT(PD0, PD1);

			return decode_4_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4,
					    read_phys(root + index));

		case DT_VALID_8_BYTE:
			*attr |= ((PD0 & 0xFFFF) >> 2);
			index = logicalAddr >> (32 - TIA);
			*pageMask = (*pageMask) >> TIA;
			root = (unsigned long*)GET_TD_LF_NEXT(PD0, PD1);

			return decode_8_PD( pageBase, pageMask, attr,
					    logicalAddr << TIA, TI << 4, 
					    read_phys(root + index),
					    read_phys(root + index + 1));
	}
	return 0;
}

int logical2physicalAttr(unsigned long logicalAddr, unsigned long *physicalAddr, unsigned long *attr)
{
	unsigned long TC;
	unsigned long CRP[2];
	unsigned long pageBase;
	unsigned long pageMask = 0xFFFFFFFF;
	int TIA;
	int max, min;
	int dt;
	unsigned long* root;
	int is;
	int index;
	int ret = -1;

	/* analyse CPU root pointer */

	get_CRP(CRP);

	dt = GET_RP_DT(CRP);
	GET_RP_LIMIT(CRP, max, min);

	/* analyse translation control register */

	get_TC(&TC);

	TIA = GET_TC_TIA(TC);
	is = GET_TC_IS(TC);

	index = (logicalAddr << is) >> (32 - TIA);
	pageMask = pageMask >> (is + TIA);

	if ( (index < min) || (index > max) )
		return -1;
	index = index - min;

	root = (unsigned long*)GET_RP_ADDR(CRP);

	*attr = 0;
	switch(dt)
	{
		case DT_INVALID:
		case DT_PAGE_DESCRIPTOR:
			ret = -1;
			break;

		case DT_VALID_4_BYTE:

			ret = decode_4_PD( &pageBase, &pageMask, attr,
					   logicalAddr << (is + TIA),
					   GET_TC_TI(TC) << 4,
					   read_phys(root + index));
			break;

		case DT_VALID_8_BYTE:

			ret = decode_8_PD( &pageBase, &pageMask, attr,
					   logicalAddr << (is + TIA),
					   GET_TC_TI(TC) << 4,
					   read_phys(root + index), 
					   read_phys(root + index + 1));
			break;
	}

	*physicalAddr = pageBase | (logicalAddr & pageMask);

	return ret;
}

int logical2physical(unsigned long logicalAddr, unsigned long *physicalAddr)
{
	unsigned long attr;

	return logical2physicalAttr(logicalAddr, physicalAddr, &attr);
}

unsigned long get_page_size(void)
{
	unsigned long TC;

	get_TC(&TC);

	return GET_TC_PAGE_SIZE(TC);
}
