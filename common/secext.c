#include "cmd_handler.h"

void
do_ISR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.62 - Interrupt Status Register */
    ASM_01("MRS %0, ISR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.97 - Interrupt Status Register */
    ASM_01("MRC p15, 0, %0, c12, c1, 0" : "+r" (*val));
#endif
}

void
do_MVBAR_read(retdatatype * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.107 - Monitor Vector base Address Regster, Security Extensions */    
    ASM_01("MRC p15, 0, %0, c12, c0, 1" : "+r" (*val));
#endif
}

void
do_NSACR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.111 - Non-Secure Access Control Register */  
    ASM_01("MRC p15, 0, %0, c1, c1, 2" : "+r" (*val));
#endif
}


void
do_SDER_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.131 - Secure Debug Enable Register */    
    ASM_01("MRC p15, 0, %0, c1, c1, 1" : "+r" (*val));  
#endif
}


void
do_SCR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.1.129 SCR - Secure Configuration Register */ 
    ASM_01("MRC p15, 0, %0, c1, c1, 0" : "+r" (*val));
#endif
}

void
do_VBAR_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: B7.2.96 - Vector Base Address Register */ 
    ASM_01("MRS %0, VBAR_EL1" : "+r" (*val));   
#else
    /* ARM reference: B4.1.156 - Vector Base Address Register */    
    ASM_01("MRC p15, 0, %0, c12, c0, 0" : "+r" (*val)); 
#endif
}
