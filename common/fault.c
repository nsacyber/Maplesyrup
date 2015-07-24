#include "cmd_handler.h"

void
do_ADFSR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.4 - Auxiliary Fault Status Register */
    ASM_01("MRS %0, AFSR0_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.2 - Auxiliary Data and Instruction Fault Status Register */
    ASM_01("MRC p15, 0, %0, c5, c1, 0" : "+r" (*val));
#endif
}

void
do_AIFSR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.7 - Auxiliary Fault Status Register */
    ASM_01("MRS %0, AFSR1_EL1" : "+r" (*val));
#else
    /* ARM referrnce: B4.1.2 - Auxiliary Data and Instruction Fault Status Register */
    ASM_01("MRC p15, 0, %0, c5, c1, 1" : "+r" (*val));
#endif
}

void
do_IFAR_read(unsigned int * val)
{
    /* ARM reference: B4.1.95 - Instruction Fault Address Register */
#ifdef __aarch64__
#else
    ASM_01("MRC p15, 0, %0, c6, c0, 2" : "+r" (*val));
#endif
}

void
do_IFSR_read(unsigned int * val)
{
    /* ARM reference: B4.1.96 - Instruction Fault Status Register */    
#ifdef __aarch64__  
#else
    ASM_01("MRC p15, 0, %0, c5, c0, 1" : "+r" (*val));
#endif
}

void
do_DFAR_read(unsigned int * val)
{
    /* ARM reference: B4.1.51 - Data Fault Address */
#ifdef __aarch64__
#else
    ASM_01("MRC p15, 0, %0, c6, c0, 0" : "+r" (*val));
#endif
}

void
do_DFSR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.24 - Exception Syndrome Register */
    ASM_01("MRS %0, ESR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.52 - Data Fault Status Register */
    ASM_01("MRC p15, 0, %0, c5, c0, 0" : "+r" (*val));
#endif
}

void
do_FAR_read(unsigned long long * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.28 - Fault Address Register */
    ASM_01("MRS %0, FAR_EL1" : "+r" (*val));
#else
#endif  
}
