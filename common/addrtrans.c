#include "cmd_handler.h"

void
do_PAR_read(retdatatype * val)
{
    /* ARM reference: B4.1.112 - Physical Address Register */
#ifdef __aarch64__
    ASM_01("MRS %0, PAR_EL1" : "+r" (*val));
#else
    ASM_01("MRC p15, 0, %0, c7, c4, 0"  : "+r" (*val));
#endif
    
}

