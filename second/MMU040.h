/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

extern void get_TC040(unsigned long *TC);
extern void get_SRP040(unsigned long *SRP);
extern void get_CRP040(unsigned long *CRP);
extern void get_ITT0040(unsigned long *TT0);
extern void get_DTT0040(unsigned long *TT0);
extern void get_ITT1040(unsigned long *TT1);
extern void get_DTT1040(unsigned long *TT1);
extern unsigned long read_phys040(unsigned long addr);


extern unsigned long MMU040_get_page_size(void);
extern int MMU040_logical2physicalAttr(unsigned long logical, unsigned long *physicalAddr, unsigned long *attr);
extern int MMU040_logical2physical(unsigned long logical, unsigned long *physicalAddr);
