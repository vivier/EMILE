/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN

/* nothing to do, because m68k is big endian too */

static inline unsigned short read_short(unsigned short* addr)
{
	return *addr;
}

static inline void write_short(unsigned short* addr, unsigned short value)
{
	*addr = value;
}

static inline unsigned long read_long(unsigned long* addr)
{
	return *addr;
}

static inline void write_long(unsigned long* addr, unsigned long value)
{
	*addr = value;
}

#else /* __BYTE_ORDER == __LITTLE_ENDIAN */

/* little endian (or unknown), read byte by byte to get it in good order */

static inline unsigned short read_short(unsigned short* addr)
{
	unsigned char* baddr = (unsigned char*)addr;

	return ((unsigned short)(*baddr) << 8) | (unsigned short)*(baddr+1);
}

static inline void write_short(unsigned short* addr, unsigned short value)
{
	unsigned char* baddr = (unsigned char*)addr;

	*baddr = (unsigned char)(value>>8);
	*(baddr+1) = (unsigned char)value;
}

static inline unsigned long read_long(unsigned long* addr)
{
	unsigned short* saddr = (unsigned short*)addr;

	return ((unsigned long)read_short(saddr) << 16) | 
		(unsigned long)read_short(saddr+1);;
}

static inline void write_long(unsigned long* addr, unsigned long value)
{
	unsigned short* saddr = (unsigned short*)addr;

	write_short(saddr, (unsigned short)(value>>16));
	write_short(saddr+1, (unsigned short)value);
}
#endif
