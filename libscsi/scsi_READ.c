/*
 *
 * (c) 2004, 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <sys/types.h>
#include <scsi/scsi.h>

#include <macos/types.h>
#include <macos/errors.h>

#include "libscsi.h"

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
	      void *buffer, int buffer_size)
{
	unsigned char	cdb[10];
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
