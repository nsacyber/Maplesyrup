#ifndef _BITFIELD_GICV2_H
#define _BITFIELD_GICV2_H

#include "parse.h"

#define MS_GICV2_ALIGNMENT 0x4

/* GICC */
extern bitfield_info bitfield_giccv2_table[];
int return_bitfield_giccv2_size(void);
int return_bitfield_giccv2_compsize(void);

/* GICD */
extern bitfield_info bitfield_gicdv2_table[];
int return_bitfield_gicdv2_size(void);
int return_bitfield_gicdv2_compsize(void);



#endif
