/*
 * 
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include <macos/lowmem.h>
#include <macos/devices.h>
#include <macos/video.h>
#include <macos/files.h>

#include "misc.h"
#include "driver.h"

#if 0
void list_drivers()
{
	int i;
	short count;
	DCtlHandle *currentHandle;
	DCtlPtr currentPtr;
	DriverHeader *driverPtr, **driverHandle;
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
	
		printf("%s\n", name);
		printf("    drvrFlags: %04x\n", driverPtr->drvrFlags);
		printf("    drvrDelay: %04x\n", driverPtr->drvrDelay);
		printf("    drvrEMask: %04x\n", driverPtr->drvrEMask);
		printf("    drvrMenu: %04x\n", driverPtr->drvrMenu);
		printf("    drvrOpen: %04x\n", driverPtr->drvrOpen);
		printf("    drvrPrime: %04x\n", driverPtr->drvrPrime);
		printf("    drvrCtl: %04x\n", driverPtr->drvrCtl);
		printf("    drvrStatus: %04x\n", driverPtr->drvrStatus);
		printf("    drvrClose: %04x\n", driverPtr->drvrClose);
	}
}
#endif

#if 0
void list_unit(void)
{
	short refnum;
	DCtlHandle currentHandle;
	DCtlPtr currentPtr;
	DriverHeader *driverPtr, **driverHandle;
	char name[256];
	int j;
	QHdrPtr    driveQHdr;
	DrvQElPtr    drivePtr;

	printf("Boot drive number: %d\n", LMGetBootDrive());

	driveQHdr = LMGetDrvQHdr();
	drivePtr = (DrvQEl *)driveQHdr->qHead;
	while (drivePtr)
	{
		printf("drive %d refnum %d\n", drivePtr->dQDrive, drivePtr->dQRefNum);
		refnum = drivePtr->dQRefNum;
		drivePtr = (DrvQEl *)drivePtr->qLink;

		currentHandle = GetDCtlEntry(refnum);
		if (!currentHandle)
			continue;
		currentPtr = *currentHandle;
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
	
	printf("%s\n", name);
	if ((refnum_to_scsi_id(refnum) >= 0) && (refnum_to_scsi_id(refnum) < 7))
		printf("SCSI target %d\n", refnum_to_scsi_id(refnum));
	}
}
#endif

signed short drive_to_refnum(int drive)
{
	QHdrPtr driveQHdr;
	DrvQElPtr drivePtr;

	driveQHdr = LMGetDrvQHdr();
	drivePtr = (DrvQEl *)driveQHdr->qHead;
	while (drivePtr)
	{
		if (drivePtr->dQDrive == drive)
			return drivePtr->dQRefNum;

		drivePtr = (DrvQEl *)drivePtr->qLink;
	}
	return -1;
}

int refnum_to_drive(signed short refnum)
{
	QHdrPtr driveQHdr;
	DrvQElPtr drivePtr;

	driveQHdr = LMGetDrvQHdr();
	drivePtr = (DrvQEl *)driveQHdr->qHead;
	while (drivePtr)
	{
		if (drivePtr->dQRefNum == refnum)
			return drivePtr->dQDrive;

		drivePtr = (DrvQEl *)drivePtr->qLink;
	}
	return -1;
}

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
