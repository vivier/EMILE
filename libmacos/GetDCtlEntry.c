/*
 *
 * (c) 2007 Laurent Vivier <Laurent@Vivier.EU>
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
	for (i = 0; i < count; i++)
	{
		if (!currentHandle[i])
			continue;
		currentPtr = *(currentHandle[i]);
		if (currentPtr->dCtlRefNum == refNum)
			return currentHandle[i];
	}
	return (DCtlHandle)0;
}
