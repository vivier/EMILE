/*
 *
 * (c) 2006 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include <sys/types.h>
#include <stdio.h>

#include <scsi/scsi.h>

#include <macos/types.h>
#include <macos/errors.h>

#include "libscsi.h"

int scsi_TEST_UNIT_READY(int target)
{
	unsigned char	cdb[6];

	cdb[0] = TEST_UNIT_READY;
	cdb[1] = 0;
	cdb[2] = 0;
	cdb[3] = 0;
	cdb[4] = 0;
	cdb[5] = 0;

	return scsi_command(target, cdb, 6, NULL);
}
