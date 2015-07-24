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


typedef struct co15_result
{
    unsigned int cpu;
    unsigned int midr;
    unsigned int in1;
    unsigned int in2;
    unsigned long long out64;
    unsigned int out1;
    unsigned int out2;
    unsigned int status;
} co15_result;

typedef struct co15_result_cpu
{
    unsigned int code;
    unsigned int pcpu; //preferred cpu code
    co15_result cpu[MS_MAX_CPUS];
} co15_result_cpu;

/*******************
 * 
 * 
 * <-PARTNUMBER-><-INSTRUCTION-><-FUNCTIONAL-><-FIELD->
 * 
 * */

#define PARTNUMBER_BITS     8       // arm-v7a part number
#define INSTRUCTION_BITS    12      // unique informational registers
#define FUNCTIONAL_BITS 6           // functional group of register
#define FIELD_BITS      8           // holds the number of bitfields in register

#define MSCODE_FIELDSHIFT 0
#define MSCODE_FUNCTIONALSHIFT  (MSCODE_FIELDSHIFT + FIELD_BITS)
#define MSCODE_INSTRUCTIONSHIFT (MSCODE_FUNCTIONALSHIFT + FUNCTIONAL_BITS)
#define MSCODE_PARTNUMBERSHIFT  (MSCODE_INSTRUCTIONSHIFT + INSTRUCTION_BITS)

#define PARTNUMBER_MASK     ((1 << MSCODE_PARTNUMBERSHIFT) - 1)
#define INSTRUCTION_MASK    ((1 << MSCODE_INSTRUCTIONSHIFT) - 1)
#define FUNCTIONAL_MASK     ((1 << MSCODE_FUNCTIONALSHIFT) - 1)
#define FIELD_MASK          ((1 << MSCODE_FIELDSHIFT) - 1)

/* To create a code */
#define MSCODE(partnumber, instruction, functional, field) \
        (((partnumber) << MSCODE_PARTNUMBERSHIFT) | \
            ((instruction) << MSCODE_INSTRUCTIONSHIFT) | \
            ((functional) << MSCODE_FUNCTIONALSHIFT) | \
            ((field) << MSCODE_FIELDSHIFT))

/* To decode */
#define MSCODE_PARTNUMBER(code)     (((code) >> MSCODE_PARTNUMBERSHIFT) & PARTNUMBER_MASK)
#define MSCODE_INSTRUCTION(code)    (((code) >> MSCODE_INSTRUCTIONSHIFT) & INSTRUCTION_MASK)
#define MSCODE_FUNCTION(code)       (((code) >> MSCODE_FUNCTIONALSHIFT) & FUNCTIONAL_MASK)
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
    unsigned int bitfield_code; // bitfield level
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
    unsigned int calc;	// calc_value is valid
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
    unsigned int table_array_index;
    unsigned int value;
} table_index;

enum
{
    /* Identification Registers function group */
    MS_IOCTL_PFR0 = 0,
    MS_IOCTL_PFR1,
    MS_IOCTL_DFR0,
    MS_IOCTL_AFR0,
    MS_IOCTL_MMFR0,
    MS_IOCTL_MMFR1,
    MS_IOCTL_MMFR2,
    MS_IOCTL_MMFR3,
    MS_IOCTL_MMFR4,
    MS_IOCTL_ISAR0,
    MS_IOCTL_ISAR1,
    MS_IOCTL_ISAR2,
    MS_IOCTL_ISAR3,
    MS_IOCTL_ISAR4,
    MS_IOCTL_ISAR5, 
    MS_IOCTL_REVIDR,
    MS_IOCTL_TLBTR, 
    MS_IOCTL_CCSIDR,
    MS_IOCTL_TCMTR,
    MS_IOCTL_AIDR,
    MS_IOCTL_CLIDR,
    MS_IOCTL_CSSELR,
    MS_IOCTL_CTR,
    MS_IOCTL_MIDR,
    MS_IOCTL_MPIDR,
    MS_IOCTL_AA64AFR0,
    MS_IOCTL_AA64AFR1,
    MS_IOCTL_AA64DFR0,
    MS_IOCTL_AA64DFR1,
    MS_IOCTL_AA64ISAR0,
    MS_IOCTL_AA64ISAR1,
    MS_IOCTL_AA64MMFR0,
    MS_IOCTL_AA64MMFR1,
    MS_IOCTL_AA64PFR0,
    MS_IOCTL_AA64PFR1,
    
    /* Security Extensions functional group */
    MS_IOCTL_ISR,
    MS_IOCTL_MVBAR,
    MS_IOCTL_NSACR,
    MS_IOCTL_SCR,
    MS_IOCTL_SDER,
    MS_IOCTL_VBAR,  
    
    /* Virtual Memory Control functional group */   
    MS_IOCTL_NMRR,
    MS_IOCTL_PRRR,
    MS_IOCTL_SCTLR,
    MS_IOCTL_TTBCR,
    MS_IOCTL_TTBR0,
    MS_IOCTL_TTBR0LPAE,
    MS_IOCTL_TTBR1,
    MS_IOCTL_TTBR1LPAE,
    MS_IOCTL_CONTEXTIDR,
    MS_IOCTL_AMAIR,
    MS_IOCTL_AMAIR0,
    MS_IOCTL_AMAIR1,
    MS_IOCTL_AMAIR2,
    MS_IOCTL_DACR,
    MS_IOCTL_MAIR,
    MS_IOCTL_MAIR0,
    MS_IOCTL_MAIR1,
    MS_IOCTL_DCZID,
    MS_IOCTL_TCR,
    
    /* Other System Control Registers */
    MS_IOCTL_ACTLR,
    MS_IOCTL_CPACR,
    MS_IOCTL_FCSEIDR,
    MS_IOCTL_RMR,
    MS_IOCTL_RVBAR,
    
    /* Performance Monitors */
    MS_IOCTL_PMCCFILTR,
    MS_IOCTL_PMCR,
    MS_IOCTL_PMCCNTR,
    MS_IOCTL_PMCEID0,
    MS_IOCTL_PMCEID1,   
    MS_IOCTL_PMCNTENSET,
    MS_IOCTL_PMCNTENCLR,
    MS_IOCTL_PMINTENSET,
    MS_IOCTL_PMINTENCLR,
    MS_IOCTL_PMOVSCLR,
    MS_IOCTL_PMOVSR,
    MS_IOCTL_PMOVSSET,
    MS_IOCTL_PMSELR,
    MS_IOCTL_PMSWINC,
    MS_IOCTL_PMUSERENR,
    MS_IOCTL_PMXEVCNTR,
    MS_IOCTL_PMXEVTYPER,    
    
    /* Fault Handling functional group */
    MS_IOCTL_IFAR,
    MS_IOCTL_IFSR,
    MS_IOCTL_ADFSR,
    MS_IOCTL_AIFSR,
    MS_IOCTL_DFAR,
    MS_IOCTL_DFSR,
    MS_IOCTL_AFSR0,
    MS_IOCTL_AFSR1,
    MS_IOCTL_ESR,
    MS_IOCTL_FAR,
    
    /* Address Translations */
    MS_IOCTL_PAR,
    
    /* Floating Point */
    MS_IOCTL_FPEXC,
    MS_IOCTL_FPSCR,
    MS_IOCTL_FPSID,
    MS_IOCTL_MVFR0,
    MS_IOCTL_MVFR1, 
    MS_IOCTL_MVFR2, 
    
    /* Debug */
    MS_IOCTL_DBGDIDR,
    MS_IOCTL_DBGAUTHSTATUS, 
    MS_IOCTL_DBGBCR,    //todo
    MS_IOCTL_DBGBVR,    //todo
    MS_IOCTL_DBGBXVR,   //todo
    MS_IOCTL_DBGCID0,   //todo
    MS_IOCTL_DBGCID1,   //todo
    MS_IOCTL_DBGCID2,   //todo
    MS_IOCTL_DBGCID3,   //todo
    MS_IOCTL_DBGCIDSR,  //todo
    MS_IOCTL_DBGCLAIMCLR,   //todo
    MS_IOCTL_DBGCLAIMSET,   //todo
    MS_IOCTL_DBGDEVID,  //todo
    MS_IOCTL_DBGDEVID1, //todo
    MS_IOCTL_DBGDEVTYPE,    //todo
    MS_IOCTL_DBGDRAR,
    MS_IOCTL_DBGDRCR,   //todo
    MS_IOCTL_DBGDSAR,   //todo
    MS_IOCTL_DBGDSCCR,  //todo
    MS_IOCTL_DBGDSCR,
    MS_IOCTL_DBGDSMCR,  //todo
    MS_IOCTL_DBGDTRRX,  //todo
    MS_IOCTL_DBGDTRTX,  //todo
    MS_IOCTL_DBGEACR,   //todo
    MS_IOCTL_DBGECR,    //todo
    MS_IOCTL_DBGITCTRL, //todo
    MS_IOCTL_DBGITR,    //todo
    MS_IOCTL_DBGLAR,    //todo
    MS_IOCTL_DBGLSR,    //todo
    MS_IOCTL_DBGOSDLR,  //todo
    MS_IOCTL_DBGOSLSR,  //todo
    MS_IOCTL_DBGOSSRR,  //todo
    MS_IOCTL_DBGPCSR,   //todo
    MS_IOCTL_DBGPID0,   //todo
    MS_IOCTL_DBGPID1,   //todo
    MS_IOCTL_DBGPID2,   //todo
    MS_IOCTL_DBGPID3,   //todo
    MS_IOCTL_DBGPID4,   //todo
    MS_IOCTL_DBGPRCR,   //todo
    MS_IOCTL_DBGPRSR,   //todo
    MS_IOCTL_DBGVCR,    //todo
    MS_IOCTL_DBGVIDSR,  //todo
    MS_IOCTL_DBGWCR,    //todo
    MS_IOCTL_DBGWFAR,   //todo
    MS_IOCTL_DBGWVR,    //todo
    
    /* Jazelle */
    MS_IOCTL_JIDR,
    MS_IOCTL_JMCR,
    MS_IOCTL_JOSCR,
    
    /* Thumb */
    MS_IOCTL_TEECR,
    MS_IOCTL_TEEHBR,
    
    /* Implementation Defined */
    MS_IOCTL_CPUMERRSR,
    MS_IOCTL_L2MERRSR,
    MS_IOCTL_IL1DATA0,
    MS_IOCTL_IL1DATA1,
    MS_IOCTL_IL1DATA2,
    MS_IOCTL_IL1DATA3,
    MS_IOCTL_DL1DATA0,
    MS_IOCTL_DL1DATA1,
    MS_IOCTL_DL1DATA2,
    MS_IOCTL_DL1DATA3,
    MS_IOCTL_L2ACTLR,
    MS_IOCTL_L2CTLR,
    MS_IOCTL_L2ECTLR,
    MS_IOCTL_L2PFR,
    MS_IOCTL_ACTLR2,
    MS_IOCTL_CDBGDR0,
    MS_IOCTL_CDBGDR1,
    MS_IOCTL_CDBGDR2,
    MS_IOCTL_CDBGDCT,
    MS_IOCTL_CDBGICT,
    MS_IOCTL_CDBGDCD,
    MS_IOCTL_CDBGICD,
    MS_IOCTL_CDBGTD,
    MS_IOCTL_CBAR,
    MS_IOCTL_CPUACTLR,
    MS_IOCTL_CPUECTLR,
    
    /* Misc */
    MS_IOCTL_NOP01,
    MS_IOCTL_NOP02,
    MS_IOCTL_TPIDRPRW,
    MS_IOCTL_TPIDRURO,
    MS_IOCTL_TPIDRURW,
    MS_IOCTL_HTPIDR,
    
    /* Program Specific Register */
    MS_IOCTL_APSR,
    MS_IOCTL_CPSR,
    MS_IOCTL_SPSR,
    
    /* Special-purpose resigers */
    MS_IOCTL_CURRENTEL,
    MS_IOCTL_DAIF,
    MS_IOCTL_FPCR,
    MS_IOCTL_FPSR,
    MS_IOCTL_NZCV,
    MS_IOCTL_SPSEL,
    
};


enum
{
    /* Functional groups */
    FUNCTION_DEBUG = 0,
    FUNCTION_TRANSLATE,
    FUNCTION_BARRIER,
    FUNCTION_CACHE, 
    FUNCTION_DMA,
    FUNCTION_FAULT,
    FUNCTION_FP,
    FUNCTION_TIMER,
    FUNCTION_ID,
    FUNCTION_JAZELLE,
    FUNCTION_MISC,
    FUNCTION_OTHER,
    FUNCTION_PERFMON,
    FUNCTION_SECEXT,
    FUNCTION_THUMB,
    FUNCTION_TLB_HYP,
    FUNCTION_TLB_NON_HYP,
    FUNCTION_VIRT_EXT,
    FUNCTION_VIRT_MEM,
    FUNCTION_IMPLEMENTATION,
    FUNCTION_PSR,
};

#endif
