#include <stdio.h>
#include <sys/ioctl.h>

#include "maplesyrup.h"
#include "parse.h"
#include "bitfield_cortex-a7.h"

bitfield_info bitfield_cortex_a7_table[] =
{   
    { BO_A7_REVIDR_ID, MS_IOCTL_REVIDR, NS_PRIVILEGE_LEVEL_1, 0, 32, "ID Number", "ident", "REVIDR", "Revision information" },
    
    { BO_A7_MPIDR_CPUID, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 0, 2, "CPU ID", "ident", "MPIDR", "Processor number" },
    { BO_A7_MPIDR_RESERVED_01, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 2, 6, "Reserved", "ident", "MPIDR", "Reserved" },
    { BO_A7_MPIDR_CLUSTERID, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 8, 4, "Cluster ID", "ident", "MPIDR", "Cluster ID" },
    { BO_A7_MPIDR_RESERVED_02, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 12, 12, "Reserved", "ident", "MPIDR", "Reserved" },
    { BO_A7_MPIDR_MT, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 24, 1, "MT", "ident", "MPIDR", "Multi-threaded" },
    { BO_A7_MPIDR_RESERVED_03, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 25, 5, "Reserved", "ident", "MPIDR", "Reserved" },
    { BO_A7_MPIDR_U, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 30, 1, "U", "ident", "MPIDR", "Uniprocessor system" },
    { BO_A7_MPIDR_ME, MS_IOCTL_MPIDR, NS_PRIVILEGE_LEVEL_1, 31, 1, "ME", "ident", "MPIDR", "Multiprocessor Extensions" },      
    
    { BO_A7_ACTLR_RESERVED_01, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 0, 6, "Reserved", "other", "ACTLR", "Reserved" },
    { BO_A7_ACTLR_SMP, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 6, 1, "SMP", "other", "ACTLR", "Coherent requests to processor" },
    { BO_A7_ACTLR_RESERVED_02, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 7, 3, "Reserved", "other", "ACTLR", "Reserved" },
    { BO_A7_ACTLR_DODMBS, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 10, 1, "DODMBS", "other", "ACTLR", "Disable optimized data memory barrier behavior" },
    { BO_A7_ACTLR_L2RADIS, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 11, 1, "L2RADIS", "other", "ACTLR", "L2 Data Cache read-allocate mode disable" },
    { BO_A7_ACTLR_L1RADIS, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 12, 1, "L1RADIS", "other", "ACTLR", "L1 Data Cache read-allocate mode disable" },
    { BO_A7_ACTLR_L1PCTL, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 13, 2, "L1PCTL", "other", "ACTLR", "L1 Data prefetch control" },
    { BO_A7_ACTLR_DDVM, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 15, 1, "DDVM", "other", "ACTLR", "Disable Distributed Virtual Memory transactions" },
    { BO_A7_ACTLR_RESERVED_03, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 16, 12, "Reserved", "other", "ACTLR", "Reserved" },
    { BO_A7_ACTLR_DDI, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 28, 1, "DDI", "other", "ACTLR", "Disable dual issue" },
    { BO_A7_ACTLR_RESERVED_04, MS_IOCTL_ACTLR, NS_PRIVILEGE_LEVEL_1, 29, 3, "Reserved", "other", "ACTLR", "Reserved" },
    
    { BO_A7_NSACR_RESERVED_01, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 0, 10, "Reserved", "secext", "NSACR", "Reserved" },
    { BO_A7_NSACR_CP10, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 10, 1, "CP10", "secext", "NSACR", "Non-secure access to coprocessor 10 enable" },
    { BO_A7_NSACR_CP11, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 11, 1, "CP11", "secext", "NSACR", "Non-secure access to coprecessor 11 enable" },
    { BO_A7_NSACR_RESERVED_02, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 12, 2, "Reserved", "secext", "NSACR", "Reserved" },
    { BO_A7_NSACR_NSD32DIS, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 14, 1, "NSD32DIS", "secext", "NSACR", "Disable the non-secure use of D16-D31 of the VFP register file" },
    { BO_A7_NSACR_NSASEDIS, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 15, 1, "NSASEDIS", "secext", "NSACR", "Disable non-secure Advanced SIMD functionality" },
    { BO_A7_NSACR_RESERVED_03, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 16, 1, "Reserved", "secext", "NSACR", "Reserved" },
    { BO_A7_NSACR_NSL2ERR, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 17, 1, "NS_L2ERR", "secext", "NSACR", "Allow write to L2 AXI Async Error bit in L2ECTLR in non-secure state" },
    { BO_A7_NSACR_NSSMP, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 18, 1, "NS_SMP", "secext", "NSACR", "Allow write to SMP bit in ACTLR in non-secure state" },
    { BO_A7_NSACR_RESERVED_04, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 19, 1, "Reserved", "secext", "NSACR", "Reserved" },
    { BO_A7_NSACR_RESERVED_05, MS_IOCTL_NSACR, NS_PRIVILEGE_LEVEL_1, 20, 12, "Reserved", "secext", "NSACR", "Reserved" },
    
    { BO_A7_L2CTLR_LATENCY, MS_IOCTL_L2CTLR, NS_PRIVILEGE_LEVEL_1, 0, 1, "Data Ram latency", "impl_def", "L2CTLR", "Data RAM latency" },
    { BO_A7_L2CTLR_RESERVED_01, MS_IOCTL_L2CTLR, NS_PRIVILEGE_LEVEL_1, 1, 22, "Reserved", "impl_def", "L2CTLR", "Reserved" },
    { BO_A7_L2CTLR_IC, MS_IOCTL_L2CTLR, NS_PRIVILEGE_LEVEL_1, 23, 1, "Interrupt Controller", "impl_def", "L2CTLR", "Interrupt Controller" },
    { BO_A7_L2CTLR_NUMPROC, MS_IOCTL_L2CTLR, NS_PRIVILEGE_LEVEL_1, 24, 2, "Number of processors", "impl_def", "L2CTLR", "Number of processors" },
    { BO_A7_L2CTLR_RESERVED_02, MS_IOCTL_L2CTLR, NS_PRIVILEGE_LEVEL_1, 26, 6, "Reserved", "impl_def", "L2CTLR", "Reserved" },
    
    { BO_A7_L2ECTLR_RESERVED_01, MS_IOCTL_L2ECTLR, NS_PRIVILEGE_LEVEL_1, 0, 29, "Reserved", "impl_def", "L2ECTLR", "Reserved" },
    { BO_A7_L2ECTLR_AXI, MS_IOCTL_L2ECTLR, NS_PRIVILEGE_LEVEL_1, 29, 1, "AXI asynchronous error", "impl_def", "L2ECTLR", "AXI asynchronous error" },
    { BO_A7_L2ECTLR_RESERVED_02, MS_IOCTL_L2ECTLR, NS_PRIVILEGE_LEVEL_1, 30, 2, "Reserved", "impl_def", "L2ECTLR", "Reserved" },
    
    { BO_A7_CBAR_PERIPHBASE_39_32, MS_IOCTL_CBAR, NS_PRIVILEGE_LEVEL_1, 0, 8, "PERIPHBASE_39_32", "impl_def", "CBAR", "Determines reset value" },
    { BO_A7_CBAR_RESERVED, MS_IOCTL_CBAR, NS_PRIVILEGE_LEVEL_1, 8, 7, "Reserved", "impl_def", "CBAR", "Reserved" },
    { BO_A7_CBAR_PERIPHBASE_31_15, MS_IOCTL_CBAR, NS_PRIVILEGE_LEVEL_1, 15, 16, "PERIPHBASE_31_15", "impl_def", "CBAR", "Determines reset value" },
    
};

int
return_bitfield_cortex_a7_size(void)
{
    return (sizeof(bitfield_cortex_a7_table) / sizeof(bitfield_info));
}
