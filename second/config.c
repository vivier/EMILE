
#define __NO_INLINE__

#include <sys/types.h>
#include <stdio.h>
#include <config.h>
#if defined(USE_CLI) && defined(__LINUX__)
#include "console.h"
#include "cli.h"
#endif
#include "arch.h"

static char kernel[256];
static char ramdisk[256];

int read_config(emile_l2_header_t* info, 
		char **kernel_path, char **command_line, char **ramdisk_path)
{
	if (!EMILE_COMPAT(EMILE_05_SIGNATURE, info->signature))
	{
		printf("Bad header signature !\n");
		return -1;
	}

	if (info->gestaltID != 0)
	{
		machine_id = info->gestaltID;
		printf("User forces gestalt ID to %ld\n", machine_id);
	}

	sprintf(kernel, "block:(fd0)0x%x", info->kernel_image_offset);
	*kernel_path = kernel;
	if (info->ramdisk_size == 0)
		*ramdisk_path = NULL;
	else
	{
		sprintf(ramdisk, "block:(fd0)0x%x,0x%x",
				info->ramdisk_offset,
				info->ramdisk_size);
		*ramdisk_path = ramdisk;
	}

	printf("kernel %s\n", *kernel_path);
	if (*ramdisk)
		printf("ramdisk %s\n", *ramdisk_path);

	*command_line = info->command_line;

#if defined(USE_CLI) && defined(__LINUX__)
	printf("command ");
	console_cursor_save();
	printf("%s", *command_line);
	console_cursor_on();
	if (console_keypressed(5 * 60))
	{
		console_cursor_restore();
		cli_edit(*command_line, COMMAND_LINE_LENGTH);
	}
	console_cursor_off();
	putchar('\n');
#else
#ifdef __LINUX__
	printf("command %s\n", *command_line);
#endif
#endif
	return 0;
}
