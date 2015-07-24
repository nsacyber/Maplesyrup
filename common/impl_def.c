#include "cmd_handler.h"

void
do_L2CTLR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: 4.3.64 - L2 Control Register */   
    ASM_01("MRS %0, S3_1_C11_C0_2" : "+r" (*val));
#else
    ASM_01("MRC p15, 1, %0, c9, c0, 2" : "+r" (*val));
#endif
}

void
do_L2ACTLR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: 4.3.78 - L2 Auxiliary Control Register */ 
    ASM_01("MRS %0, S3_1_C15_C0_0" : "+r" (*val));
#else
    /* ARM reference: B4.3.60 - L2 Auxiliary Control Register */    
    ASM_01("MRC p15, 1, %0, c15, c0, 0" : "+r" (*val));
#endif
}

void
do_L2PFR_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.61 - L2 Prefetch Control Register */ 
    ASM_01("MRC p15, 1, %0, c15, c0, 3" : "+r" (*val));
#endif
}

void
do_CPUACTLR_read(unsigned long long * val)
{
#ifdef __aarch64__
    /* ARM reference: 7.3.78 - Auxiliary Control Register */
    ASM_01("MRS %0, S3_1_C15_C2_0" : "+r" (*val));
#else   
#endif
}

void
do_ACTLR_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.1 - Auxiliary Control Register */
    ASM_01("MRS %0, ACTLR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.1 - Auxiliary Control Register */
    ASM_01("MRC p15, 0, %0, c1, c0, 1" : "+r" (*val));
#endif
}

void
do_ACTLR2_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.62 - Auxiliary Control Register 2 */ 
    ASM_01("MRC p15, 1, %0, c15, c0, 4" : "+r" (*val));
#endif
}

void
do_CPUMERRSR_read(retdatatype * val1, unsigned int * val2)
{
#ifdef __aarch64__
    /* ARM reference: 4.3.80 - CPU Memory Error Syndrome Register */    
    ASM_01("MRS %0, S3_1_C15_C2_2" : "+r" (*val1));
#else   
    /* ARM reference: B4.3.64 - CPU Memory Error Syndrome Register */   
    ASM_02("MRRC p15, 0, %0, %1, c15" : "+r" (*val1), "+r" (*val2));
#endif
}

void
do_L2MERRSR_read(retdatatype * val1, unsigned int * val2)
{
#ifdef __aarch64__
    /* ARM reference: 4.3.81 - L2 Memory Error Syndrome Register */ 
    ASM_01("MRS %0, S3_1_C15_C2_3" : "+r" (*val1));
#else
    /* ARM reference: B4.3.65 - L2 Memory Error Syndrome Register */    
    ASM_02("MRRC p15, 1, %0, %1, c15" : "+r" (*val1), "+r" (*val2));
#endif
}

void
do_L2ECTLR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: 4.3.65 - L2 Auxiliary Control Register */ 
    ASM_01("MRS %0, S3_1_C11_C0_3" : "+r" (*val));
#else
    ASM_01("MRC p15, 1, %0, c9, c0, 3" : "+r" (*val));
#endif
}

void
do_CPUECTLR_read(unsigned long long * val)
{
#ifdef __aarch64__
    /* ARM reference: 4.3.67 - CPU Extended Control Register */ 
    ASM_01("MRS %0, S3_1_C15_C2_1" : "+r" (*val));
#else
#endif
}

void
do_CBAR_read(retdatatype * val)
{   
#ifdef __aarch64__
    /* ARM reference: 4.3.82 - Configuration Base Address */
    ASM_01("MRS %0, S3_1_C15_C3_0" : "+r" (*val));
#else
    ASM_01("MRC p15, 4, %0, c15, c0, 0" : "+r" (*val));
#endif
}

void
do_IL1DATA0_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.57 - Instruction L1 Data n Register */   
    ASM_01("MRC p15, 0, %0, c15, c0, 0" : "+r" (*val));
#endif
}

void
do_IL1DATA1_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.57 - Instruction L1 Data n Register */   
    ASM_01("MRC p15, 0, %0, c15, c0, 1" : "+r" (*val));
#endif
}

void
do_IL1DATA2_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.57 - Instruction L1 Data n Register */   
    ASM_01("MRC p15, 0, %0, c15, c0, 2" : "+r" (*val));
#endif
}

void
do_IL1DATA3_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.57 - Instruction L1 Data n Register */   
    ASM_01("MRC p15, 0, %0, c15, c0, 3" : "+r" (*val));
#endif
}

void
do_DL1DATA0_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.58 - Data L1 Data n Register */  
    ASM_01("MRC p15, 0, %0, c15, c1, 0" : "+r" (*val));
#endif
}

void
do_DL1DATA1_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.58 - Data L1 Data n Register */  
    ASM_01("MRC p15, 0, %0, c15, c1, 1" : "+r" (*val));
#endif
}

void
do_DL1DATA2_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.58 - Data L1 Data n Register */  
    ASM_01("MRC p15, 0, %0, c15, c1, 2" : "+r" (*val));
#endif
}

void
do_DL1DATA3_read(unsigned int * val)
{   
#ifdef __aarch64__
#else
    /* ARM reference: B4.3.58 - Data L1 Data n Register */  
    ASM_01("MRC p15, 0, %0, c15, c1, 3" : "+r" (*val));
#endif
}
