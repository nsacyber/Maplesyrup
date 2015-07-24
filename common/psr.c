#include "cmd_handler.h"

void
do_APSR_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    ASM_01("MRS %0, APSR"   : "+r" (*val));
#endif
}

void
do_CPSR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    ASM_01("MRS %0, CPSR"   : "+r" (*val));
#endif
}

void
do_SPSR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    ASM_01("MRS %0, SPSR"   : "+r" (*val));
#endif
}
