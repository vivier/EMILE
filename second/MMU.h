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
unsigned long read_phys(void *addr);
void write_phys(void *addr, unsigned long value);

/* from MMU.c */

extern unsigned long get_page_size(void);
extern int logical2physicalAttr(unsigned long logical, unsigned long *physicalAddr, unsigned long *attr);
extern int logical2physical(unsigned long logical, unsigned long *physicalAddr);
