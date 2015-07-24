#include "cmd_handler.h"

void
do_CNTFRQ_read(retdatatype * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.5.1 - Counter Frequency */ 
    ASM_01("MRS %0, CNTFRQ_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.21 - Counter Frequency */    
    ASM_01("MRC p15, 0, %0, c14, c0, 0" : "+r" (*val));
#endif
}

void
do_CNTHCTL_read(retdatatype * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.22 - Timer PL2 */
    ASM_01("MRC p15, 4, %0, c14, c1, 0" : "+r" (*val));
#endif
}

void
do_CNTKCTL_read(retdatatype * val)
{
    
#ifdef __aarch64__
    /* ARM reference: D7.5.6 - Counter-timer Kernel Control */
    ASM_01("MRS %0, CNTKCTL_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.26 - Counter-timer Kernel Control */
    ASM_01("MRC p15, 0, %0, c14, c1, 0" : "+r" (*val));
#endif
}

void
do_CNTPCTL_read(retdatatype * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.5.6 - Physical Timer Control */
    ASM_01("MRS %0, CNTP_CTL_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.27 - Physical Timer Control */
    ASM_01("MRC p15, 0, %0, c14, c2, 1" : "+r" (*val));
#endif
}

void
do_CNTPCVAL_read(
#ifdef __aarch64__
        unsigned long long * val
#else
        unsigned int * val1,
        unsigned int * val2
#endif
    )
{   
#ifdef __aarch64__
    /* ARM reference: D7.5.6 - Physical Timer Control */
    ASM_01("MRS %0, CNTP_CTL_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.27 - Physical Timer Control */   
    ASM_02("MRRC p15, 2, %0, %1, c14" : "+r" (*val1), "+r" (*val2));
#endif
}
