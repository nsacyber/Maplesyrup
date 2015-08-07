#include <stdio.h>
#include <sys/ioctl.h>

#include "maplesyrup.h"
#include "parse.h"
#include "parse_breakout.h"
#include "bitfield_gicv2.h"


bitfield_info bitfield_gicdv2_table[] =
{       
    { 1, 0, MS_GIC_GICD_CTLR, NS_PRIVILEGE_LEVEL_1, 0, 1, "Enable", "gicd", "GICD_CTLR", "Enable forwarding of Group 1 interrupts to CPU interface" },
    { 1, 1, MS_GIC_GICD_CTLR, NS_PRIVILEGE_LEVEL_1, 1, 31, "Reserved", "gicd", "GICD_CTLR", "Reserved" },
    
    { 1, 0, MS_GIC_GICD_TYPER, NS_PRIVILEGE_LEVEL_1, 0, 5, "ITLinesNumber", "gicd", "GICD_TYPER", "Indicates the maximum number of interrupts that the GIC supports. 32(ITLinesNumber + 1)" },
    { 1, 1, MS_GIC_GICD_TYPER, NS_PRIVILEGE_LEVEL_1, 5, 3, "CPUNumber", "gicd", "GICD_TYPER", "Indicates the number of implemented CPU interfaces" },
    { 1, 2, MS_GIC_GICD_TYPER, NS_PRIVILEGE_LEVEL_1, 8, 2, "Reserved", "gicd", "GICD_TYPER", "Reserved" },
    { 1, 3, MS_GIC_GICD_TYPER, NS_PRIVILEGE_LEVEL_1, 10, 1, "SecurityExtn", "gicd", "GICD_TYPER", "Indicates whether the GIC implements the Security Extensions" },
    { 1, 4, MS_GIC_GICD_TYPER, NS_PRIVILEGE_LEVEL_1, 11, 5, "LSPI", "gicd", "GICD_TYPER", "The maximum number of implemented lockable SPIs" },
    { 1, 5, MS_GIC_GICD_TYPER, NS_PRIVILEGE_LEVEL_1, 16, 16, "Reserved", "gicd", "GICD_TYPER", "Reserved" },
    
    { 1, 0, MS_GIC_GICD_IIDR, NS_PRIVILEGE_LEVEL_1, 0, 12, "Implementer", "gicd", "GICD_IIDR", "Contains the JEP106 of the company that implemented the GIC Distributor" },
    { 1, 1, MS_GIC_GICD_IIDR, NS_PRIVILEGE_LEVEL_1, 12, 4, "Revision", "gicd", "GICD_IIDR", "Denotes minor revisions of a product" },
    { 1, 2, MS_GIC_GICD_IIDR, NS_PRIVILEGE_LEVEL_1, 16, 4, "Variant", "gicd", "GICD_IIDR", "Distinguishes product variations, or major revisions of a product" },
    { 1, 3, MS_GIC_GICD_IIDR, NS_PRIVILEGE_LEVEL_1, 20, 4, "Reserved", "gicd", "GICD_IIDR", "Reserved" },
    { 1, 4, MS_GIC_GICD_IIDR, NS_PRIVILEGE_LEVEL_1, 24, 8, "ProductID", "gicd", "GICD_IIDR", "Implementation defined product identifier" },
    
    { 5, 0, MS_GIC_GICD_RESERVED_00, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicd", "GICD_RESERVED_00", "Reserved" },
    
    { 8, 0, MS_GIC_GICD_IMP_DEF_00, NS_PRIVILEGE_LEVEL_1, 0, 32, "Implementation Defined", "gicd", "GICD_IMP_DEF_00", "Implementation Defined" },
    
    { 16, 0, MS_GIC_GICD_RESERVED_01, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicd", "GICD_RESERVED_01", "Reserved" },
    
    { 32, 0, MS_GIC_GICD_IGROUPR, NS_PRIVILEGE_LEVEL_1, 0, 32, "Group status bits", "gicd", "GICD_IGROUPR", "Group number for interrupt (0 or 1)" },
    
    { 32, 0, MS_GIC_GICD_ISENABLER, NS_PRIVILEGE_LEVEL_1, 0, 32, "Set-enable bits", "gicd", "GICD_ISENABLER", "Enable forwarding of interrupt from Distributor to CPU interface" },
    
    { 32, 0, MS_GIC_GICD_ISPENDR, NS_PRIVILEGE_LEVEL_1, 0, 32, "Set-pending bits", "gicd", "GICD_ISPENDR", "Identifies whether an interrupt is pending" },
    
    { 32, 0, MS_GIC_GICD_ICPENDR, NS_PRIVILEGE_LEVEL_1, 0, 32, "Clear-pending bits", "gicd", "GICD_ICPENDR", "Identifies whether an interrupt is pending" },
    
    { 32, 0, MS_GIC_GICD_ISACTIVER, NS_PRIVILEGE_LEVEL_1, 0, 32, "Set-active bits", "gicd", "GICD_ISACTIVER", "Activates an interrupt" },
    
    { 32, 0, MS_GIC_GICD_ICACTIVER, NS_PRIVILEGE_LEVEL_1, 0, 32, "Clear-active bits", "gicd", "GICD_ICACTIVER", "Deactivates an interrupt" },
    
    { 256, 0, MS_GIC_GICD_IPRIORITYR, NS_PRIVILEGE_LEVEL_1, 0, 8, "Priority 0", "gicd", "GICD_IPRIORITYR", "The lower the value, the greater the priority of the interrupt" },  
    { 256, 1, MS_GIC_GICD_IPRIORITYR, NS_PRIVILEGE_LEVEL_1, 8, 8, "Priority 1", "gicd", "GICD_IPRIORITYR", "The lower the value, the greater the priority of the interrupt" },
    { 256, 2, MS_GIC_GICD_IPRIORITYR, NS_PRIVILEGE_LEVEL_1, 16, 8, "Priority 2", "gicd", "GICD_IPRIORITYR", "The lower the value, the greater the priority of the interrupt" },
    { 256, 3, MS_GIC_GICD_IPRIORITYR, NS_PRIVILEGE_LEVEL_1, 24, 8, "Priority 3", "gicd", "GICD_IPRIORITYR", "The lower the value, the greater the priority of the interrupt" },
    
    
    { 1, 0, MS_GIC_GICD_RESERVED_02, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicd", "GICD_RESERVED_02", "Reserved" },
    
    { 64, 0, MS_GIC_GICD_ITARGETSR, NS_PRIVILEGE_LEVEL_1, 0, 8, "CPU Targets 0", "gicd", "GICD_ITARGETSR", "List of target processors for the interrupt" },
    { 64, 1, MS_GIC_GICD_ITARGETSR, NS_PRIVILEGE_LEVEL_1, 8, 8, "CPU Targets 1", "gicd", "GICD_ITARGETSR", "List of target processors for the interrupt" },
    { 64, 2, MS_GIC_GICD_ITARGETSR, NS_PRIVILEGE_LEVEL_1, 16, 8, "CPU Targets 2", "gicd", "GICD_ITARGETSR", "List of target processors for the interrupt" },
    { 64, 3, MS_GIC_GICD_ITARGETSR, NS_PRIVILEGE_LEVEL_1, 24, 8, "CPU Targets 3", "gicd", "GICD_ITARGETSR", "List of target processors for the interrupt" },
    
    { 1, 0, MS_GIC_GICD_RESERVED_03, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicd", "GICD_RESERVED_03", "Reserved" },
    
    { 128, 0, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 0, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 1, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 2, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 2, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 4, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 3, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 6, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 4, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 8, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 5, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 10, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 6, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 12, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 7, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 14, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 8, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 16, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 9, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 18, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 10, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 20, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 11, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 22, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 12, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 24, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 13, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 26, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 14, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 28, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },
    { 128, 15, MS_GIC_GICD_ICFGR, NS_PRIVILEGE_LEVEL_1, 30, 2, "Int_config", "gicd", "GICD_ICFGR", "Interrupt is level-sensitive of edge-triggered" },  
    
    { 32, 0, MS_GIC_GICD_IMP_DEF_01, NS_PRIVILEGE_LEVEL_1, 0, 32, "Implementation Defined", "gicd", "GICD_IMP_DEF_01", "Implementation Defined" },
    
    { 64, 0, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 0, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 1, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 2, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 2, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 4, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 3, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 6, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 4, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 8, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 5, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 10, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 6, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 12, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 7, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 14, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 8, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 16, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 9, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 18, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 10, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 20, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 11, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 22, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 12, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 24, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 13, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 26, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 14, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 28, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    { 64, 15, MS_GIC_GICD_NSACR, NS_PRIVILEGE_LEVEL_1, 30, 2, "NS_access", "gicd", "GICD_NSACR", "Configures level of non-secure access permittted" },
    
    { 1, 0, MS_GIC_GICD_SGIR, NS_PRIVILEGE_LEVEL_1, 0, 4, "SGIINTD", "gicd", "GICD_SGIR", "Interrupt ID of SGI to forward to specified CPU interface" },
    { 1, 1, MS_GIC_GICD_SGIR, NS_PRIVILEGE_LEVEL_1, 4, 11, "Reserved", "gicd", "GICD_SGIR", "Reserved" },
    { 1, 2, MS_GIC_GICD_SGIR, NS_PRIVILEGE_LEVEL_1, 15, 1, "NSATT", "gicd", "GICD_SGIR", "Required security value of the SGI" },
    { 1, 3, MS_GIC_GICD_SGIR, NS_PRIVILEGE_LEVEL_1, 16, 8, "CPUTargetList", "gicd", "GICD_SGIR", "Defines the CPU interface to which the Distributor must forward the interrupt" },
    { 1, 4, MS_GIC_GICD_SGIR, NS_PRIVILEGE_LEVEL_1, 24, 2, "TargetListFilter", "gicd", "GICD_SGIR", "Determines how the Distributor must forward the requested SGI" },
    { 1, 5, MS_GIC_GICD_SGIR, NS_PRIVILEGE_LEVEL_1, 26, 6, "Reserved", "gicd", "GICD_SGIR", "Reserved" },
    
    { 3, 0, MS_GIC_GICD_RESERVED_04, NS_PRIVILEGE_LEVEL_1, 0, 4, "Reserved", "gicd", "GICD_RESERVED_04", "Reserved" },
    
    { 4, 0, MS_GIC_GICD_CPENDSGIR, NS_PRIVILEGE_LEVEL_1, 0, 8, "SGI clear-pending 0", "gicd", "GICD_CPENDSGIR", "Indicates whether the SGI is pending" },
    { 4, 1, MS_GIC_GICD_CPENDSGIR, NS_PRIVILEGE_LEVEL_1, 8, 8, "SGI clear-pending 1", "gicd", "GICD_CPENDSGIR", "Indicates whether the SGI is pending" },
    { 4, 2, MS_GIC_GICD_CPENDSGIR, NS_PRIVILEGE_LEVEL_1, 16, 8, "SGI clear-pending 2", "gicd", "GICD_CPENDSGIR", "Indicates whether the SGI is pending" },
    { 4, 3, MS_GIC_GICD_CPENDSGIR, NS_PRIVILEGE_LEVEL_1, 24, 8, "SGI clear-pending 3", "gicd", "GICD_CPENDSGIR", "Indicates whether the SGI is pending" },
    
    { 4, 0, MS_GIC_GICD_SPENDSGIR, NS_PRIVILEGE_LEVEL_1, 0, 8, "SGI set-pending 0", "gicd", "GICD_SPENDSGIR", "Indicates whether the SGI is pending" },
    { 4, 1, MS_GIC_GICD_SPENDSGIR, NS_PRIVILEGE_LEVEL_1, 8, 8, "SGI set-pending 1", "gicd", "GICD_SPENDSGIR", "Indicates whether the SGI is pending" },
    { 4, 2, MS_GIC_GICD_SPENDSGIR, NS_PRIVILEGE_LEVEL_1, 16, 8, "SGI set-pending 2", "gicd", "GICD_SPENDSGIR", "Indicates whether the SGI is pending" },
    { 4, 3, MS_GIC_GICD_SPENDSGIR, NS_PRIVILEGE_LEVEL_1, 24, 8, "SGI set-pending 3", "gicd", "GICD_SPENDSGIR", "Indicates whether the SGI is pending" },
    
    { 40, 0, MS_GIC_GICD_RESERVED_05, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicd", "GICD_RESERVED_05", "Reserved" },
    
    { 12, 0, MS_GIC_GICD_IDENT, NS_PRIVILEGE_LEVEL_1, 0, 32, "Implementation Defined", "gicd", "GICD_IDENT", "Implementation Defined ID" },

    { 1, 0, MS_GIC_GICD_ICPIDR2, NS_PRIVILEGE_LEVEL_1, 0, 4, "Implementation Defined", "gicd", "GICD_ICPIDR2", "Implementation Defined" },
    { 1, 1, MS_GIC_GICD_ICPIDR2, NS_PRIVILEGE_LEVEL_1, 4, 4, "ArchRev", "gicd", "GICD_ICPIDR2", "Revision field for the GIC architecture" },
    { 1, 2, MS_GIC_GICD_ICPIDR2, NS_PRIVILEGE_LEVEL_1, 8, 24, "Implementation Defined", "gicd", "GICD_ICPIDR2", "Implementation Defined" },
    
};

int
return_bitfield_gicdv2_size(void)
{
    unsigned int sum_size = 0;
    unsigned int i = 0;
    
    /* take into account ranged entries */
    for (i = 0; i < sizeof(bitfield_gicdv2_table) / sizeof(bitfield_info); ++i)
    {
        sum_size += bitfield_gicdv2_table[i].range;
    }
    return sum_size;
}

int
return_bitfield_gicdv2_compsize(void)
{
    return (sizeof(bitfield_gicdv2_table) / sizeof(bitfield_info));
}


bitfield_info bitfield_giccv2_table[] =
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
      
    { 4, 0, MS_GIC_GICC_RESERVED_00, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicc", "GICC_RESERVED", "Reserved" },
    
    { 24, 0, MS_GIC_GICC_IMPL_DEF_00, NS_PRIVILEGE_LEVEL_1, 0, 32, "Implementation Defined", "gicc", "GICC_IMPL_DEF", "Implementation Defined" },
    
    { 4, 0, MS_GIC_GICC_APR, NS_PRIVILEGE_LEVEL_1, 0, 32, "APR", "gicc", "GICC_APR", "Active Priorities Registers" },
      
    { 4, 0, MS_GIC_GICC_NSAPR, NS_PRIVILEGE_LEVEL_1, 0, 32, "NSAPR", "gicc", "GICC_NSAPR", "Non-secure Active Priorities Registers" },
    
    { 3, 0, MS_GIC_GICC_RESERVED_01, NS_PRIVILEGE_LEVEL_1, 0, 32, "Reserved", "gicc", "GICC_RESERVED", "Reserved" },

    { 1, 0, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 0, 12, "Implementer", "gicc", "GICC_IIDR", "The JEP106 code for the implementer of the GIC CPU interface" },
    { 1, 1, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 12, 4, "Revision", "gicc", "GICC_IIDR", "Revision number of GIC CPU inteface" },
    { 1, 2, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 16, 4, "Architecture version", "gicc", "GICC_IIDR", "Architecture version" },
    { 1, 3, MS_GIC_GICC_IIDR, NS_PRIVILEGE_LEVEL_1, 20, 12, "Product ID", "gicc", "GICC_IIDR", "Implentation defined product identifier" },

    { 1, 0, MS_GIC_GICC_DIR, NS_PRIVILEGE_LEVEL_1, 0, 10, "InterruptID", "gicc", "GICC_DIR", "The interrupt ID" },
    { 1, 1, MS_GIC_GICC_DIR, NS_PRIVILEGE_LEVEL_1, 10, 3, "CPUID", "gicc", "GICC_DIR", "The processor that requested the interrupt" },
    { 1, 2, MS_GIC_GICC_DIR, NS_PRIVILEGE_LEVEL_1, 13, 19, "Reserved", "gicc", "GICC_DIR", "Reserved" },
   
};

int
return_bitfield_giccv2_size(void)
{
    unsigned int sum_size = 0;
    unsigned int i = 0;
    
    /* take into account ranged entries */
    for (i = 0; i < sizeof(bitfield_giccv2_table) / sizeof(bitfield_info); ++i)
    {
        sum_size += bitfield_giccv2_table[i].range;
    }
    return sum_size;
}

int
return_bitfield_giccv2_compsize(void)
{
    return (sizeof(bitfield_giccv2_table) / sizeof(bitfield_info));
}
