/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

extern void MMU030_get_TC(unsigned long *TC);
extern void MMU030_get_SRP(unsigned long *SRP);
extern void MMU030_get_CRP(unsigned long *CRP);
extern void MMU030_get_TT0(unsigned long *TT0);
extern void MMU030_get_TT1(unsigned long *TT1);
extern int MMU030_ptest(unsigned long logical, unsigned long* physical);
extern unsigned long MMU030_read_phys(unsigned long addr);
extern void MMU030_write_phys(void *addr, unsigned long value);


extern unsigned long MMU030_get_page_size(void);
extern int MMU030_logical2physicalAttr(unsigned long logical, unsigned long *physicalAddr, unsigned long *attr);
extern int MMU030_logical2physical(unsigned long logical, unsigned long *physicalAddr);
#ifdef TRACE_MMU
extern void MMU030_set_trace(int enable);
#endif
#ifdef MMU_DUMP
extern void MMU030_dump_table();
#endif
