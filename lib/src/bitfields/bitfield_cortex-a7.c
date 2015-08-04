#include <stdio.h>
#include <sys/ioctl.h>

#include "maplesyrup.h"
#include "parse.h"
#include "bitfield_cortex-a7.h"

bitfield_info bitfield_cortex_a7_table[] =
{   
    { 1, 0, MS_SYSREG_REVIDR, NS_PRIVILEGE_LEVEL_1, 0, 32, "ID Number", "ident", "REVIDR", "Revision information" },
    
    { 1, 0, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 0, 2, "CPU ID", "ident", "MPIDR", "Processor number" },
    { 1, 1, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 2, 6, "Reserved", "ident", "MPIDR", "Reserved" },
    { 1, 2, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 8, 4, "Cluster ID", "ident", "MPIDR", "Cluster ID" },
    { 1, 3, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 12, 12, "Reserved", "ident", "MPIDR", "Reserved" },
    { 1, 4, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 24, 1, "MT", "ident", "MPIDR", "Multi-threaded" },
    { 1, 5, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 25, 5, "Reserved", "ident", "MPIDR", "Reserved" },
    { 1, 6, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 30, 1, "U", "ident", "MPIDR", "Uniprocessor system" },
    { 1, 7, MS_SYSREG_MPIDR, NS_PRIVILEGE_LEVEL_1, 31, 1, "ME", "ident", "MPIDR", "Multiprocessor Extensions" },      
    
    { 1, 0, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 0, 6, "Reserved", "other", "ACTLR", "Reserved" },
    { 1, 1, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 6, 1, "SMP", "other", "ACTLR", "Coherent requests to processor" },
    { 1, 2, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 7, 3, "Reserved", "other", "ACTLR", "Reserved" },
    { 1, 3, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 10, 1, "DODMBS", "other", "ACTLR", "Disable optimized data memory barrier behavior" },
    { 1, 4, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 11, 1, "L2RADIS", "other", "ACTLR", "L2 Data Cache read-allocate mode disable" },
    { 1, 5, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 12, 1, "L1RADIS", "other", "ACTLR", "L1 Data Cache read-allocate mode disable" },
    { 1, 6, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 13, 2, "L1PCTL", "other", "ACTLR", "L1 Data prefetch control" },
    { 1, 7, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 15, 1, "DDVM", "other", "ACTLR", "Disable Distributed Virtual Memory transactions" },
    { 1, 8, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 16, 12, "Reserved", "other", "ACTLR", "Reserved" },
    { 1, 9, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 28, 1, "DDI", "other", "ACTLR", "Disable dual issue" },
    { 1, 10, MS_SYSREG_ACTLR, NS_PRIVILEGE_LEVEL_1, 29, 3, "Reserved", "other", "ACTLR", "Reserved" },
    
    { 1, 0, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 0, 10, "Reserved", "secext", "NSACR", "Reserved" },
    { 1, 1, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 10, 1, "CP10", "secext", "NSACR", "Non-secure access to coprocessor 10 enable" },
    { 1, 2, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 11, 1, "CP11", "secext", "NSACR", "Non-secure access to coprecessor 11 enable" },
    { 1, 3, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 12, 2, "Reserved", "secext", "NSACR", "Reserved" },
    { 1, 4, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 14, 1, "NSD32DIS", "secext", "NSACR", "Disable the non-secure use of D16-D31 of the VFP register file" },
    { 1, 5, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 15, 1, "NSASEDIS", "secext", "NSACR", "Disable non-secure Advanced SIMD functionality" },
    { 1, 6, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 16, 1, "Reserved", "secext", "NSACR", "Reserved" },
    { 1, 7, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 17, 1, "NS_L2ERR", "secext", "NSACR", "Allow write to L2 AXI Async Error bit in L2ECTLR in non-secure state" },
    { 1, 8, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 18, 1, "NS_SMP", "secext", "NSACR", "Allow write to SMP bit in ACTLR in non-secure state" },
    { 1, 9, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 19, 1, "Reserved", "secext", "NSACR", "Reserved" },
    { 1, 10, MS_SYSREG_NSACR, NS_PRIVILEGE_LEVEL_1, 20, 12, "Reserved", "secext", "NSACR", "Reserved" },
    
    { 1, 0, MS_SYSREG_L2CTLR, NS_PRIVILEGE_LEVEL_1, 0, 1, "Data Ram latency", "impl_def", "L2CTLR", "Data RAM latency" },
    { 1, 1, MS_SYSREG_L2CTLR, NS_PRIVILEGE_LEVEL_1, 1, 22, "Reserved", "impl_def", "L2CTLR", "Reserved" },
    { 1, 2, MS_SYSREG_L2CTLR, NS_PRIVILEGE_LEVEL_1, 23, 1, "Interrupt Controller", "impl_def", "L2CTLR", "Interrupt Controller" },
    { 1, 3, MS_SYSREG_L2CTLR, NS_PRIVILEGE_LEVEL_1, 24, 2, "Number of processors", "impl_def", "L2CTLR", "Number of processors" },
    { 1, 4, MS_SYSREG_L2CTLR, NS_PRIVILEGE_LEVEL_1, 26, 6, "Reserved", "impl_def", "L2CTLR", "Reserved" },
    
    { 1, 0, MS_SYSREG_L2ECTLR, NS_PRIVILEGE_LEVEL_1, 0, 29, "Reserved", "impl_def", "L2ECTLR", "Reserved" },
    { 1, 1, MS_SYSREG_L2ECTLR, NS_PRIVILEGE_LEVEL_1, 29, 1, "AXI asynchronous error", "impl_def", "L2ECTLR", "AXI asynchronous error" },
    { 1, 2, MS_SYSREG_L2ECTLR, NS_PRIVILEGE_LEVEL_1, 30, 2, "Reserved", "impl_def", "L2ECTLR", "Reserved" },
    
    { 1, 0, MS_SYSREG_CBAR, NS_PRIVILEGE_LEVEL_1, 0, 8, "PERIPHBASE_39_32", "impl_def", "CBAR", "Determines reset value" },
    { 1, 1, MS_SYSREG_CBAR, NS_PRIVILEGE_LEVEL_1, 8, 7, "Reserved", "impl_def", "CBAR", "Reserved" },
    { 1, 2, MS_SYSREG_CBAR, NS_PRIVILEGE_LEVEL_1, 15, 16, "PERIPHBASE_31_15", "impl_def", "CBAR", "Determines reset value" },
    
};

int
return_bitfield_cortex_a7_size(void)
{
    return (sizeof(bitfield_cortex_a7_table) / sizeof(bitfield_info));
}
