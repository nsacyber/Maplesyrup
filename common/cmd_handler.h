#ifndef _CMD_HANDLER_H
#define _CMD_HANDLER_H

#include "parse.h"

#ifdef __KERNEL__

#define MS_CPU_ALL 0xFFFFFFFF

int call_cmd(void * buffer, int (*pcall)(ms_data_cpu * result_out));
int call_func_return_results(ms_data_cpu * result_out);
int gic_return_results(ms_data_cpu * result_out);

/* To facilitate 'on_each_cpu_kthread' call */
typedef struct co15_result_cpu_wrapper
{
    unsigned int found_index;
    void * base_addr;
    unsigned int pcpu;  
    ms_data_cpu results;
} co15_result_cpu_wrapper;

int unpack_result_wrapper(ms_data_cpu * userin, co15_result_cpu_wrapper * kernelout);
int repack_result_wrapper(co15_result_cpu_wrapper * kernelin, ms_data_cpu * userout);
int on_each_cpu_kick(void (*funcptr)(void * funcdata), co15_result_cpu_wrapper * data);

#else
int usermode_results(unsigned int cmd, ms_data * result_out);
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
