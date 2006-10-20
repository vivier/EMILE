/*
 * 
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <macos/lowmem.h>
#include <macos/devices.h>
#include <macos/video.h>

#include "misc.h"

void turn_off_interrupts()
{
	int i;
	short count;
	DCtlHandle *currentHandle;
	DCtlPtr currentPtr;
	DriverHeader *driverPtr, **driverHandle;
	short refnum;
	volatile OSErr err; /* because -O2 remove call to PBControlSync() otherwise... */
	VDParamBlock pb;
	VDFlagRec flag;
	char name[256];
	int j;
	
	count = LMGetUnitTableEntryCount();
	currentHandle = (DCtlEntry ***) LMGetUTableBase();
	for (i = 0; i < count; i++)
	{
		if (!currentHandle[i])
			continue;
		currentPtr = *(currentHandle[i]);
		if (currentPtr->dCtlFlags & dRAMBasedMask)
		{
			driverHandle = (void*)(currentPtr->dCtlDriver);
			if (!driverHandle)
				continue;
			driverPtr = *driverHandle;
		}
		else
			driverPtr = (void*)(currentPtr->dCtlDriver);

		for(j = 0; j < driverPtr->drvrName[0]; j++)
			name[j] = driverPtr->drvrName[j + 1];
		name[j] = 0;
	
		err = OpenDriver(driverPtr->drvrName, &refnum);
		if (err != noErr)
			continue;

		if (strncmp(name, ".Display", 8) == 0)
		{
			pb.ioRefNum = refnum;
			pb.csCode = 7; /* SetInterrupt */
			flag.flag = 1;
			pb.csParam = &flag;

			err = PBControlSync((ParmBlkPtr) &pb);
		}
		DrvrRemove(refnum);
	}
}
