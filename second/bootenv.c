/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <macos/lowmem.h>
#include <macos/gestalt.h>

#include "arch.h"
#include "vga.h"
#include "bank.h"
#include "bootenv.h"

static char* setenv(char* env, char* variable, unsigned long value)
{
	sprintf(env, "%s=%lu", variable, value);
	env += strlen(env) + 1;
	env[1] = 0;

	return env;
}

void bootenv_init(char* env)
{
	unsigned long videoaddr;
	env = setenv(env, "ROOT_SCSI_ID", 0);
	env = setenv(env, "SINGLE_USER", 0);
	logical2physical(vga_get_videobase(), &videoaddr);
	env = setenv(env, "VIDEO_ADDR", videoaddr);
	env = setenv(env, "ROW_BYTES", vga_get_row_bytes());
	env = setenv(env, "SCREEN_DEPTH", vga_get_depth());
	env = setenv(env, "DIMENSIONS", (vga_get_height() << 16) | vga_get_width());
	env = setenv(env, "BOOTTIME", Time - 2082844800);
	env = setenv(env, "GMTBIAS", gmt_bias);
	env = setenv(env, "BOOTERVER", 111);	/* "1.11.4a4" */
	env = setenv(env, "MACOS_VIDEO", vga_get_video());
	env = setenv(env, "MACOS_SCC", SCCRd);
	env = setenv(env, "MACHINEID", machine_id);
	env = setenv(env, "MEMSIZE", ram_size);
	env = setenv(env, "GRAYBARS", 0);
	env = setenv(env, "SERIALECHO", 0);
	env = setenv(env, "SERIALCONSOLE", 0);
	env = setenv(env, "PROCESSOR",	cpu_type - gestalt68020);
	env = setenv(env, "ROMBASE", (unsigned long)ROMBase);
	env = setenv(env, "TIMEDBRA", TimeDBRA);
	env = setenv(env, "ADBDELAY", TimeVIADB);
}
