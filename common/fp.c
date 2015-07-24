#include "cmd_handler.h"


void
do_FPEXC_read(retdatatype * val)
{
    /* ARM reference: B4.1.57 - Floating-Point Exception Control register */    
#ifdef __aarch64__
#else
    ASM_FP("VMRS %0, FPEXC" : "+r" (*val)); 
#endif
}

void
do_MVFR0_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.68 - Media and VFP Feature Register */
    ASM_01("MRS %0, MVFR0_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.108 - Media and VFP Feature Register */
    ASM_FP("VMRS %0, MVFR0" : "+r" (*val));
#endif
}

void
do_MVFR1_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.69 - Media and VFP Feature Register */
    ASM_01("MRS %0, MVFR1_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.109 - Media and VFP Feature Register */
    ASM_FP("VMRS %0, MVFR1" : "+r" (*val));
#endif
}

void
do_MVFR2_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.70 - Media and VFP Feature Register */
    ASM_01("MRS %0, MVFR2_EL1" : "+r" (*val));
#else   
#endif
}

void
do_FPSID_read(retdatatype * val)
{       
    /* ARM reference: B4.1.59 - Floating-point System ID Register */    
#ifdef __aarch64__
#else
    ASM_FP("VMRS %0, FPSID" : "+r" (*val));
#endif
}

void
do_FPSCR_read(retdatatype * val)
{   
    /* ARM reference: B4.1.58 - Floating-point Status and Control Register */   
#ifdef __aarch64__
#else
    ASM_FP("VMRS %0, FPSCR" : "+r" (*val));
#endif
}
