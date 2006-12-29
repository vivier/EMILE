/*
 * This file describes the structure passed from the BootX application
 * (for MacOS) when it is used to boot Linux.
 *
 * Written by Benjamin Herrenschmidt.
 * Modified by Laurent Vivier
 */


#ifndef __BOOTX_H__
#define __BOOTX_H__

#include "misc.h"

/* On kernel entry:
 *
 * r3 = 0x426f6f58    ('BooX')
 * r4 = pointer to boot_infos
 * r5 = NULL
 *
 * Data and instruction translation disabled, interrupts
 * disabled, kernel loaded at physical 0x00000000 on PCI
 * machines (will be different on NuBus).
 */

#define BOOT_INFO_VERSION               5
#define BOOT_INFO_COMPATIBLE_VERSION    1

/* Bit in the architecture flag mask. More to be defined in
   future versions. Note that either BOOT_ARCH_PCI or
   BOOT_ARCH_NUBUS is set. The other BOOT_ARCH_NUBUS_xxx are
   set additionally when BOOT_ARCH_NUBUS is set.
 */
#define BOOT_ARCH_PCI                   0x00000001UL
#define BOOT_ARCH_NUBUS                 0x00000002UL
#define BOOT_ARCH_NUBUS_CLASS		0x00000070UL
#define BOOT_ARCH_NUBUS_PDM             0x00000010UL
#define BOOT_ARCH_NUBUS_PERFORMA        0x00000020UL
#define BOOT_ARCH_NUBUS_POWERBOOK       0x00000040UL

/*  Maximum number of ranges in phys memory map */
#define MAX_MEM_MAP_SIZE				26

/* This is the format of an element in the physical memory map. Note that
   the map is optional and current BootX will only build it for pre-PCI
   machines */
typedef struct boot_info_map_entry
{
    u_int32_t       physAddr;                /* Physical starting address */
    u_int32_t       size;                    /* Size in bytes */
} boot_info_map_entry_t;


/* Here are the boot informations that are passed to the bootstrap
 * Note that the kernel arguments and the device tree are appended
 * at the end of this structure. */
typedef struct boot_infos
{
    /* Version of this structure */
    u_int32_t       version;
    /* backward compatible down to version: */
    u_int32_t       compatible_version;

    /* NEW (vers. 2) this holds the current _logical_ base addr of
       the frame buffer (for use by early boot message) */
    u_int8_t*       logicalDisplayBase;

    /* NEW (vers. 4) Apple's machine identification */
    u_int32_t       machineID;

    /* NEW (vers. 4) Detected hw architecture */
    u_int32_t       architecture;

    /* The device tree (internal addresses relative to the beginning of the tree,
     * device tree offset relative to the beginning of this structure).
     * On pre-PCI macintosh (BOOT_ARCH_PCI bit set to 0 in architecture), this
     * field is 0.
     */
    u_int32_t       deviceTreeOffset;        /* Device tree offset */
    u_int32_t       deviceTreeSize;          /* Size of the device tree */

    /* Some infos about the current MacOS display */
    u_int32_t       dispDeviceRect[4];       /* left,top,right,bottom */
    u_int32_t       dispDeviceDepth;         /* (8, 16 or 32) */
    u_int8_t*       dispDeviceBase;          /* base address (physical) */
    u_int32_t       dispDeviceRowBytes;      /* rowbytes (in bytes) */
    u_int32_t       dispDeviceColorsOffset;  /* Colormap (8 bits only) or 0 (*) */
    /* Optional offset in the registry to the current
     * MacOS display. (Can be 0 when not detected) */
     u_int32_t      dispDeviceRegEntryOffset;

    /* Optional pointer to boot ramdisk (offset from this structure) */
    u_int32_t       ramDisk;
    u_int32_t       ramDiskSize;             /* size of ramdisk image */

    /* Kernel command line arguments (offset from this structure) */
    u_int32_t       kernelParamsOffset;

    /* ALL BELOW NEW (vers. 4) */

    /* This defines the physical memory. Valid with BOOT_ARCH_NUBUS flag
       (non-PCI) only. On PCI, memory is contiguous and it's size is in the
       device-tree. */
    boot_info_map_entry_t
    	        physMemoryMap[MAX_MEM_MAP_SIZE]; /* Where the phys memory is */
    u_int32_t       physMemoryMapSize;               /* How many entries in map */


    /* The framebuffer size (optional, currently 0) */
    u_int32_t       frameBufferSize;         /* Represents a max size, can be 0. */

    /* NEW (vers. 5) */

    /* Total params size (args + colormap + device tree + ramdisk) */
    u_int32_t       totalParamsSize;

} boot_infos_t;

/* (*) The format of the colormap is 256 * 3 * 2 bytes. Each color index is represented
 * by 3 short words containing a 16 bits (unsigned) color component.
 * Later versions may contain the gamma table for direct-color devices here.
 */
#define BOOTX_COLORTABLE_SIZE    (256UL*3UL*2UL)

extern struct boot_infos bootx_infos;

extern void bootx_init(char* command_line, char* ramdisk_start, unsigned long ramdisk_size);
#endif
