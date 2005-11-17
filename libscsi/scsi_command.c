/*
 *
 * (c) 2004, 2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>

#include <scsi/scsi.h>

#include <macos/types.h>
#include <macos/errors.h>

#include "libscsi.h"

#define COMPLETION_TIMEOUT	300

int scsi_command(int target, char* cdb, int count, TIB_t* tib)
{
	int err;
	short stat;
	short message;

	err = SCSIGet();
	if (err != noErr) 
	{
		printf("Cannot get SCSI bus (%d)\n", err);
		return err;
	}

	err = SCSISelect(target);
	if (err != noErr) 
	{
		printf("Cannot select target %d (%d)\n", target, err);
		return err;
	}

	err = SCSICmd(cdb, count);
	if (err != noErr) 
	{
		printf("Cannot send command (%d)\n", err);
		goto complete;
	}

	err = SCSIRead(tib);
	if (err != noErr) 
	{
		printf("Cannot read data (%d)\n", err);
		goto complete;
	}

complete:
	err = SCSIComplete(&stat, &message, COMPLETION_TIMEOUT);
	if (err != noErr) 
	{
		printf("Cannot complete transaction %d %d(%d)\n", 
							stat, message, err);
		return err;
	}

	return noErr;
}
