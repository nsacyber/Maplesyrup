#include "cmd_handler.h"

void
do_TEECR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.133 - ThumbEE Configuration Register */  
    ASM_01("MRC p14, 0, %0, c0, c0, 0" : "+r" (*val));
#endif
}

void
do_TEEHBR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.134 - ThumbEE Handler Base Register */
    ASM_01("MRC p14, 6, %0, c1, c0, 0" : "+r" (*val));  
#endif
}


