/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include "bank.h"
#include "misc.h"
#include "glue.h"

#define COMPLETION_TIMEOUT	300

static int scsi_command(int target, char* cdb, int count, TIB_t* tib)
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

int scsi_INQUIRY(int target, char *buffer, size_t count)
{
	char	cdb[6];
	TIB_t	tib[2];

	cdb[0] = 0x12;
	cdb[1] = 0;
	cdb[2] = 0;
	cdb[3] = 0;
	cdb[4] = count;
	cdb[5] = 0;

	tib[0].opcode = op_no_inc;
	tib[0].param1 = (int)buffer;
	tib[0].param2 = count;
	tib[1].opcode = op_stop;
	tib[1].param1 = 0;
	tib[1].param2 = 0;

	return scsi_command(target, cdb, 6, tib);
}

#if 0
	char cdb[6] = {
		0x08,	/* READ */
		(offset >> 16) & 0x1F,
		(offset >> 8) & 0xFF,
		offset & 0xFF,
		(size + 511) / 512,	/* FIXME: blocksize and max is 255 */
		0 };
#endif
