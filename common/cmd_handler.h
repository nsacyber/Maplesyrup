#ifndef _CMD_HANDLER_H
#define _CMD_HANDLER_H

#include "parse.h"

#ifdef __KERNEL__

struct undef_hook; // forward declare

#define MS_CPU_ALL 0xFFFFFFFF

typedef void (*register_undef_hook_t)(struct undef_hook * hook);
typedef void (*unregister_undef_hook_t)(struct undef_hook * hook);

int call_cmd_co(void * result);
void ms_del_undef_instr_hook(void);
int ms_add_undef_instr_hook(void);
int ms_find_undef_hook(void);

/* To facilitate 'on_each_cpu_kthread' call */
typedef struct co15_result_cpu_wrapper
{
    unsigned int found_index;
    unsigned int pcpu;  
    co15_result_cpu results;
} co15_result_cpu_wrapper;

#else
int usermode_results(unsigned int cmd, co15_result * result_out);
int register_signal_handlers();
void restore_signal_handlers();
#endif

enum
{
    /* number of output params */
    FUNCTYPE_OUT_1 = 0, 
    FUNCTYPE_OUT_2,
    
    /* size of output params */
    FUNCTYPE_BITS_32,
    FUNCTYPE_BITS_64,
    
    FUNCTYPE_BITS_ARCH, 
};

#ifdef __aarch64__
typedef unsigned long long retdatatype;
#else
typedef unsigned int retdatatype;
#endif

typedef struct func_table_entry {
    unsigned int code;
    void * func_ptr;
    unsigned int function_prototype;
    unsigned int data_size;
} func_table_entry;


typedef void (*func_ptr_arch_01)(retdatatype * out);
typedef void (*func_ptr_arch_02)(retdatatype * out, unsigned int * out2);
typedef void (*func_ptr_32_01)(unsigned int * out1);
typedef void (*func_ptr_32_02)(unsigned int * out1, unsigned int * out2);
typedef void (*func_ptr_64_01)(unsigned long long * out64);
typedef void (*func_ptr_64_02)(unsigned long long * out64, unsigned long long * out2);


extern func_table_entry func_table[];
int return_table_size(void);

#ifdef __aarch64__
    #define ASM_01(instruction) asm volatile(instruction)
    #define ASM_02(inst1, inst2) asm volatile(inst1, inst2)
    #define ASM_FP(instruction) asm volatile(instruction)
#else
    #define ASM_01(instruction) asm volatile(".arch armv7a\n\t" instruction)
    #define ASM_02(inst1, inst2) asm volatile(".arch armv7a\n\t" inst1, inst2)
    #define ASM_FP(instruction) asm volatile(".fpu vfpv3-d16\n\t" instruction)
#endif

#endif
