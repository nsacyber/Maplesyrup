#include "cmd_handler.h"

void
do_NMRR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.110 - Normal Memory Remap Register */    
    ASM_01("MRC p15, 0, %0, c10, c2, 1" : "+r" (*val));
#endif
}

void
do_PRRR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.127 - Primary Region Remap Register */
    ASM_01("MRC p15, 0, %0, c10, c2, 0" : "+r" (*val));
#endif  
}


void
do_SCTLR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.81 - System Control Register */  
    ASM_01("MRS %0, SCTLR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.130 - System Control Register */ 
    ASM_01("MRC p15, 0, %0, c1, c0, 0" : "+r" (*val));
#endif
}

void
do_TTBCR_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.84 - Translation control register */ 
    ASM_01("MRS %0, TCR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.153 - Translation Table Base Control Register */ 
    ASM_01("MRC p15, 0, %0, c2, c0, 2" : "+r" (*val));
#endif
}

void
do_TTBR0_read(retdatatype * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.92 - Translation Table Base  Register 0 */   
    ASM_01("MRS %0, TTBR0_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.154 - Translation Table Base  Register 0 */  
    ASM_01("MRC p15, 0, %0, c2, c0, 0" : "+r" (*val));    
#endif
}

void
do_TTBR0LPAE_read(unsigned int * low, unsigned int * hi)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.154 - Translation Table Base  Register 0 */  
    ASM_02("MRRC p15, 0, %0, %1, c2" : "+r" (*low), "+r" (*hi));
#endif
}

void
do_TTBR1_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.95 - Translation Table Base  Register 0 */   
    ASM_01("MRS %0, TTBR1_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.155 - Translation Table Base  Register 1 */  
    ASM_01("MRC p15, 0, %0, c2, c0, 1" : "+r" (*val));
#endif
}

void
do_TTBR1LPAE_read(unsigned int * low, unsigned int * hi)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.155 - Translation Table Base  Register 1 */  
    ASM_02("MRRC p15, 1, %0, %1, c2" : "+r" (*low), "+r" (*hi));
#endif
}

void
do_CONTEXTIDR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.16 - Context ID */   
    ASM_01("MRS %0, CONTEXTIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.36 - Context ID */   
    ASM_01("MRC p15, 0, %0, c13, c0, 1" : "+r" (*val));
#endif  
}

void
do_DACR_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.43 - Domain Access */    
    ASM_01("MRC p15, 0, %0, c3, c0, 0" : "+r" (*val));
#endif
}

void
do_MAIR_read(unsigned long long * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.63 - MAIR Memory Attribute Indirection Register */   
    ASM_01("MRS %0, MAIR_EL1" : "+r" (*val));
#else   
#endif  
}

void
do_MAIR0_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.104 - MAIR0 Memory Attribute Indirection Register */ 
    ASM_01("MRC p15, 0, %0, c10, c2, 0" : "+r" (*val));
#endif
}

void
do_MAIR1_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.104 - MAIR1 Memory Attribute Indirection Register */ 
    ASM_01("MRC p15, 0, %0, c10, c2, 1" : "+r" (*val));
#endif
}

void
do_AMAIR_read(unsigned long long * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.11 - Auxiliary Memory Attribute Indirection Register */
    ASM_01("MRS %0, AMAIR_EL1" : "+r" (*val));
#else
#endif
}

void
do_AMAIR0_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.5 - Auxiliary Memory Attribute Indirection Register 0 */ 
    ASM_01("MRC p15, 0, %0, c10, c3, 0" : "+r" (*val));
#endif
}

void
do_AMAIR1_read(unsigned int * val)
{
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.5 - Auxiliary Memory Attribute Indirection Register 1 */ 
    ASM_01("MRC p15, 0, %0, c10, c3, 1" : "+r" (*val));
#endif
}

void
do_DCZID_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.23 - Data Cache Zero ID register */  
    ASM_01("MRS %0, DCZID_EL0" : "+r" (*val));
#else   
#endif  
}
