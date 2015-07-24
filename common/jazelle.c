#include "cmd_handler.h"

void
do_JIDR_read(retdatatype * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.101 - Jazelle ID Register */ 
    ASM_01("MRC p14, 7, %0, c0, c0, 0" : "+r" (*val));
#endif
}


void
do_JMCR_read(retdatatype * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.102 - Jazelle Main Configuration Register */ 
    ASM_01("MRC p14, 7, %0, c2, c0, 0" : "+r" (*val));
#endif
}

void
do_JOSCR_read(retdatatype * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.103 - Jazelle OS Configuration Register */
    ASM_01("MRC p14, 7, %0, c1, c0, 0" : "+r" (*val));
#endif
}
