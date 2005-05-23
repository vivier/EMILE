/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * some parts from BootX, (c) Benjamin Herrenschmidt
 *
 */

#include "misc.h"
#include "glue.h"
#include "arch.h"
#include "lowmem.h"
#ifdef ARCH_PPC
#include "bootx.h"
#endif

unsigned long cpu_type;
unsigned long mmu_type;
unsigned long fpu_type;
unsigned long machine_id;
unsigned long arch_type;
#ifdef ARCH_PPC
unsigned long bus_type;
#endif

struct older_macintosh_info {
	char *name;
	unsigned short ROMID;
	unsigned short SSW;
	unsigned long cpu_type;
	unsigned long mmu_type;
	unsigned long fpu_type;
	unsigned long machine_id;
	unsigned long arch_type;
};

struct older_macintosh_info older_macintosh[] = {
	{ "Macintosh SE/30", 0x0178, 0x0603, 
		gestalt68030, gestalt68030MMU, gestalt68882, 
		gestaltMacSE030, gestalt68k },
	{ "Macintosh Classic", 0x0276, 0x0607, 
		gestalt68000, gestaltNoMMU, gestaltNoFPU, 
		gestaltClassic, gestalt68k },
	{ NULL }
};

void arch_init()
{
	int i = 0;

	/* Some systems don't support Gestalt() */

	while (older_macintosh[i].name != NULL )
	{
		if (ROMBase[4] == older_macintosh[i].ROMID)
		{
			/* Doesn't support Gestalt(), nor SysEnvirons() */

			cpu_type = older_macintosh[i].cpu_type;
			mmu_type = older_macintosh[i].mmu_type;
			fpu_type = older_macintosh[i].fpu_type;
			machine_id = older_macintosh[i].machine_id;
			arch_type = older_macintosh[i].arch_type;

			return;
		}
		i++;
	}

	/* get processor type */

	Gestalt(gestaltProcessorType, &cpu_type);

	/* check FPU */

	if (Gestalt('FPUE', &fpu_type) == noErr)
		fpu_type = 0;
	else
		Gestalt(gestaltFPUType, &fpu_type);

	/* check MMU */

	Gestalt(gestaltMMUType, &mmu_type);

	/* I'v got a 'noMMU' with my 68030, not cool... */

	if (mmu_type == gestaltNoMMU)
	{
		switch(cpu_type)
		{
			case gestalt68030:
				mmu_type = gestalt68030MMU;
				break;
			case gestalt68040:
				mmu_type = gestalt68040MMU;
				break;
		}
	}

	/* get architecture type: powerPC or m68k */

	if (Gestalt(gestaltSysArchitecture, &arch_type) != noErr)
		arch_type = gestalt68k;

	/* check machine type */

	Gestalt(gestaltMachineType, &machine_id);

#ifdef ARCH_PPC
	/* check bus type */

	if (arch_type == gestaltPowerPC)
	{
		unsigned long response;

		/* OpenFirmware implies PCI */

		if ( (Gestalt('opfw', &response) == noErr) &&
		     (Gestalt('nreg', &response) == noErr) )
			bus_type = BOOT_ARCH_PCI;
		else
			bus_type = BOOT_ARCH_NUBUS;

		switch(machine_id)
		{
			case gestaltPowerMac6100_60:
			case gestaltPowerMac6100_66:
			case gestaltPowerMac6100_80:
			case gestaltPowerMac7100_66:
			case gestaltPowerMac7100_80:
			case gestaltPowerMac7100_80_chipped:
			case gestaltPowerMac8100_80:
			case gestaltPowerMac8100_100:
			case gestaltPowerMac8100_110:
			case gestaltPowerMac8100_120:
			case gestaltAWS9150_80:
			case gestaltAWS9150_120:
				bus_type |= BOOT_ARCH_NUBUS_PDM;
				break;
			case gestaltPowerMac5200:
			case gestaltPowerMac6200:
				bus_type |= BOOT_ARCH_NUBUS_PERFORMA;
				break;
			case gestaltPowerBook1400:
			case gestaltPowerBook5300:
			case gestaltPowerBookDuo2300:
				bus_type |= BOOT_ARCH_NUBUS_POWERBOOK;
				break;
		}
	}
#endif
}
