/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef _PARTITION_H
#define _PARTITION_H
static __attribute__((used)) char* partition_header = "$CVSHeader$";

enum {
    kPartitionAUXIsValid= 0x00000001,
    kPartitionAUXIsAllocated  = 0x00000002,
    kPartitionAUXIsInUse= 0x00000004,
    kPartitionAUXIsBootValid  = 0x00000008,
    kPartitionAUXIsReadable   = 0x00000010,
    kPartitionAUXIsWriteable  = 0x00000020,
    kPartitionAUXIsBootCodePositionIndependent  = 0x00000040,
 
    kPartitionIsMountedAtStartup = 0x40000000,
    kPartitionIsStartup = 0x80000000,
 
    kPartitionIsChainCompatible  = 0x00000100,
    kPartitionIsRealDeviceDriver = 0x00000200,
    kPartitionCanChainToNext  = 0x00000400,
};

struct Block0 {
	int16_t	Sig;
	int16_t BlkSize;
	int32_t BlkCount;
	int16_t DevType;
	int16_t DevId;
	int32_t Data;
	int16_t DrvrCount;
	int32_t Block;
	int16_t Size;
	int16_t Type;
	int16_t Pad[243];
} __attribute__((packed));
#define ASSERT_B0(a)   if ( sizeof(struct Block0) != 512 ) { a }

struct Partition {
	int16_t	Sig;
	int16_t	SigPad;
	int32_t	MapBlkCnt;
	int32_t	PyPartStart;
	int32_t	PartBlkCnt;
	char	PartName[32];
	char	PartType[32];
	int32_t	LgDataStart;
	int32_t	DataCnt;
	int32_t	PartStatus;
	int32_t	LgBootStart;
	int32_t	BootSize;
	int32_t	BootAddr;
	int32_t	BootAddr2;
	int32_t	BootEntry;
	int32_t	BootEntry2;
	int32_t	BootCksum;
	char	Processor[16];
	int16_t	Pad[188];
} __attribute__((packed));

#define ASSERT_P(a)   if ( sizeof(struct Partition) != 512 ) { a }
#endif
