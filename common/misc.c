#include "cmd_handler.h"


void
do_TPIDRPRW_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.88 - Thread ID Register */   
    ASM_01("MRS %0, TPIDR_EL1" : "+r" (*val));
#else
    /* ARM reference: B4.1.139 - Thread ID Register */  
    ASM_01("MRC p15, 0, %0, c13, c0, 4" : "+r" (*val));
#endif
}

void
do_TPIDRURO_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.91 - EL0 Read-only Software Thread ID Register */
    ASM_01("MRS %0, TPIDRRO_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.151 - User Read-Only Thread ID Register */
    ASM_01("MRC p15, 0, %0, c13, c0, 3" : "+r" (*val));
#endif
}

void
do_TPIDRURW_read(retdatatype * val)
{
#ifdef __aarch64__
    /* ARM reference: D7.2.87 - EL0 Read/Write Thread ID Register */
    ASM_01("MRS %0, TPIDR_EL0" : "+r" (*val));
#else
    /* ARM reference: B4.1.152 - User Read/Write Thread ID Register */
    ASM_01("MRC p15, 0, %0, c13, c0, 2" : "+r" (*val));
#endif
}
