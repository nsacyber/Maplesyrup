#include "cmd_handler.h"


void do_AA64AFR0_read(unsigned long long *val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.36 - Aux Feature Register */ 
    ASM_01("MRS %0, ID_AA64AFR0_EL1" : "+r" (*val));    
#else
#endif
}

void do_AA64AFR1_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.37 - Aux Feature Register */ 
    ASM_01("MRS %0, ID_AA64AFR1_EL1" : "+r" (*val));    
#else
#endif
}

void do_AA64DFR0_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.38 - Debug Feature Register */   
    ASM_01("MRS %0, ID_AA64DFR0_EL1" : "+r" (*val));    
#else
#endif
}

void do_AA64DFR1_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.39 - Debug Feature Register */   
    ASM_01("MRS %0, ID_AA64DFR1_EL1" : "+r" (*val));    
#else
#endif
}

void do_AA64ISAR0_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.40 - Instruction Set Attribute Register */   
    ASM_01("MRS %0, ID_AA64ISAR0_EL1" : "+r" (*val));   
#else
#endif
}

void do_AA64ISAR1_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.41 - Instruction Set Attribute Register */   
    ASM_01("MRS %0, ID_AA64ISAR1_EL1" : "+r" (*val));   
#else
#endif
}

void do_AA64MMFR0_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.42 - Memory Model Feature Register */    
    ASM_01("MRS %0, ID_AA64MMFR0_EL1" : "+r" (*val));   
#else
#endif
}

void do_AA64MMFR1_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.43 - Memory Model Feature Register */    
    ASM_01("MRS %0, ID_AA64MMFR1_EL1" : "+r" (*val));   
#else
#endif
}

void do_AA64PFR0_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.44 - Processor Feature Register */
    ASM_01("MRS %0, ID_AA64PFR0_EL1" : "+r" (*val));    
#else
#endif
}

void do_AA64PFR1_read(unsigned long long * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.45 - Processor Feature Register */
    ASM_01("MRS %0, ID_AA64PFR1_EL1" : "+r" (*val));    
#else
#endif
}

void do_CURRENTEL_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C5.3.1 - Current exception level */
    ASM_01("MRS %0, CurrentEL" : "+r" (*val));  
#else
#endif
}

void do_DAIF_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C5.3.2 - Interrup mask bits */
    ASM_01("MRS %0, DAIF" : "+r" (*val));   
#else
#endif
}

void do_FPCR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C5.3.8 - Floating pointer control register */
    ASM_01("MRS %0, FPCR" : "+r" (*val));   
#else
#endif
}

void do_FPSR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C5.3.9 - Floating point status register */
    ASM_01("MRS %0, FPSR" : "+r" (*val));   
#else
#endif
}

void do_NZCV_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C5.3.10 - Condition flags */
    ASM_01("MRS %0, NZCV" : "+r" (*val));   
#else
#endif
}

void do_SPSEL_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C5.3.15 - Stack pointer select */
    ASM_01("MRS %0, SPSEL" : "+r" (*val));  
#else
#endif
}


