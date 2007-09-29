/*
 *
 * (c) 2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>

#include <macos/lowmem.h>
#include <macos/devices.h>

DCtlHandle GetDCtlEntry(short refNum)
{
	DCtlHandle *currentHandle;
	DCtlPtr currentPtr;
	short count;
	int i;

	count = LMGetUnitTableEntryCount();
	currentHandle = (DCtlEntry ***) LMGetUTableBase();
printf("\ndctlref ");
	for (i = 0; i < count; i++)
	{
		if (!currentHandle[i])
			continue;
		currentPtr = *(currentHandle[i]);
printf("%d \n", currentPtr->dCtlRefNum);
		if (currentPtr->dCtlRefNum == refNum)
			return currentHandle[i];
	}
printf("\n");
	return (DCtlHandle)0;
}
