/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 *
 */

#include "vga.h"
#include "bank.h"
#include "bootx.h"

struct boot_infos bootx_infos;

extern unsigned long machine_id;
extern unsigned long arch_type;

void bootx_init(char* command_line, 
		   char* ramdisk_start, unsigned long ramdisk_size)
{
	int i;

	//bootx_infos.version = BOOT_INFO_VERSION;
	bootx_infos.version = 4;
	bootx_infos.compatible_version = BOOT_INFO_COMPATIBLE_VERSION;

	bootx_infos.logicalDisplayBase = (u_int8_t*)vga_get_video();
	bootx_infos.machineID = machine_id;
	bootx_infos.architecture = arch_type;

	bootx_infos.deviceTreeOffset = 0;
	bootx_infos.deviceTreeSize = 0;

	bootx_infos.dispDeviceRect[0] = 0;			/* left */
	bootx_infos.dispDeviceRect[1] = 0;			/* top */
	bootx_infos.dispDeviceRect[2] = vga_get_width();	/* right */
	bootx_infos.dispDeviceRect[3] = vga_get_height();	/* bottom */
	bootx_infos.dispDeviceDepth = vga_get_depth();
	bootx_infos.dispDeviceBase = (u_int8_t*)vga_get_videobase();
	bootx_infos.dispDeviceRowBytes = vga_get_row_bytes();
	bootx_infos.dispDeviceColorsOffset = 0;

	bootx_infos.dispDeviceRegEntryOffset = 0;

	bootx_infos.ramDisk = (u_int32_t)ramdisk_start;
	bootx_infos.ramDiskSize = ramdisk_size;

	bootx_infos.kernelParamsOffset = (long)command_line - (long)&bootx_infos;

	for (i = 0; i < memory_map.bank_number; i++)
        {
               bootx_infos.physMemoryMap[i].physAddr = memory_map.bank[i].physAddr;
               bootx_infos.physMemoryMap[i].size = memory_map.bank[i].size;
        }

	bootx_infos.physMemoryMapSize = i;

	bootx_infos.frameBufferSize = 0;
	//bootx_infos.totalParamsSize
}
