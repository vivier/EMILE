/*
 *
 * (c) 2004 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>

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

/* Constants for the Type field */

enum {
    kDriverTypeMacSCSI = 0x0001,
    kDriverTypeMacATA = 0x0701,
    kDriverTypeMacSCSIChained = 0xFFFF,
    kDriverTypeMacATAChained = 0xF8FF
};

struct DriverInfo {
	int32_t Block;
	int16_t Size;
	int16_t Type;
} __attribute__((packed));

#define DD_MAX_DRIVER	61

struct DriverDescriptor {
	int16_t	Sig;
	int16_t BlkSize;
	int32_t BlkCount;
	int16_t DevType;
	int16_t DevId;
	int32_t Data;
	int16_t DrvrCount;
	struct DriverInfo DrvInfo[DD_MAX_DRIVER];
	int8_t Pad[6];
} __attribute__((packed));
#define ASSERT_DD(a)   if ( sizeof(struct DriverDescriptor) != 512 ) { a }

/* Driver signatures, stored in the first four byte of pmPad. */

enum {
    kPatchDriverSignature = 0x70744452, /* 'ptDR', SCSI and ATA[PI] patch driver */
    kSCSIDriverSignature = 0x00010600,	/* SCSI hard disk driver */
    kATADriverSignature = 0x77696b69,	/*'wiki', ATA hard disk driver */
    kSCSICDDriverSignature = 0x43447672, /* 'CDvr', SCSI  CD-ROM driver */
    kATAPIDriverSignature = 0x41545049,	/* 'ATPI', ATAPI CD-ROM driver */
    kDriveSetupHFSSignature = 0x44535531,	/* 'DSU1', Drive Setup HFS partition */
};

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

#define DD_SIGNATURE	0x4552
#define MAP_SIGNATURE	0x504D

#define APPLE_PARTITION_MAP	"Apple_partition_map"
#define APPLE_DRIVER		"Apple_Driver"
#define APPLE_DRIVER43		"Apple_Driver43"
#define APPLE_MFS		"Apple_MFS"
#define APPLE_HFS		"Apple_HFS"
#define APPLE_UNIX_SVR2		"Apple_Unix_SVR2"
#define APPLE_PRODOS		"Apple_PRODOS"
#define APPLE_FREE		"Apple_Free"
#define APPLE_SCRATCH		"Apple_Scratch"
#define APPLE_DRIVER_ATA	"Apple_Driver_ATA"
#define APPLE_DRIVER_ATAPI	"Apple_Driver_ATAPI"
#define APPLE_DRIVER43_CD	"Apple_Driver43_CD"
#define APPLE_FWDRIVER		"Apple_FWDriver"
#define APPLE_VOID		"Apple_Void"
#define APPLE_PATCHES		"Apple_Patches" 

#define MAP_NAME_LEN	256
typedef struct {
	int fd;
	char name[MAP_NAME_LEN];
	int current;
	struct DriverDescriptor drivers;
	struct Partition partition;
} emile_map_t;
#endif
