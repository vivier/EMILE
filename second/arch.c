#include "glue.h"
#include "arch.h"

unsigned long cpu_type = 0;
unsigned long mmu_type = 0;
unsigned long fpu_type = 0;
unsigned long machine_id = 0;
unsigned long arch_type = gestalt68k;

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

	/* check machine type */

	Gestalt(gestaltMachineType, &machine_id);
}
