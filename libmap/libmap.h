/*
 *
 * (c) 2004-2007 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#ifndef _LIBMAP_H
#define _LIBMAP_H

#include <sys/types.h>

#include "libstream.h"
#include "emile.h"

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

enum {
	kPatchMesh	= 0x6d657368,	/* 'mesh', fixes MESH bug */
	kPatchSCSI	= 0x73637369,	/* 'scsi' enable booting from CDROM */
	kPatchRuby	= 0x72756279,	/* 'ruby', volume larger than 2GB */
	kPatchSnag	= 0x736e6167,	/* 'snag', enable C key to boot CDROM */
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
#define APPLE_DRIVER_EMILE	"Apple_Driver_EMILE"

#define MAP_NAME_LEN	256
#define MAP_BLOCKSIZE	2048
typedef struct {
	device_io_t *device;
	int current;
	struct DriverDescriptor drivers;
	char filler1[MAP_BLOCKSIZE-512];
	struct Partition partition;
	char filler2[MAP_BLOCKSIZE-512];
} map_t;

enum {
	INVALID_BOOTBLOCK = -1,
	APPLE_BOOTBLOCK = 0,
	EMILE_BOOTBLOCK,
	UNKNOWN_BOOTBLOCK,
};

#define FLOPPY_SECTOR_SIZE      512
#define FIRST_LEVEL_SIZE        (FLOPPY_SECTOR_SIZE * 2)
#define BOOTBLOCK_SIZE          (FLOPPY_SECTOR_SIZE * 2)

extern int map_init(device_io_t *device, int partition);
extern map_t* map_open(device_io_t *device);
extern void map_close(map_t *map);
extern int map_get_number(map_t *map);
extern int map_read(map_t *map, int part);
extern int map_write(map_t *map, int part);
extern int map_partition_is_valid(map_t *map);
extern int map_get_partition_geometry(map_t *map, int *start, int *count);
extern char* map_get_partition_type(map_t *map);
extern char* map_get_partition_name(map_t *map);
extern int map_partition_is_bootable(map_t *map);
extern int map_partition_is_startup(map_t *map);
extern int map_set_partition_type(map_t *map, char* type);
extern int map_set_partition_name(map_t *map, char* name);
extern int map_partition_set_bootable(map_t *map, int enable);
extern int map_partition_set_startup(map_t *map, int enable);
extern int map_is_valid(map_t *map);
extern int map_partition_get_flags(map_t *map);
extern int map_partition_set_flags(map_t *map, int flags);
extern int map_geometry(map_t *map, int *block_size,
                              int *block_count);
extern int map_get_driver_number(map_t *map);
extern int map_get_driver_info(map_t *map, int number,
                              int *block, int *size, int* type);
extern int map_bootblock_read(map_t* map, char* bootblock);
extern int map_bootblock_write(map_t* map, char* bootblock);
extern int map_bootblock_get_type(char* bootblock);
extern int map_bootblock_is_valid(char *bootblock);
extern int map_set_startup(map_t *map, int partition);
extern int emile_is_apple_driver(map_t *map);
extern int map_has_apple_driver(map_t *map);
extern int map_seek_driver_partition(map_t *map, int start);
extern int map_get_bootinfo(map_t* map, int* bootstart, int *bootsize, int *bootaddr, int *bootentry, int* checksum, char* processor);
extern int map_set_bootinfo(map_t *map, int bootstart, int bootsize, int bootaddr, int bootentry, int checksum, char* processor);
extern int map_set_driver_info(map_t *map, int number, int block, int size, int type);
extern int map_set_driver_number(map_t *map, int number);
extern unsigned long map_get_driver_signature(map_t* map);
extern int map_block0_write(map_t *map);
extern int map_read_sector(map_t* map, off_t block, char *buffer, size_t nb);
extern int map_write_sector(map_t* map, off_t block, char *buffer, size_t nb);
extern int map_get_blocksize(map_t *map);
extern unsigned short map_checksum(unsigned char *addr, unsigned int length);
extern unsigned short map_checksum_ATA(unsigned char *addr, unsigned int length);
extern char *map_read_driver(map_t *map, int driver_number);
extern int map_write_driver(map_t *map, int driver_number, char *driver);
extern int map_update_checksum(map_t *map, int driver_number);
#endif
