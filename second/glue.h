/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

struct MachineLocation
{
	long latitude;
	long longitude;
	union
	{
		char dlsDelta;
		long gmtDelta;
	} u;
};
typedef struct MachineLocation MachineLocation;


extern void glue_display_properties(unsigned long *base, 
				    unsigned long *row_bytes,
				    unsigned long *width, unsigned long *height,
				    unsigned long *depth, unsigned long *video);
extern int Gestalt(unsigned long selector, long * response);
extern void ReadLocation(MachineLocation * loc);
