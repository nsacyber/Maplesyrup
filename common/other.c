#include "cmd_handler.h"

void
do_CPACR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.17 - Auxiliary Control Register */
    ASM_01("MRS %0, CPACR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.40 - Coprocessor Access */
    ASM_01("MRC p15, 0, %0, c1, c0, 2" : "+r" (*val));  
#endif
}

void
do_FCSEIDR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.56 - Fast Context Switch Extension Data Register */  
    ASM_01("MRC p15, 0, %0, c13, c0, 0" : "+r" (*val));
#endif
}

void do_RMR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.73 - Reset Management Register */
    ASM_01("MRS %0, RMR_EL1" : "+r" (*val));
#else   
#endif
}

void do_RVBAR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.76 - Reset Vector Base Address Register */
    ASM_01("MRS %0, RVBAR_EL1" : "+r" (*val));
#else   
#endif
}
