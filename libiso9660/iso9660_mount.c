/*
 *
 * (c) 2005 Laurent Vivier <Laurent@lvivier.info>
 *
 * some parts from mkisofs (c) J. Schilling
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include "libiso9660.h"
#include "iso9660.h"

#ifdef DEBUG
void
printchars(s, n)
        char    *s;
        int     n;
{
        int     i;
        char    *p;

        for (; n > 0 && *s; n--) {
                if (*s == ' ') {
                        p = s;
                        i = n;
                        while (--i >= 0 && *p++ == ' ')
                                ;
                        if (i <= 0)
                                break;
                }
                putchar(*s++);
        }
}

void print_info(struct iso_primary_descriptor *ipd)
{
	/* filesystem information */

	printf("System id: ");
	printchars(ipd->system_id, 32);
	putchar('\n');
	printf("Volume id: ");
	printchars(ipd->volume_id, 32);
	putchar('\n');

	printf("Volume set id: ");
	printchars(ipd->volume_set_id, 128);
	putchar('\n');
	printf("Publisher id: ");
	printchars(ipd->publisher_id, 128);
	putchar('\n');
	printf("Data preparer id: ");
	printchars(ipd->preparer_id, 128);
	putchar('\n');
	printf("Application id: ");
	printchars(ipd->application_id, 128);
	putchar('\n');

	printf("Copyright File id: ");
	printchars(ipd->copyright_file_id, 37);
	putchar('\n');
	printf("Abstract File id: ");
	printchars(ipd->abstract_file_id, 37);
	putchar('\n');
	printf("Bibliographic File id: ");
	printchars(ipd->bibliographic_file_id, 37);
	putchar('\n');

	printf("Volume set size is: %d\n", isonum_723(ipd->volume_set_size));
	printf("Volume set sequence number is: %d\n", isonum_723(ipd->volume_sequence_number));
	printf("Logical block size is: %d\n", isonum_723(ipd->logical_block_size));
	printf("Volume size is: %d\n", isonum_733((unsigned char *)ipd->volume_space_size));
}
#endif

void iso9660_name(stream_VOLUME *volume, struct iso_directory_record *idr, char *buffer)
{
	int	j;
	unsigned char	/* uh, */ ul, uc;

	buffer[0] = 0;
	if (idr->name_len[0] == 1 && idr->name[0] == 0)
		strcpy(buffer, ".");
	else if (idr->name_len[0] == 1 && idr->name[0] == 1)
		strcpy(buffer, "..");
	else {
		switch (((iso9660_VOLUME*)volume)->ucs_level) {
		case 3:
		case 2:
		case 1:
			/*
			 * Unicode name.
			 */

			for (j = 0; j < (int)idr->name_len[0] / 2; j++) {
				/* uh = idr->name[j*2]; */
				ul = idr->name[j*2+1];

				/*
				 * unicode convertion
				 * up = unls->unls_uni2cs[uh];
				 *
				 * if (up == NULL)
				 *	uc = '\0';
				 * else
				 *	uc = up[ul];
				 *
				 * we use only low byte
				 */

				uc = ul;

				buffer[j] = uc ? uc : '_';
			}
			buffer[idr->name_len[0]/2] = '\0';
			break;
		case 0:
			/*
			 * Normal non-Unicode name.
			 */
			strncpy(buffer, idr->name, idr->name_len[0]);
			buffer[idr->name_len[0]] = 0;
			break;
		default:
			/*
			 * Don't know how to do these yet.  Maybe they are the same
			 * as one of the above.
			 */
			break;
		}
	}
}

stream_VOLUME *iso9660_mount(device_io_t *device)
{
	iso9660_VOLUME* volume;
	struct iso_primary_descriptor *jpd;
	struct iso_primary_descriptor ipd;
	/* struct iso_directory_record * idr; */
	int	block;
	int ucs_level;

	/* read filesystem descriptor */

	device->read_sector(device->data, 16, &ipd, sizeof (ipd));
	/* idr = (struct iso_directory_record *)ipd.root_directory_record; */

	/*
	 * High sierra:
	 *
	 *	DESC TYPE	== 1 (VD_SFS)	offset 8	len 1
	 *	STR ID		== "CDROM"	offset 9	len 5
	 *	STD_VER		== 1		offset 14	len 1
	 */

	/* High Sierra format ? */

	if ((((char *)&ipd)[8] == 1) &&
	    (strncmp(&((char *)&ipd)[9], "CDROM", 5) == 0) &&
	    (((char *)&ipd)[14] == 1)) {
		printf("Incompatible format: High Sierra format\n");
		return NULL;
	}

	/*
	 * ISO 9660:
	 *
	 *	DESC TYPE	== 1 (VD_PVD)	offset 0	len 1
	 *	STR ID		== "CD001"	offset 1	len 5
	 *	STD_VER		== 1		offset 6	len 1
	 */

	/* NOT ISO 9660 format ? */

	if ((ipd.type[0] != ISO_VD_PRIMARY) ||
	    (strncmp(ipd.id, ISO_STANDARD_ID, sizeof (ipd.id)) != 0) ||
	    (ipd.version[0] != 1)) {
		printf("Not ISO 9660 format\n");
		return NULL;
	}

#ifdef DEBUG
	print_info(&ipd);
#endif

	/* UCS info */

	block = 16;

	jpd = (struct iso_primary_descriptor *)
		malloc(sizeof(struct iso_primary_descriptor));
	if (jpd == NULL)
		return NULL;

	memcpy(jpd, &ipd, sizeof (ipd));
	while ((u_int8_t)jpd->type[0] != ISO_VD_END) {

		/*
		 * If Joliet UCS escape sequence found, we may be wrong
		 */

		if (jpd->unused3[0] == '%' &&
		    jpd->unused3[1] == '/' &&
		    (jpd->unused3[3] == '\0' ||
		    jpd->unused3[3] == ' ') &&
		    (jpd->unused3[2] == '@' ||
		    jpd->unused3[2] == 'C' ||
		    jpd->unused3[2] == 'E')) {

			if (jpd->version[0] == 1)
				goto nextblock;
		}
#ifdef DEBUG
		if (jpd->type[0] == 0) {
			printf("EL TORITO\n");
		}
		if (jpd->version[0] == 2) {
			printf("CD-ROM uses ISO 9660:1999 relaxed format\n");
			break;
		}
#endif

nextblock:
		block++;
		device->read_sector(device->data, block, jpd, sizeof (*jpd));
	}

	/*
	 * ISO 9660:
	 *
	 *	DESC TYPE	== 1 (VD_PVD)	offset 0	len 1
	 *	STR ID		== "CD001"	offset 1	len 5
	 *	STD_VER		== 1		offset 6	len 1
	 */

	/* NOT ISO 9660 format ? */

	if ((ipd.type[0] != ISO_VD_PRIMARY) ||
	    (strncmp(ipd.id, ISO_STANDARD_ID, sizeof (ipd.id)) != 0) ||
	    (ipd.version[0] != 1)) {
		printf("Not ISO9660 format\n");
		return NULL;
	}

	block = 16;
	memcpy(jpd, &ipd, sizeof (ipd));
	while ((unsigned char) jpd->type[0] != ISO_VD_END) {

		/*
		 * Find the UCS escape sequence.
		 */
		if (jpd->unused3[0] == '%' &&
		    jpd->unused3[1] == '/' &&
		    (jpd->unused3[3] == '\0' ||
		    jpd->unused3[3] == ' ') &&
		    (jpd->unused3[2] == '@' ||
		    jpd->unused3[2] == 'C' ||
		    jpd->unused3[2] == 'E')) {
			break;
		}

		block++;
		device->read_sector(device->data, block, jpd, sizeof (*jpd));
	}

	/* Unable to find Joliet SVD */

	if (((unsigned char) jpd->type[0] == ISO_VD_END)) {
		free(jpd);
		printf("Not ISO9660 Joliet format\n");
		return NULL;
	}

	switch (jpd->unused3[2]) {
	case '@':
		ucs_level = 1;
		break;
	case 'C':
		ucs_level = 2;
		break;
	case 'E':
		ucs_level = 3;
		break;
	default:
		/* Don't know what ucs_level */
		free(jpd);
		return NULL;
	}

	if (jpd->unused3[3] == ' ')
		printf("Warning: Joliet escape sequence uses illegal space at offset 3\n");

	volume = (iso9660_VOLUME*)malloc(sizeof(iso9660_VOLUME));
	if (volume == NULL)
		return NULL;

	volume->descriptor = jpd;
	volume->ucs_level = ucs_level;
	volume->device = device;

	return (stream_VOLUME*)volume;
}

int iso9660_umount(stream_VOLUME* volume)
{
	iso9660_VOLUME *__volume = (iso9660_VOLUME *)volume;
	if (__volume == NULL)
		return -1;
	free(__volume->descriptor);
	free(__volume);
	return 0;
}

struct iso_directory_record *iso9660_get_root_node(iso9660_VOLUME* volume)
{
	return (struct iso_directory_record *)volume->descriptor->root_directory_record;
}
