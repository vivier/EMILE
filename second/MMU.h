/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

/* from MMU_asm.S */

void get_TC(unsigned long *TC);
void get_SRP(unsigned long *SRP);
void get_CRP(unsigned long *CRP);
void get_TT0(unsigned long *TT0);
void get_TT1(unsigned long *TT1);
int ptest(unsigned long logical, unsigned long* physical);
unsigned long read_phys(unsigned long addr);
void write_phys(void *addr, unsigned long value);

/* from MMU.c */

extern unsigned long get_page_size(void);
extern int logical2physicalAttr(unsigned long logical, unsigned long *physicalAddr, unsigned long *attr);
extern int logical2physical(unsigned long logical, unsigned long *physicalAddr);
#ifdef TRACE_MMU
extern void MMU_set_trace(int enable);
#endif
#ifdef MMU_DUMP
extern void dump_MMU_table();
#endif
