#ifndef _MS_PARSE_H
#define _MS_PARSE_H

#include "support.h"

#define MS_MAX_CPUS 8       // Fixed number of CPUs

#define MS_MAGIC 0xB0
#define MS_IOCTL_CO         _IO(MS_MAGIC, 0)        // Coprocessor interface
#define MS_IOCTL_MM         _IO(MS_MAGIC, 1)        // Mem-mapped coprocessor interface
#define MS_IOCTL_GIC        _IO(MS_MAGIC, 2)        // GIC interface
#define MS_IOCTL_TRACE      _IO(MS_MAGIC, 3)        // TRACE interface
#define MS_IOCTL_DEBUG      _IO(MS_MAGIC, 4)        // Debug interface


typedef struct ms_data
{
    unsigned int cpu;
    unsigned int midr;
    unsigned int in1;
    unsigned int in2;
    unsigned long long out64;
    unsigned int out1;
    unsigned int out2;
    unsigned int status;
} ms_data;

typedef struct ms_data_cpu
{
    unsigned int code;
    unsigned int pcpu; //preferred cpu code
    unsigned int part_number;
    ms_data cpu[MS_MAX_CPUS];
} ms_data_cpu;


/*******************
 * 
 * 
 * <-PARTNUMBER-><-INSTRUCTION-><-FIELD->
 * 
 * */

#define PARTNUMBER_BITS     5       // arm-v7a part number
#define INSTRUCTION_BITS    11      // unique informational registers
#define FIELD_BITS      16           // holds the number of bitfields in register

#define MSCODE_FIELDSHIFT 0
#define MSCODE_INSTRUCTIONSHIFT (MSCODE_FIELDSHIFT + FIELD_BITS)
#define MSCODE_PARTNUMBERSHIFT  (MSCODE_INSTRUCTIONSHIFT + INSTRUCTION_BITS)

#define PARTNUMBER_MASK     ((1 << PARTNUMBER_BITS) - 1)
#define INSTRUCTION_MASK    ((1 << INSTRUCTION_BITS) - 1)
#define FIELD_MASK          ((1 << FIELD_BITS) - 1)

/* To create a code */
#define MSCODE(partnumber, instruction, field) \
        (((partnumber) << MSCODE_PARTNUMBERSHIFT) | \
            ((instruction) << MSCODE_INSTRUCTIONSHIFT) | \
            ((field) << MSCODE_FIELDSHIFT))

/* To decode */
#define MSCODE_PARTNUMBER(code)     (((code) >> MSCODE_PARTNUMBERSHIFT) & PARTNUMBER_MASK)
#define MSCODE_INSTRUCTION(code)    (((code) >> MSCODE_INSTRUCTIONSHIFT) & INSTRUCTION_MASK)
#define MSCODE_FIELD(code)          (((code) >> MSCODE_FIELDSHIFT) & FIELD_MASK)


#define PARSE_MAX_DESCRIPTION 256
#define PARSE_MAX_BITFIELD 64
#define PARSE_MAX_FUNCTIONAL 64
#define PARSE_MAX_REGISTER 32
#define PARSE_MAX_PARTNUMBER 64
#define PARSE_MAX_ARCHITECTURE 16
#define PARSE_MAX_IMPLEMENTATION 16


typedef struct bitfield_info
{
    unsigned int range;         // greater or equal to than 1
    unsigned int bitfield_code; // bitfield 
    unsigned int ioctl_code;    // register level
    unsigned int ppl;           // preferred privilege level
    unsigned int rightshift;    // number of right bitshifts
    unsigned int mask;          // mask to apply after right bitshift
    char * bitfield;            // string representation of 
    char * functional_group;
    char * register_name;
    char * description; 
} bitfield_info;

/* *****************************
 * Final bitfield data returned 
 * to user
 * ****************************/
typedef struct bitfield_info_populated
{
    list_element list;
    unsigned int cpu;
    unsigned int epl;
    unsigned int uid;
    unsigned int ioctl;
    unsigned int midr;
    unsigned int status;
    char bitfield[PARSE_MAX_BITFIELD];
    char functional_group[PARSE_MAX_FUNCTIONAL];
    char register_name[PARSE_MAX_REGISTER];
    char description[PARSE_MAX_DESCRIPTION];    
    unsigned long long value;
    unsigned long long uvalue;
} bitfield_info_populated;

extern bitfield_info bitfield_info_table[];

/* *****************************
 * Internal structures used
 * for managing data
 * ****************************/
typedef struct match_info
{
    unsigned int architecture;
    unsigned int implementation;
    unsigned char functional_group[PARSE_MAX_FUNCTIONAL];
    unsigned char register_name[PARSE_MAX_REGISTER];
    unsigned int bitfield_code;
    unsigned int pl; // override preferred pl
    unsigned int noparse;
    unsigned int cpu;
    unsigned int calc;  // calc_value is valid
    unsigned long long calc_value; 
    
} match_info;

typedef struct results_info
{
    list_element list;
    unsigned int ioctl_code;
    unsigned int uid;
    unsigned int ppl;
    unsigned int pl;
    unsigned int cpu;
    unsigned int noparse;
    unsigned int part_number;
    unsigned int calc;
    unsigned long long calc_value;
} results_info;

typedef struct used_ioctl
{
    list_element list;
    unsigned int ioctl_code;
    unsigned int cpu;
    unsigned int partnumber;
} used_ioctl;

typedef struct ioctl_result
{
    list_element list;
    unsigned int ioctl_code;
    unsigned long long val64;
    unsigned int low;
    unsigned int high;
    unsigned int cpu; // executed CPU
    unsigned int midr; // for implementation number
    unsigned int epl; // executed PL
    unsigned int status; // indicates that call failed if non-null
} ioctl_result;

typedef struct table_index
{
    list_element list;
    unsigned int index;    
} table_index;

enum
{
    /* Identification Registers function group */
    MS_SYSREG_PFR0 = 1,
    MS_SYSREG_PFR1,
    MS_SYSREG_DFR0,
    MS_SYSREG_AFR0,
    MS_SYSREG_MMFR0,
    MS_SYSREG_MMFR1,
    MS_SYSREG_MMFR2,
    MS_SYSREG_MMFR3,
    MS_SYSREG_MMFR4,
    MS_SYSREG_ISAR0,
    MS_SYSREG_ISAR1,
    MS_SYSREG_ISAR2,
    MS_SYSREG_ISAR3,
    MS_SYSREG_ISAR4,
    MS_SYSREG_ISAR5, 
    MS_SYSREG_REVIDR,
    MS_SYSREG_TLBTR, 
    MS_SYSREG_CCSIDR,
    MS_SYSREG_TCMTR,
    MS_SYSREG_AIDR,
    MS_SYSREG_CLIDR,
    MS_SYSREG_CSSELR,
    MS_SYSREG_CTR,
    MS_SYSREG_MIDR,
    MS_SYSREG_MPIDR,
    MS_SYSREG_AA64AFR0,
    MS_SYSREG_AA64AFR1,
    MS_SYSREG_AA64DFR0,
    MS_SYSREG_AA64DFR1,
    MS_SYSREG_AA64ISAR0,
    MS_SYSREG_AA64ISAR1,
    MS_SYSREG_AA64MMFR0,
    MS_SYSREG_AA64MMFR1,
    MS_SYSREG_AA64PFR0,
    MS_SYSREG_AA64PFR1,
    
    /* Security Extensions functional group */
    MS_SYSREG_ISR,
    MS_SYSREG_MVBAR,
    MS_SYSREG_NSACR,
    MS_SYSREG_SCR,
    MS_SYSREG_SDER,
    MS_SYSREG_VBAR,  
    
    /* Virtual Memory Control functional group */   
    MS_SYSREG_NMRR,
    MS_SYSREG_PRRR,
    MS_SYSREG_SCTLR,
    MS_SYSREG_TTBCR,
    MS_SYSREG_TTBR0,
    MS_SYSREG_TTBR0LPAE,
    MS_SYSREG_TTBR1,
    MS_SYSREG_TTBR1LPAE,
    MS_SYSREG_CONTEXTIDR,
    MS_SYSREG_AMAIR,
    MS_SYSREG_AMAIR0,
    MS_SYSREG_AMAIR1,
    MS_SYSREG_AMAIR2,
    MS_SYSREG_DACR,
    MS_SYSREG_MAIR,
    MS_SYSREG_MAIR0,
    MS_SYSREG_MAIR1,
    MS_SYSREG_DCZID,
    MS_SYSREG_TCR,
    
    /* Other System Control Registers */
    MS_SYSREG_ACTLR,
    MS_SYSREG_CPACR,
    MS_SYSREG_FCSEIDR,
    MS_SYSREG_RMR,
    MS_SYSREG_RVBAR,
    
    /* Performance Monitors */
    MS_SYSREG_PMCCFILTR,
    MS_SYSREG_PMCR,
    MS_SYSREG_PMCCNTR,
    MS_SYSREG_PMCEID0,
    MS_SYSREG_PMCEID1,   
    MS_SYSREG_PMCNTENSET,
    MS_SYSREG_PMCNTENCLR,
    MS_SYSREG_PMINTENSET,
    MS_SYSREG_PMINTENCLR,
    MS_SYSREG_PMOVSCLR,
    MS_SYSREG_PMOVSR,
    MS_SYSREG_PMOVSSET,
    MS_SYSREG_PMSELR,
    MS_SYSREG_PMSWINC,
    MS_SYSREG_PMUSERENR,
    MS_SYSREG_PMXEVCNTR,
    MS_SYSREG_PMXEVTYPER,    
    
    /* Fault Handling functional group */
    MS_SYSREG_IFAR,
    MS_SYSREG_IFSR,
    MS_SYSREG_ADFSR,
    MS_SYSREG_AIFSR,
    MS_SYSREG_DFAR,
    MS_SYSREG_DFSR,
    MS_SYSREG_AFSR0,
    MS_SYSREG_AFSR1,
    MS_SYSREG_ESR,
    MS_SYSREG_FAR,
    
    /* Address Translations */
    MS_SYSREG_PAR,
    
    /* Floating Point */
    MS_SYSREG_FPEXC,
    MS_SYSREG_FPSCR,
    MS_SYSREG_FPSID,
    MS_SYSREG_MVFR0,
    MS_SYSREG_MVFR1, 
    MS_SYSREG_MVFR2, 
    
    /* Debug */
    MS_SYSREG_DBGDIDR,
    MS_SYSREG_DBGAUTHSTATUS, 
    MS_SYSREG_DBGBCR,    //todo
    MS_SYSREG_DBGBVR,    //todo
    MS_SYSREG_DBGBXVR,   //todo
    MS_SYSREG_DBGCID0,   //todo
    MS_SYSREG_DBGCID1,   //todo
    MS_SYSREG_DBGCID2,   //todo
    MS_SYSREG_DBGCID3,   //todo
    MS_SYSREG_DBGCIDSR,  //todo
    MS_SYSREG_DBGCLAIMCLR,   //todo
    MS_SYSREG_DBGCLAIMSET,   //todo
    MS_SYSREG_DBGDEVID,  //todo
    MS_SYSREG_DBGDEVID1, //todo
    MS_SYSREG_DBGDEVTYPE,    //todo
    MS_SYSREG_DBGDRAR,
    MS_SYSREG_DBGDRCR,   //todo
    MS_SYSREG_DBGDSAR,   //todo
    MS_SYSREG_DBGDSCCR,  //todo
    MS_SYSREG_DBGDSCR,
    MS_SYSREG_DBGDSMCR,  //todo
    MS_SYSREG_DBGDTRRX,  //todo
    MS_SYSREG_DBGDTRTX,  //todo
    MS_SYSREG_DBGEACR,   //todo
    MS_SYSREG_DBGECR,    //todo
    MS_SYSREG_DBGITCTRL, //todo
    MS_SYSREG_DBGITR,    //todo
    MS_SYSREG_DBGLAR,    //todo
    MS_SYSREG_DBGLSR,    //todo
    MS_SYSREG_DBGOSDLR,  //todo
    MS_SYSREG_DBGOSLSR,  //todo
    MS_SYSREG_DBGOSSRR,  //todo
    MS_SYSREG_DBGPCSR,   //todo
    MS_SYSREG_DBGPID0,   //todo
    MS_SYSREG_DBGPID1,   //todo
    MS_SYSREG_DBGPID2,   //todo
    MS_SYSREG_DBGPID3,   //todo
    MS_SYSREG_DBGPID4,   //todo
    MS_SYSREG_DBGPRCR,   //todo
    MS_SYSREG_DBGPRSR,   //todo
    MS_SYSREG_DBGVCR,    //todo
    MS_SYSREG_DBGVIDSR,  //todo
    MS_SYSREG_DBGWCR,    //todo
    MS_SYSREG_DBGWFAR,   //todo
    MS_SYSREG_DBGWVR,    //todo
    
    /* Jazelle */
    MS_SYSREG_JIDR,
    MS_SYSREG_JMCR,
    MS_SYSREG_JOSCR,
    
    /* Thumb */
    MS_SYSREG_TEECR,
    MS_SYSREG_TEEHBR,
    
    /* Implementation Defined */
    MS_SYSREG_CPUMERRSR,
    MS_SYSREG_L2MERRSR,
    MS_SYSREG_IL1DATA0,
    MS_SYSREG_IL1DATA1,
    MS_SYSREG_IL1DATA2,
    MS_SYSREG_IL1DATA3,
    MS_SYSREG_DL1DATA0,
    MS_SYSREG_DL1DATA1,
    MS_SYSREG_DL1DATA2,
    MS_SYSREG_DL1DATA3,
    MS_SYSREG_L2ACTLR,
    MS_SYSREG_L2CTLR,
    MS_SYSREG_L2ECTLR,
    MS_SYSREG_L2PFR,
    MS_SYSREG_ACTLR2,
    MS_SYSREG_CDBGDR0,
    MS_SYSREG_CDBGDR1,
    MS_SYSREG_CDBGDR2,
    MS_SYSREG_CDBGDCT,
    MS_SYSREG_CDBGICT,
    MS_SYSREG_CDBGDCD,
    MS_SYSREG_CDBGICD,
    MS_SYSREG_CDBGTD,
    MS_SYSREG_CBAR,
    MS_SYSREG_CPUACTLR,
    MS_SYSREG_CPUECTLR,
    
    /* Misc */
    MS_SYSREG_NOP01,
    MS_SYSREG_NOP02,
    MS_SYSREG_TPIDRPRW,
    MS_SYSREG_TPIDRURO,
    MS_SYSREG_TPIDRURW,
    MS_SYSREG_HTPIDR,
    
    /* Program Specific Register */
    MS_SYSREG_APSR,
    MS_SYSREG_CPSR,
    MS_SYSREG_SPSR,
    
    /* Special-purpose resigers */
    MS_SYSREG_CURRENTEL,
    MS_SYSREG_DAIF,
    MS_SYSREG_FPCR,
    MS_SYSREG_FPSR,
    MS_SYSREG_NZCV,
    MS_SYSREG_SPSEL,
    
};

/* GICv2 CPU Interface */
#define MS_GIC_GICC_CTLR            0x0
#define MS_GIC_GICC_PMR             0x4
#define MS_GIC_GICC_BPR             0x8
#define MS_GIC_GICC_IAR             0xC
#define MS_GIC_GICC_EOIR            0x10
#define MS_GIC_GICC_RPR             0x14
#define MS_GIC_GICC_HPPIR           0x18
#define MS_GIC_GICC_ABPR            0x1C
#define MS_GIC_GICC_AIAR            0x20
#define MS_GIC_GICC_AEOIR           0x24
#define MS_GIC_GICC_AHPPIR          0x28
#define MS_GIC_GICC_RESERVED_00     0x2C // ranged
#define MS_GIC_GICC_IMPL_DEF_00     0x40 // ranged
#define MS_GIC_GICC_APR             0xD0 // ranged
#define MS_GIC_GICC_NSAPR           0xE0 // ranged
#define MS_GIC_GICC_RESERVED_01     0xF0 // ranged
#define MS_GIC_GICC_IIDR            0xFC
#define MS_GIC_GICC_DIR             0x1000

/* GICv2 Distributor Interface */
#define MS_GIC_GICD_CTLR        0x0
#define MS_GIC_GICD_TYPER       0x4
#define MS_GIC_GICD_IIDR        0x8
#define MS_GIC_GICD_RESERVED_00 0xC   // ranged
#define MS_GIC_GICD_IMP_DEF_00  0x20  // ranged
#define MS_GIC_GICD_RESERVED_01 0x40  // ranged
#define MS_GIC_GICD_IGROUPR     0x80  // ranged
#define MS_GIC_GICD_ISENABLER   0x100 // ranged
#define MS_GIC_GICD_ISPENDR     0x200 // ranged

/*
#define MS_GIC_GICD_RESERVED_01 0x10
#define MS_GIC_GICD_RESERVED_02 0x14
#define MS_GIC_GICD_RESERVED_03 0x18
#define MS_GIC_GICD_RESERVED_04 0x1C
*/

/*
#define MS_GIC_GICD_IMP_DEF_01  0x24
#define MS_GIC_GICD_IMP_DEF_02  0x28
#define MS_GIC_GICD_IMP_DEF_03  0x2C
#define MS_GIC_GICD_IMP_DEF_04  0x30
#define MS_GIC_GICD_IMP_DEF_05  0x34
#define MS_GIC_GICD_IMP_DEF_06  0x38
#define MS_GIC_GICD_IMP_DEF_07  0x3C
*/

/*
#define MS_GIC_GICD_RESERVED_06 0x44
#define MS_GIC_GICD_RESERVED_07 0x48
#define MS_GIC_GICD_RESERVED_08 0x4C
#define MS_GIC_GICD_RESERVED_09 0x50
#define MS_GIC_GICD_RESERVED_10 0x54
#define MS_GIC_GICD_RESERVED_11 0x58
#define MS_GIC_GICD_RESERVED_12 0x5C
#define MS_GIC_GICD_RESERVED_13 0x60
#define MS_GIC_GICD_RESERVED_14 0x64
#define MS_GIC_GICD_RESERVED_15 0x68
#define MS_GIC_GICD_RESERVED_16 0x6C
#define MS_GIC_GICD_RESERVED_17 0x70
#define MS_GIC_GICD_RESERVED_18 0x74
#define MS_GIC_GICD_RESERVED_19 0x78
#define MS_GIC_GICD_RESERVED_20 0x7C
*/

/*
#define MS_GIC_GICD_IGROUPR1 0x84
#define MS_GIC_GICD_IGROUPR2 0x88
#define MS_GIC_GICD_IGROUPR3 0x8C
#define MS_GIC_GICD_IGROUPR4 0x90
#define MS_GIC_GICD_IGROUPR5 0x94
#define MS_GIC_GICD_IGROUPR6 0x98
#define MS_GIC_GICD_IGROUPR7 0x9C
#define MS_GIC_GICD_IGROUPR8 0xA0
#define MS_GIC_GICD_IGROUPR9 0xA4
#define MS_GIC_GICD_IGROUPR10 0xA8
#define MS_GIC_GICD_IGROUPR11 0xAC
#define MS_GIC_GICD_IGROUPR12 0xB0
#define MS_GIC_GICD_IGROUPR13 0xB4
#define MS_GIC_GICD_IGROUPR14 0xB8
#define MS_GIC_GICD_IGROUPR15 0xBC
#define MS_GIC_GICD_IGROUPR16 0xC0
#define MS_GIC_GICD_IGROUPR17 0xC4
#define MS_GIC_GICD_IGROUPR18 0xC8
#define MS_GIC_GICD_IGROUPR19 0xCC
#define MS_GIC_GICD_IGROUPR20 0xD0
#define MS_GIC_GICD_IGROUPR21 0xD4
#define MS_GIC_GICD_IGROUPR22 0xD8
#define MS_GIC_GICD_IGROUPR23 0xDC
#define MS_GIC_GICD_IGROUPR24 0xE0
#define MS_GIC_GICD_IGROUPR25 0xE4
#define MS_GIC_GICD_IGROUPR26 0xE8
#define MS_GIC_GICD_IGROUPR27 0xEC
#define MS_GIC_GICD_IGROUPR28 0xF0
#define MS_GIC_GICD_IGROUPR29 0xF4
#define MS_GIC_GICD_IGROUPR30 0xF8
#define MS_GIC_GICD_IGROUPR31 0xFC
*/


/*
#define MS_GIC_GICD_ISENABLER1 0x104
#define MS_GIC_GICD_ISENABLER2 0x108
#define MS_GIC_GICD_ISENABLER3 0x10C
#define MS_GIC_GICD_ISENABLER4 0x110
#define MS_GIC_GICD_ISENABLER5 0x114
#define MS_GIC_GICD_ISENABLER6 0x118
#define MS_GIC_GICD_ISENABLER7 0x11C
#define MS_GIC_GICD_ISENABLER8 0x120
#define MS_GIC_GICD_ISENABLER9 0x124
#define MS_GIC_GICD_ISENABLER10 0x128
#define MS_GIC_GICD_ISENABLER11 0x12C
#define MS_GIC_GICD_ISENABLER12 0x130
#define MS_GIC_GICD_ISENABLER13 0x134
#define MS_GIC_GICD_ISENABLER14 0x138
#define MS_GIC_GICD_ISENABLER15 0x13C
#define MS_GIC_GICD_ISENABLER16 0x140
#define MS_GIC_GICD_ISENABLER17 0x144
#define MS_GIC_GICD_ISENABLER18 0x148
#define MS_GIC_GICD_ISENABLER19 0x14C
#define MS_GIC_GICD_ISENABLER20 0x150
#define MS_GIC_GICD_ISENABLER21 0x154
#define MS_GIC_GICD_ISENABLER22 0x158
#define MS_GIC_GICD_ISENABLER23 0x15C
#define MS_GIC_GICD_ISENABLER24 0x160
#define MS_GIC_GICD_ISENABLER25 0x164
#define MS_GIC_GICD_ISENABLER26 0x168
#define MS_GIC_GICD_ISENABLER27 0x16C
#define MS_GIC_GICD_ISENABLER28 0x170
#define MS_GIC_GICD_ISENABLER29 0x174
#define MS_GIC_GICD_ISENABLER30 0x178
#define MS_GIC_GICD_ISENABLER31 0x17C
#define MS_GIC_GICD_ICENABLER0 0x180
#define MS_GIC_GICD_ICENABLER1 0x184
#define MS_GIC_GICD_ICENABLER2 0x188
#define MS_GIC_GICD_ICENABLER3 0x18C
#define MS_GIC_GICD_ICENABLER4 0x190
#define MS_GIC_GICD_ICENABLER5 0x194
#define MS_GIC_GICD_ICENABLER6 0x198
#define MS_GIC_GICD_ICENABLER7 0x19C
#define MS_GIC_GICD_ICENABLER8 0x1A0
#define MS_GIC_GICD_ICENABLER9 0x1A4
#define MS_GIC_GICD_ICENABLER10 0x1A8
#define MS_GIC_GICD_ICENABLER11 0x1AC
#define MS_GIC_GICD_ICENABLER12 0x1B0
#define MS_GIC_GICD_ICENABLER13 0x1B4
#define MS_GIC_GICD_ICENABLER14 0x1B8
#define MS_GIC_GICD_ICENABLER15 0x1BC
#define MS_GIC_GICD_ICENABLER16 0x1C0
#define MS_GIC_GICD_ICENABLER17 0x1C4
#define MS_GIC_GICD_ICENABLER18 0x1C8
#define MS_GIC_GICD_ICENABLER19 0x1CC
#define MS_GIC_GICD_ICENABLER20 0x1D0
#define MS_GIC_GICD_ICENABLER21 0x1D4
#define MS_GIC_GICD_ICENABLER22 0x1D8
#define MS_GIC_GICD_ICENABLER23 0x1DC
#define MS_GIC_GICD_ICENABLER24 0x1E0
#define MS_GIC_GICD_ICENABLER25 0x1E4
#define MS_GIC_GICD_ICENABLER26 0x1E8
#define MS_GIC_GICD_ICENABLER27 0x1EC
#define MS_GIC_GICD_ICENABLER28 0x1F0
#define MS_GIC_GICD_ICENABLER29 0x1F4
#define MS_GIC_GICD_ICENABLER30 0x1F8
#define MS_GIC_GICD_ICENABLER31 0x1FC
*/



/*
#define MS_GIC_GICD_ISPENDR1 0x204
#define MS_GIC_GICD_ISPENDR2 0x208
#define MS_GIC_GICD_ISPENDR3 0x20C
#define MS_GIC_GICD_ISPENDR4 0x210
#define MS_GIC_GICD_ISPENDR5 0x214
#define MS_GIC_GICD_ISPENDR6 0x218
#define MS_GIC_GICD_ISPENDR7 0x21C
#define MS_GIC_GICD_ISPENDR8 0x220
#define MS_GIC_GICD_ISPENDR9 0x224
#define MS_GIC_GICD_ISPENDR10 0x228
#define MS_GIC_GICD_ISPENDR11 0x22C
#define MS_GIC_GICD_ISPENDR12 0x230
#define MS_GIC_GICD_ISPENDR13 0x234
#define MS_GIC_GICD_ISPENDR14 0x238
#define MS_GIC_GICD_ISPENDR15 0x23C
#define MS_GIC_GICD_ISPENDR16 0x240
#define MS_GIC_GICD_ISPENDR17 0x244
#define MS_GIC_GICD_ISPENDR18 0x248
#define MS_GIC_GICD_ISPENDR19 0x24C
#define MS_GIC_GICD_ISPENDR20 0x250
#define MS_GIC_GICD_ISPENDR21 0x254
#define MS_GIC_GICD_ISPENDR22 0x258
#define MS_GIC_GICD_ISPENDR23 0x25C
#define MS_GIC_GICD_ISPENDR24 0x260
#define MS_GIC_GICD_ISPENDR25 0x264
#define MS_GIC_GICD_ISPENDR26 0x268
#define MS_GIC_GICD_ISPENDR27 0x26C
#define MS_GIC_GICD_ISPENDR28 0x270
#define MS_GIC_GICD_ISPENDR29 0x274
#define MS_GIC_GICD_ISPENDR30 0x278
#define MS_GIC_GICD_ISPENDR31 0x27C
*/

#endif
