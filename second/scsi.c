/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <malloc.h>
#include <scsi/scsi.h>
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

	cdb[0] = INQUIRY;
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

/******************************************************************************
+=====-========-========-========-========-========-========-========-========+
|  Bit|   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
|Byte |        |        |        |        |        |        |        |        |
|=====+=======================================================================|
| 0   |                           Operation code (28h)                        |
|-----+-----------------------------------------------------------------------|
| 1   |   Logical unit number    |   DPO  |   FUA  |     Reserved    | RelAdr |
|-----+-----------------------------------------------------------------------|
| 2   | (MSB)                                                                 |
|-----+---                                                                 ---|
| 3   |                                                                       |
|-----+---                        Logical block address                    ---|
| 4   |                                                                       |
|-----+---                                                                 ---|
| 5   |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 6   |                           Reserved                                    |
|-----+-----------------------------------------------------------------------|
| 7   | (MSB)                                                                 |
|-----+---                        Transfer length                             |
| 8   |                                                                 (LSB) |
|-----+-----------------------------------------------------------------------|
| 9   |                           Control                                     |
+=============================================================================+
******************************************************************************/

int scsi_READ(int target, unsigned long offset, unsigned short nb_blocks, 
	      char *buffer, int buffer_size)
{
	char	cdb[10];
	TIB_t	tib[2];

	cdb[0] = READ_10;
	cdb[1] = 0;
	cdb[2] = (offset >> 24) & 0xFF;
	cdb[3] = (offset >> 16) & 0xFF;
	cdb[4] = (offset >> 8) & 0xFF;
	cdb[5] = offset & 0xFF;
	cdb[6] = 0;
	cdb[7] = (nb_blocks >> 8) & 0xFF;
	cdb[8] = nb_blocks & 0xFF;
	cdb[9] = 0;

	tib[0].opcode = op_no_inc;
	tib[0].param1 = (int)buffer;
	tib[0].param2 = buffer_size;
	tib[1].opcode = op_stop;
	tib[1].param1 = 0;
	tib[1].param2 = 0;

	return scsi_command(target, cdb, 10, tib);
}
