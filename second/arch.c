/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "glue.h"
#include "arch.h"

unsigned long cpu_type = 0;
unsigned long mmu_type = 0;
unsigned long fpu_type = 0;
unsigned long machine_id = 0;
unsigned long arch_type = gestalt68k;
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

	/* get architecture type */

	Gestalt(gestaltSysArchitecture, &arch_type);

	/* check machine type: powerPC or m68k */

	Gestalt(gestaltMachineType, &machine_id);

#if 0
	/* check bus type */

     if (Gestalt(gestaltOpenFirmwareInfo, &response) == noErr)
                if (Gestalt(gestaltNameRegistryVersion, &response) == noErr)
                        arch_pci = true;
#endif
}
