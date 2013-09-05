/*
 *
 * (c) 2004,2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef  _EMILE_H
#define  _EMILE_H
static __attribute__((used)) char* emile_header = "$CVSHeader: $";
#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN

/* nothing to do, because m68k is big endian too */

static inline u_int16_t read_short(u_int16_t* addr)
{
	return *addr;
}

static inline void write_short(u_int16_t* addr, u_int16_t value)
{
	*addr = value;
}

static inline u_int32_t read_long(u_int32_t* addr)
{
	return *addr;
}

static inline void write_long(u_int32_t* addr, u_int32_t value)
{
	*addr = value;
}

#else /* __BYTE_ORDER == __LITTLE_ENDIAN */

/* little endian (or unknown), read byte by byte to get it in good order */

static inline u_int16_t read_short(u_int16_t* addr)
{
	unsigned char* baddr = (unsigned char*)addr;

	return ((u_int16_t)(*baddr) << 8) | (u_int16_t)*(baddr+1);
}

static inline void write_short(u_int16_t* addr, u_int16_t value)
{
	unsigned char* baddr = (unsigned char*)addr;

	*baddr = (unsigned char)(value>>8);
	*(baddr+1) = (unsigned char)value;
}

static inline u_int32_t read_long(u_int32_t* addr)
{
	u_int16_t* saddr = (u_int16_t*)addr;

	return ((u_int32_t)read_short(saddr) << 16) | 
		(u_int32_t)read_short(saddr+1);;
}

static inline void write_long(u_int32_t* addr, u_int32_t value)
{
	u_int16_t* saddr = (u_int16_t*)addr;

	write_short(saddr, (u_int16_t)(value>>16));
	write_short(saddr+1, (u_int16_t)value);
}
#endif
#endif
