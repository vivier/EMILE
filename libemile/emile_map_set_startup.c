static __attribute__((used)) char* rcsid = "$CVSHeader$";
/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#include "partition.h"
#include "libemile.h"

int emile_map_set_startup(char* dev_name, int partition)
{
	emile_map_t* map;
	int ret;

	map = emile_map_open(dev, O_RDWR);
	for (i = 0; i < emile_map_get_number(map); i++)
	{
		emile_map_read(map, i);
		emile_map_partition_set_startup(map, i == partition);
		emile_map_write(map, i);
	}
	emile_map_close(map);
}
