#include <stdio.h>
#include <sys/ioctl.h>

#include "maplesyrup.h"
#include "parse.h"
#include "parse_breakout.h"
#include "bitfield_gicv2.h"


bitfield_info bitfield_gicv2_table[] =
{       
    { 1, 0, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 0, 1, "EnableGrp1", "gicc", "GICC_CTLR", "Enable signaling of Group 1 interrupts to CPU" },
    { 1, 1, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 1, 4, "Reserved", "gicc", "GICC_CTLR", "Reserved" },
    { 1, 2, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 5, 1, "FIQBypDisGrp1", "gicc", "GICC_CTLR", "Bypass FIQ signal is signaled to the processor" },
    { 1, 3, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 6, 1, "IRQBypDisGrp1", "gicc", "GICC_CTLR", "Bypass IRQ signal is signaled to the processor" },
    { 1, 4, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 7, 2, "Reserved", "gicc", "GICC_CTLR", "Reserved" },
    { 1, 5, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 9, 1, "EOImodeNS", "gicc", "GICC_CTLR", "Controls behavior of non-secures accesses to GICC_EOIR and GICC_DIR registers" },
    { 1, 6, MS_GIC_GICC_CTLR, NS_PRIVILEGE_LEVEL_1, 10, 22, "Reserved", "gicc", "GICC_CTLR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_PMR, NS_PRIVILEGE_LEVEL_1, 0, 8, "Priority", "gicc", "GICC_PMR", "The priority mask level for the CPU interface" },
    { 1, 1, MS_GIC_GICC_PMR, NS_PRIVILEGE_LEVEL_1, 8, 24, "Reserved", "gicc", "GICC_PMR", "Reserved" },

    { 1, 0, MS_GIC_GICC_BPR, NS_PRIVILEGE_LEVEL_1, 0, 3, "Binary Point", "gicc", "GICC_BPR", "Binary Point Register" },
    { 1, 1, MS_GIC_GICC_BPR, NS_PRIVILEGE_LEVEL_1, 3, 29, "Reserved", "gicc", "GICC_BPR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_IAR, NS_PRIVILEGE_LEVEL_1, 0, 10, "Interrupt ID", "gicc", "GICC_IAR", "The interrupt ID" },
    { 1, 1, MS_GIC_GICC_IAR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_IAR", "CPU interface that generated the SGI" },
    { 1, 2, MS_GIC_GICC_IAR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_IAR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_EOIR, NS_PRIVILEGE_LEVEL_1, 0, 10, "EOIINTID", "gicc", "GICC_EOIR", "The interrupt ID value from the corresponding GICC_IAR access" },
    { 1, 1, MS_GIC_GICC_EOIR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_EOIR", "CPUID from corresponding GICC_IAR access" },
    { 1, 2, MS_GIC_GICC_EOIR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_EOIR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_RPR, NS_PRIVILEGE_LEVEL_1, 0, 8, "Priority", "gicc", "GICC_RPR", "The current running priority of the CPU interface" },
    { 1, 1, MS_GIC_GICC_RPR, NS_PRIVILEGE_LEVEL_1, 8, 24, "Reserved", "gicc", "GICC_RPR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_HPPIR, NS_PRIVILEGE_LEVEL_1, 0, 10, "PENDINTID", "gicc", "GICC_HPPIR", "The interrupt ID of the highest pending priority interrupt" },
    { 1, 1, MS_GIC_GICC_HPPIR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_HPPIR", "CPUID for corresponding interrupt" },
    { 1, 2, MS_GIC_GICC_HPPIR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_HPPIR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_ABPR, NS_PRIVILEGE_LEVEL_1, 0, 3, "Binary Point", "gicc", "GICC_ABPR", "Binary Point" },
    { 1, 1, MS_GIC_GICC_ABPR, NS_PRIVILEGE_LEVEL_1, 3, 29, "Reserved", "gicc", "GICC_ABPR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_AIAR, NS_PRIVILEGE_LEVEL_1, 0, 10, "Interrupt ID", "gicc", "GICC_AIAR", "The interrupt ID" },
    { 1, 1, MS_GIC_GICC_AIAR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_AIAR", "The processor that requested the interrupt" },
    { 1, 2, MS_GIC_GICC_AIAR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_AIAR", "Reserved" },
    
    { 1, 0, MS_GIC_GICC_AEOIR, NS_PRIVILEGE_LEVEL_1, 0, 10, "Interrupt ID", "gicc", "GICC_AEOIR", "The interrupt ID" },
    { 1, 1, MS_GIC_GICC_AEOIR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_AEOIR", "CPUID of the corresponding interrupt" },
    { 1, 2, MS_GIC_GICC_AEOIR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_AEOIR", "Reserved" },
  
    { 1, 0, MS_GIC_GICC_AHPPIR, NS_PRIVILEGE_LEVEL_1, 0, 10, "PENDINTID", "gicc", "GICC_AHPPIR", "The interrupt ID of the highest priority interrupt" },
    { 1, 1, MS_GIC_GICC_AHPPIR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_AHPPIR", "CPUID of the corresponding interrupt" },
    { 1, 2, MS_GIC_GICC_AHPPIR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_AHPPIR", "Reserved" },
      
    { 4, -1, MS_GIC_GICC_RESERVED_00, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicc", "GICC_RESERVED", "Reserved" },
    { 36, -1, MS_GIC_GICC_IMPL_DEF_00, NS_PRIVILEGE_LEVEL_1, 0, 32, "Implementation Defined", "gicc", "GICC_IMPL_DEF", "Implementation Defined" },
    { 4, -1, MS_GIC_GICC_APR, NS_PRIVILEGE_LEVEL_1, 0, 32, "APR", "gicc", "GICC_APR", "Active Priorities Registers" },
    { 4, -1, MS_GIC_GICC_NSAPR, NS_PRIVILEGE_LEVEL_1, 0, 32, "NSAPR", "gicc", "GICC_NSAPR", "Non-secure Active Priorities Registers" },
    { 3, -1, MS_GIC_GICC_RESERVED_01, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicc", "GICC_RESERVED", "Reserved" },

    { 1, 0, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 0, 12, "Implementer", "gicc", "GICC_IIDR", "The JEP106 code for the implementer of the GIC CPU interface" },
    { 1, 1, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 12, 4, "Revision", "gicc", "GICC_IIDR", "Revision number of GIC CPU inteface" },
    { 1, 2, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 16, 4, "Architecture version", "gicc", "GICC_IIDR", "Architecture version" },
    { 1, 3, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 20, 12, "Product ID", "gicc", "GICC_IIDR", "Implentation defined product identifier" },

    { 1, 0, MS_GIC_GICC_DIR, NS_PRIVILEGE_LEVEL_1, 0, 10, "InterruptID", "gicc", "GICC_DIR", "The interrupt ID" },
    { 1, 1, MS_GIC_GICC_DIR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_DIR", "The processor that requested the interrupt" },
    { 1, 2, MS_GIC_GICC_DIR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_DIR", "Reserved" },
   
};

int
return_bitfield_gicv2_size(void)
{
    unsigned int sum_size = 0;
    unsigned int i = 0;
    
    /* take into account ranged entries */
    for (i = 0; i < sizeof(bitfield_gicv2_table) / sizeof(bitfield_info); ++i)
    {
        sum_size += bitfield_gicv2_table[i].range;
    }
    return sum_size;
}

int
return_bitfield_gicv2_compsize(void)
{
    return (sizeof(bitfield_gicv2_table) / sizeof(bitfield_info));
}
