/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

extern void MMU040_get_TC(unsigned long *TC);
extern void MMU040_get_SRP(unsigned long *SRP);
extern void MMU040_get_CRP(unsigned long *CRP);
extern void MMU040_get_ITT0(unsigned long *TT0);
extern void MMU040_get_DTT0(unsigned long *TT0);
extern void MMU040_get_ITT1(unsigned long *TT1);
extern void MMU040_get_DTT1(unsigned long *TT1);
extern unsigned long MMU040_read_phys(unsigned long addr);


extern unsigned long MMU040_get_page_size(void);
extern int MMU040_logical2physicalAttr(unsigned long logical, unsigned long *physicalAddr, unsigned long *attr);
extern int MMU040_logical2physical(unsigned long logical, unsigned long *physicalAddr);
