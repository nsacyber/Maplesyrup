#include "cmd_handler.h"

void do_DBGDIDR_read(unsigned int * val)
{
    /* ARM reference: C11.11.15 - Debug ID Register */
#ifdef __aarch64__
#else
    ASM_01("MRC p14, 0, %0, c0, c0, 0"  : "+r" (*val));
#endif
}

void do_DBGAUTHSTATUS_read(unsigned int *val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.3.1 - Authentication Status Register */    
    ASM_01("MRS %0, DBGAUTHSTATUS_EL1" : "+r" (*val));  
#else
    /* ARM reference: C11.11.1 - Authentication Status Register */  
    ASM_01("MRC p14, 0, %0, c7, c14, 6" : "+r" (*val)); 
#endif
}

void do_DBGDRAR_read(unsigned long long * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.3.19 - Monitor Debug ROM Address Register */   
    ASM_01("MRS %0, MDRAR_EL1" : "+r" (*val));  
#else
    /* ARM reference: C11.11.16 - Debug ROM Address Register */ 
    ASM_01("MRC p14, 0, %0, c1, c0, 0" : "+r" (*val));  
#endif
}

void do_DBGDSCR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.3.20 - Debug Status and Control Register */    
    ASM_01("MRS %0, MDSCR_EL1" : "+r" (*val));  
#else
    /* ARM reference: C11.11.20 - Debug Status and Control Register */  
    ASM_01("MRC p14, 0, %0, c1, c0, 0" : "+r" (*val));  
#endif
}
