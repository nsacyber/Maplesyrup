#include "cmd_handler.h"

void
do_AIDR_read(unsigned int * val)
{       
#ifdef __aarch64__
    /* ARM reference: D7.2.10 - Auxiliary ID Register */
    ASM_01("MRS %0, AIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.3 - Auxiliary ID Register */
    ASM_01("MRC p15, 1, %0, c0, c0, 7" : "+r" (*val));
#endif
}

void
do_REVIDR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.72 - Revision ID Register */
    ASM_01("MRS %0, REVIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.128 - Revision ID Register */    
    ASM_01("MRC p15, 0, %0, c0, c0, 6" : "+r" (*val));
#endif
}

void
do_TLBTR_read(unsigned int * val)
{   
#ifdef __aarch64__  
#else
    /* ARM reference: B4.1.139 - TLB Type Register */   
    ASM_01("MRC p15, 0, %0, c0, c0, 3" : "+r" (*val));  
#endif
}

void
do_AFR0_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.46 - Auxiliary Feature Register */   
    ASM_01("MRS %0, ID_AFR0_EL1" : "+r" (*val));    
#else
    /* ARM reference: B4.1.81 - Auxiliary Feature Register */   
    ASM_01("MRC p15, 0, %0, c0, c1, 3" : "+r" (*val));  
#endif
}

void
do_CCSIDR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.14 - Cache Size ID Registers */  
    ASM_01("MRS %0, CCSIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.19 - Cache Size ID Registers */  
    ASM_01("MRC p15, 1, %0, c0, c0, 0" : "+r" (*val));
#endif
}


void
do_CLIDR_read(retdatatype * val)
{
    
#ifdef __aarch64__
    /* ARM reference: D7.2.15 - Cache Level ID Register */  
    ASM_01("MRS %0, CLIDR_EL1" : "+r" (*val));  
#else
    /* ARM reference: B4.1.20 - Cache Level ID Register */
    ASM_01("MRC p15, 1, %0, c0, c0, 1" : "+r" (*val));          
#endif
}

void
do_TCM_read(retdatatype * val)
{
    
#ifdef __aarch64__
#else
    /* ARM reference: B4.1.132 - TCM Type Register */   
    ASM_01("MRC p15, 0, %0, c0, c0, 2" : "+r" (*val));  
#endif
}


void
do_CSSELR_read(unsigned int * val)
{
    
#ifdef __aarch64__
    /* ARM reference: D7.2.20 - Cache size selection */ 
    ASM_01("MRS %0, CSSELR_EL1" : "+r" (*val)); 
#else
    /* ARM reference: B4.1.41 - Cache size selection */ 
    ASM_01("MRC p15, 2, %0, c0, c0, 0" : "+r" (*val));  
#endif
}

void
do_CTR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: C7.2.21 - Cache Type */   
    ASM_01("MRS %0, CTR_EL0" : "+r" (*val));    
#else
    /* ARM reference: B4.1.42 - Cache Type */   
    ASM_01("MRC p15, 0, %0, c0, c0, 1" : "+r" (*val));  
#endif
}

void do_TCMTR_read(unsigned int * val)
{   
#ifdef __aarch64__  
#else
    ASM_01("MRC p15, 0, %0, c0, c0, 2" : "+r" (*val));
#endif
}

void
do_MIDR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.66 - Multiprocessor Affinity Register */ 
    ASM_01("MRS %0, MIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.105 - Multiprocessor Affinity Register */    
    ASM_01("MRC p15, 0, %0, c0, c0, 0" : "+r" (*val));      
#endif
}

void
do_MPIDR_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.67 - Multiprocessor Affinity Register */ 
    ASM_01("MRS %0, MPIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.106 - Multiprocessor Affinity Register */
    ASM_01("MRC p15, 0, %0, c0, c0, 5" : "+r" (*val));
#endif
}

void
do_DFR0_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.47 - Multiprocessor Affinity Register */ 
    ASM_01("MRS %0, ID_DFR0_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.82 - Debug Feature Register */   
    ASM_01("MRC p15, 0, %0, c0, c1, 2" : "+r" (*val));
#endif
}

void
do_ISAR0_read(unsigned int * val)
{   
#ifdef __aarch64__
    /* ARM reference: D7.2.48 - Instruction Set Attribute Register */   
    ASM_01("MRS %0, ID_ISAR0_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.83 - Instruction Set Attribute Register */   
    ASM_01("MRC p15, 0, %0, c0, c2, 0" : "+r" (*val));
#endif
}

void
do_ISAR1_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.49 - Instruction Set Attribute Register 1 */ 
    ASM_01("MRS %0, ID_ISAR1_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.84 - Instruction Set Attribute Register 1 */ 
    ASM_01("MRC p15, 0, %0, c0, c2, 1" : "+r" (*val));
#endif
}

void
do_ISAR2_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.50 - Instruction Set Attribute Register 2 */ 
    ASM_01("MRS %0, ID_ISAR2_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.85 - Instruction Set Attribute Register 2 */
    ASM_01("MRC p15, 0, %0, c0, c2, 2" : "+r" (*val));
#endif
}

void
do_ISAR3_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.51 - Instruction Set Attribute Register 3 */ 
    ASM_01("MRS %0, ID_ISAR3_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.86 - Instruction Set Attribute Register 3 */
    ASM_01("MRC p15, 0, %0, c0, c2, 3" : "+r" (*val));  
#endif
}

void
do_ISAR4_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.52 - Instruction Set Attribute Register 4 */ 
    ASM_01("MRS %0, ID_ISAR4_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.87 - Instruction Set Attribute Register 4 */ 
    ASM_01("MRC p15, 0, %0, c0, c2, 4" : "+r" (*val));  
#endif
}

void
do_ISAR5_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.53 - Instruction Set Attribute Register 5 */ 
    ASM_01("MRS %0, ID_ISAR5_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.88 - Instruction Set Attribute Register 5 */
    ASM_01("MRC p15, 0, %0, c0, c2, 5" : "+r" (*val));
#endif
}


void
do_MMFR0_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.1.54 - Memory Model Feature Register 0 */
    ASM_01("MRS %0, ID_MMFR0_EL1" : "+r" (*val));   
#else
    /* ARM reference: B4.1.89 - Memory Model Feature Register 0 */
    ASM_01("MRC p15, 0, %0, c0, c1, 4" : "+r" (*val));  
#endif
}

void
do_MMFR1_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.1.55 - Memory Model Feature Register 1 */
    ASM_01("MRS %0, ID_MMFR1_EL1" : "+r" (*val));   
#else
    /* ARM reference: B4.1.90 - Memory Model Feature Register 1 */  
    ASM_01("MRC p15, 0, %0, c0, c1, 5" : "+r" (*val));  
#endif
}

void
do_MMFR2_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.1.56 - Memory Model Feature Register 2 */
    ASM_01("MRS %0, ID_MMFR2_EL1" : "+r" (*val));   
#else
    /* ARM reference: B4.1.91 - Memory Model Feature Register 2 */
    ASM_01("MRC p15, 0, %0, c0, c1, 6" : "+r" (*val));  
#endif
}

void
do_MMFR3_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.1.57 - Memory Model Feature Register 3 */
    ASM_01("MRS %0, ID_MMFR3_EL1" : "+r" (*val));   
#else
    /* ARM reference: B4.1.92 - Memory Model Feature Register 3 */  
    ASM_01("MRC p15, 0, %0, c0, c1, 7" : "+r" (*val));  
#endif
}

void
do_PFR0_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.59 - Processor Feature Register 0 */ 
    ASM_01("MRS %0, ID_PFR0_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.93 - Processor Feature Register 0 */ 
    ASM_01("MRC p15, 0, %0, c0, c1, 0" : "+r" (*val));
#endif
}

void
do_PFR1_read(unsigned int * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.60 - Processor Feature Register 1 */ 
    ASM_01("MRS %0, ID_PFR1_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.94 - Processor Feature Register 1 */ 
    ASM_01("MRC p15, 0, %0, c0, c1, 1" : "+r" (*val));  
#endif
}
