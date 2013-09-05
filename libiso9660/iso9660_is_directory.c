/*
 *
 * (c) 2005 Laurent Vivier <Laurent@Vivier.EU>
 *
 */

#include "libiso9660.h"

int iso9660_is_directory(struct iso_directory_record * idr)
{
	return ((idr->flags[0] & 2) != 0 &&
		    (idr->name_len[0] != 1 ||
		    (idr->name[0] != 0 && idr->name[0] != 1)));
}
