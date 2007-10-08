/*
 * 
 * (c) 2005-2007 Laurent Vivier <Laurent@lvivier.info>
 *
 */

static inline int refnum_to_scsi_id(signed short refnum)
{
	return (~refnum) - 32;
}

static inline signed short scsi_id_to_refnum(int scsi_id)
{
	return ~((signed short)scsi_id + 32);
}

extern void turn_off_interrupts();
