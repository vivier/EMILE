/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 * some parts from BootX, (c) Benjamin Herrenschmidt
 *
 */

#include "glue.h"
#include "arch.h"

unsigned long cpu_type = 0;
unsigned long mmu_type = 0;
unsigned long fpu_type = 0;
unsigned long machine_id = 0;
unsigned long arch_type = 0;
unsigned long bus_type = 0;

void arch_init()
{
	/* get processor type */

	Gestalt(gestaltProcessorType, &cpu_type);

	/* check FPU */

	if (Gestalt('FPUE', &fpu_type) == noErr)
		fpu_type = 0;
	else
		Gestalt(gestaltFPUType, &fpu_type);

	/* check MMU */

	Gestalt(gestaltMMUType, &mmu_type);

	/* get architecture type: powerPC or m68k */

	if (Gestalt(gestaltSysArchitecture, &arch_type) != noErr)
		arch_type = gestalt68k;

	/* check machine type */

	Gestalt(gestaltMachineType, &machine_id);

	/* check bus type */

	if (arch_type == gestalt68k)
	{
		bus_type = busNUBUS;
	}
	else
	{ 
		unsigned long response;

		/* OpenFirmware implies PCI */

		if ( (Gestalt('opfw', &response) == noErr) &&
		     (Gestalt('nreg', &response) == noErr) )
			bus_type = busPCI;
		else
			bus_type = busNUBUS;

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
				bus_type |= busPDM;
				break;
			case gestaltPowerMac5200:
			case gestaltPowerMac6200:
				bus_type |= busPERFORMA;
				break;
			case gestaltPowerBook1400:
			case gestaltPowerBook5300:
			case gestaltPowerBookDuo2300:
				bus_type |= busPOWERBOOK;
				break;
		}
	}
}
