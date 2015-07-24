#include "cmd_handler.h"

void
do_PMCCFILTR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.1 -  Performance Monitors Cycle Count Filter Register */
    ASM_01("MRS %0, PMCCFILTR_EL0": "+r" (*val));   
#else   
#endif
}

void
do_PMCCNTR_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.2 -  Performance Monitors Cycle Count Register */
    ASM_01("MRS %0, PMCCNTR_EL0": "+r" (*val)); 
#else
    /* ARM reference: B4.1.113 - Performance Monitors Cycle Count Register */
    ASM_01("MRC p15, 0, %0, c9, c13, 0" : "+r" (*val));
#endif
}

void
do_PMCEID0_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.3 -  Performance Monitors Common Event ID Register */
    ASM_01("MRS %0, PMCEID0_EL0": "+r" (*val)); 
#else
    /* ARM reference: B4.1.114 - Performance Monitors Common Event ID Register */   
    ASM_01("MRC p15, 0, %0, c9, c12, 6" : "+r" (*val)); 
#endif
}

void
do_PMCEID1_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.4 -  Performance Monitors Common Event ID Register */
    ASM_01("MRS %0, PMCEID1_EL0": "+r" (*val)); 
#else
    /* ARM reference: B4.1.114 - Performance Monitors Common Event ID Register */   
    ASM_01("MRC p15, 0, %0, c9, c12, 7" : "+r" (*val));
#endif
}

void
do_PMCNTENSET_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.6 -  Performance Monitors Count Enable Set Register */   
    ASM_01("MRS %0, PMCNTENSET_EL0": "+r" (*val));  
#else
    /* ARM reference: B4.1.115 - Performance Monitors Count Enable Set Register */  
    ASM_01("MRC p15, 0, %0, c9, c12, 1" : "+r" (*val)); 
#endif
}

void
do_PMCNTENCLR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.5 -  Performance Monitors Count Enable Clear Register */ 
    ASM_01("MRS %0, PMCNTENCLR_EL0": "+r" (*val));  
#else
    /* ARM reference: B4.1.115 - Performance Monitors Count Enable Clear Register */    
    ASM_01("MRC p15, 0, %0, c9, c12, 2" : "+r" (*val)); 
#endif
}

void
do_PMCR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.7 - Performance Monitors Control Register */ 
    ASM_01("MRS %0, PMCR_EL0": "+r" (*val));    
#else
    /* ARM reference: B4.1.117 - Performance Monitors Control Register */   
    ASM_01("MRC p15, 0, %0, c9, c12, 0" : "+r" (*val)); 
#endif
}

void
do_PMSELR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.14 - Performance Monitors Event Counter Selection Register */    
    ASM_01("MRS %0, PMSELR_EL0" : "+r" (*val)); 
#else
    /* ARM reference: B4.1.122 - Performance Monitors Event Counter Selection Register */   
    ASM_01("MRC p15, 0, %0, c9, c12, 5" : "+r" (*val)); 
#endif
}

void
do_PMXEVTYPER_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.18 - Performance Monitors Event Type Register */ 
    ASM_01("MRS %0, PMXEVTYPER_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.125 - Performance Monitors Event Type Register */    
    ASM_01("MRC p15, 0, %0, c9, c13, 2" : "+r" (*val)); 
#endif
}

void
do_PMXEVCNTR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.17 - Performance Monitors Event Count Register */    
    ASM_01("MRS %0, PMXEVCNTR_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.125 - Performance Monitors Event Count Register */   
    ASM_01("MRC p15, 0, %0, c9, c13, 1" : "+r" (*val)); 
#endif
}

void
do_PMUSERENR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.16 - Performance Monitors User Enable Register */    
    ASM_01("MRS %0, PMUSERENR_EL0" : "+r" (*val));  
#else
    /* ARM reference: B4.1.124 - Performance Monitors User Enable Register */   
    ASM_01("MRC p15, 0, %0, c9, c14, 0" : "+r" (*val)); 
#endif
}

void
do_PMOVSR_read(unsigned int * val)
{
#ifdef __aarch64__  
#else
    /* ARM reference: B4.1.120 - Performance Monitors Overflow Flag Status Register */  
    ASM_01("MRC p15, 0, %0, c9, c12, 3" : "+r" (*val)); 
#endif
}

void
do_PMINTENCLR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.10 - Performance Monitors Count Enable Clear Register */ 
    ASM_01("MRS %0, PMINTENCLR_EL1" : "+r" (*val)); 
#else
    /* ARM reference: B4.1.115 - Performance Monitors Count Enable Clear Register */    
    ASM_01("MRC p15, 0, %0, c9, c14, 1" : "+r" (*val)); 
#endif
}

void
do_PMINTENSET_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.11 - Performance Monitors Interrupt Enable Set Register */   
    ASM_01("MRS %0, PMINTENSET_EL1" : "+r" (*val)); 
#else
    /* ARM reference: B4.1.110 - Performance Monitors Interrupt Enable Set Register */  
    ASM_01("MRC p15, 0, %0, c9, c14, 1" : "+r" (*val)); 
#endif
}

void
do_PMOVSCLR_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.12 - Performance Monitors Overflow Flag Status Clear */  
    ASM_01("MRS %0, PMOVSCLR_EL0" : "+r" (*val));   
#else
#endif
}

void
do_PMOVSSET_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.13 - Performance Monitors Interrupt Enable Set Register */   
    ASM_01("MRS %0, PMOVSSET_EL0" : "+r" (*val));   
#else
    /* ARM reference: B4.1.121 - Performance Monitors Interrupt Enable Set Register */  
    ASM_01("MRC p15, 0, %0, c9, c14, 3" : "+r" (*val)); 
#endif
}

void
do_PMSWINC_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.4.13 - Performance Monitors Software Increment Register */ 
    ASM_01("MRS %0, PMSWINC_EL0" : "+r" (*val));    
#else
    /* ARM reference: B4.1.123 - Performance Monitors Software Increment Register */    
    ASM_01("MRC p15, 0, %0, c9, c12, 4" : "+r" (*val)); 
#endif
}
