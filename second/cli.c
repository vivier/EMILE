/*
 *
 * (c) 2008 Laurent Vivier <Laurent@lvivier.info>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libui.h>

#include "vga.h"
#include "console.h"
#include "arch.h"
#include "cli.h"

#define COMMAND_LINE_LENGTH     256

typedef int (*command_handler)(int argc, char **argv);

struct command_interface {
	char *name;
	command_handler handler;
	char *help;
};

static struct command_interface command[];
static int get_command(char *name);

static int do_loop;

static int exit_handler(int argc, char **argv)
{
	do_loop = 0;
	return 0;
}

static int help_handler(int argc, char **argv)
{
	int index;

	if (argc != 2)
	{
		printf("help needs one parameter\n");
		printf("Available commands are:\n");
		for (index = 0; command[index].name; index++)
		{
			printf("%s", command[index].name);
			if (command[index].help)
				printf("    %s", command[index].help);
			printf("\n");
		}
		return 1;
	}

	index = get_command(argv[1]);
	if (index == -1)
	{
		printf("Unknown command: %s\n", argv[1]);
		return 1;
	}

	if (command[index].help == NULL)
	{
		printf("No help available for %s\n", argv[1]);
		return 2;
	}

	printf("%s\n", command[index].help);

	return 0;
}

static int vga_handler(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("vga needs one parameter to set the display mode\n");
		return 1;
	}
	vga_init(argv[1]);
	return 0;
}

static int modem_handler(int argc, char **argv)
{
	return 0;
}

static int printer_handler(int argc, char **argv)
{
	return 0;
}

static int gestaltid_handler(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("gestaltID needs a parameter\n");
		return 1;
	}
	machine_id = strtol(argv[1], NULL, 0);

	printf("New gestalt ID is %ld\n", machine_id);

	return 0;
}

#define COMMAND_NB	7
static struct command_interface command[COMMAND_NB];
#define NEW_COMMAND(a,b,c) \
	do { \
		if (i < COMMAND_NB) { \
			command[i].name = a; \
			command[i].handler = b; \
			command[i].help = c; \
			i++; \
		} \
	} while(0)


static void cli_init(void)
{
	int i = 0;

	/* because of GOT */

	NEW_COMMAND("exit", exit_handler, "exit from interpreter");
	NEW_COMMAND("help", help_handler, "give help of commands");
	NEW_COMMAND("vga", vga_handler, "set display mode");
	NEW_COMMAND("modem", modem_handler,
	            "set modem serial port configuation");
	NEW_COMMAND("printer", printer_handler,
	            "set printer serial port configuration");
	NEW_COMMAND("gestaltID", gestaltid_handler, "set machine gestalt ID");
	NEW_COMMAND(NULL, NULL, NULL);
}

static int get_command(char *name)
{
	int i;

	for (i = 0; command[i].name; i++)
	{
		if (strcmp(name, command[i].name) == 0)
			return i;
	}

	return -1;
}

static int skip_blank(char *line, int i)
{
	while (line[i] == ' ' || line[i] == '\t')
		i++;
	return i;
}

static int skip_word(char *line, int i)
{
	while (line[i] != ' ' && line[i] != '\t' && line[i])
		i++;
	return i;
}

static int split_line(char *line, char ***argv)
{
	int i, j;
	int argc;
	char **words;

	/* how many words ? */

	argc = 0;
	for (i = skip_blank(line, 0); line[i]; i = skip_blank(line, i))
	{
		i = skip_word(line, i);
		argc++;
	}

	words = (char**)malloc(argc * sizeof(char*));

	/* where are the words ? */

	i = skip_blank(line, 0);
	j = 0;
	while(1)
	{
		words[j++] = line + i;
		i = skip_word(line, i);
		if (line[i] == 0)
			break;
		line[i++] = 0;
		i = skip_blank(line, i);
	}
	*argv = words;
	return argc;
}

void cli(void)
{
	char line[COMMAND_LINE_LENGTH];
	int argc;
	char **argv;
	int index;

	do_loop = 1;

	cli_init();
	console_clear();
	while(do_loop)
	{
		printf("emile> ");
		memset(line, 0, COMMAND_LINE_LENGTH);
		emile_edit(line, COMMAND_LINE_LENGTH);
		printf("\n");
		argc = split_line(line, &argv);
		if (argc > 0)
		{
			index = get_command(argv[0]);
			if (index == -1)
				printf("Unknown command: %s\n", argv[0]);
			else
			{
				command[index].handler(argc, argv);
				free(argv);
			}
		}
	}
	console_clear();
}
