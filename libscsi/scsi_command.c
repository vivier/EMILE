/*
 *
 * (c) 2004, 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <sys/types.h>

#include <scsi/scsi.h>

#include <macos/types.h>
#include <macos/errors.h>
#include <macos/scsi.h>
#include <macos/lowmem.h>

#include "libscsi.h"

#define COMPLETION_TIMEOUT	300

#define SCSI_BUSY		(1 << 6)
#define SCSI_SEL		(1 << 1)

static inline int scsi_busy(void)
{
	return (SCSIStat() & (SCSI_BUSY | SCSI_SEL)) != 0;
}

static inline int scsi_wait_bus()
{
	int timeout;

	timeout = Ticks + 300;

	while (scsi_busy())
		if (Ticks > timeout)
			return scsiBusy;
	return noErr;
}

int scsi_command(int target, unsigned char* cdb, int count, TIB_t* tib)
{
	int err;
	short stat;
	short message;

	err = scsi_wait_bus();
	if (err != noErr)
	{
		printf("SCSI bus is busy\n");
		return err;
	}

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

	if (tib != NULL)
	{
		err = SCSIRead(tib);
		if ((err != scPhaseErr) && (err != noErr))
		{
			printf("Cannot read data (%d)\n", err);
			goto complete;
		}
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
