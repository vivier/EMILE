static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "libemile.h"

int emile_get_uncompressed_size(char *file)
{
	int ret;
	int i;
	int tube[2];
	char buffer[1024];
	char *uncompressed;

	ret = pipe(tube);
	if (ret == -1)
		return -1;

	if (fork() == 0)
	{
		/* child */

		close(tube[0]);

		close(STDOUT_FILENO);
		dup(tube[1]);
		close(STDERR_FILENO);

		execlp("gzip", "gzip", "-l", file, NULL);

		/* never comes here ... */

		exit(1);
	}

	/* parent */

	close(tube[1]);

	ret = read(tube[0], buffer, 1024);
	if (ret <= 0)
		return -1;

	/* skip first line */

	i = 0;
	while ( (buffer[i] != '\n') && (i < 1024))
		i++;
	i++;	/* skip '\n' */

	/* skip space */

	while ( (buffer[i] == ' ') && (i < 1024))
		i++;

	/* skip compressed size */

	while ( (buffer[i] >= '0') && buffer[i] <= '9' && (i < 1024))
		i++;
	buffer[i++] = 0;

	/* skip space */

	while ( (buffer[i] == ' ') && (i < 1024))
		i++;

	/* get uncompressed size */

	uncompressed = &buffer[i];
	while ( (buffer[i] >= '0') && buffer[i] <= '9' && (i < 1024))
		i++;
	buffer[i++] = 0;

	close(tube[0]);

	return atoi(uncompressed);
}
