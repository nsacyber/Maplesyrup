#include <stdio.h> 
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <setjmp.h>

#include "ms_error.h"
#include "cmd_handler.h"
#include "raw_results.h"
#include "ms_debug.h"

int ms_reg_handlers = -1;
unsigned int g_illsig = 0;
struct sigaction saved_sigaction;
sigjmp_buf ret_to_buffer;

void
ms_signal_handler(int signo)
{
    switch(signo)
    {
        case SIGILL:
            g_illsig = 1;
            siglongjmp(ret_to_buffer, 1);
            break;
        default:
            break;
    }
}

/* 1 out -- 64 bit */
static
void
try_command_1_32(
    void (*pfunc)(unsigned int * val),
    unsigned int * val)
{   
    if (pfunc == NULL)
    {
        goto done;
    }
    if (sigsetjmp(ret_to_buffer, 1) == 0)
    {
        pfunc(val);
    }
done:
    return;
}

static
void
try_command_1_64(
    void (*pfunc)(unsigned long long * val),
    unsigned long long * val)
{   
    if (pfunc == NULL)
    {
        goto done;
    }
    if (sigsetjmp(ret_to_buffer, 1) == 0)
    {
        pfunc(val);
    }
done:
    return;
}

/* 2 out */
static
void
try_command_2_32(
    void (*pfunc)(unsigned int * val1, unsigned int * val2),
    unsigned int * val1,
    unsigned int * val2)
{   
    if (pfunc == NULL)
    {
        goto done;
    }
    if (sigsetjmp(ret_to_buffer, 1) == 0)
    {
        pfunc(val1, val2);
    }
done:
    return;
}

static
void
try_command_arch_02(
    void (*pfunc)(retdatatype * val1, unsigned int * val2),
    retdatatype * val1,
    unsigned int * val2)
{   
    if (pfunc == NULL)
    {
        goto done;
    }
    if (sigsetjmp(ret_to_buffer, 1) == 0)
    {
        pfunc(val1, val2);
    }
done:
    return;
}

int
register_signal_handlers()
{
    int result = -1;
    struct sigaction new_sigaction;   
    
    memset(&new_sigaction, 0x0, sizeof(struct sigaction));
    memset(&saved_sigaction, 0x0, sizeof(struct sigaction));
    new_sigaction.sa_handler = ms_signal_handler;
        
    /* Illegal instruction */
    if (sigaction(SIGILL, &new_sigaction, &saved_sigaction) == -1)
    {
        TRACE("Failed to register signal\n", 0);
        goto done;
    }
    ms_reg_handlers = 0;
    g_illsig = 0;
    result = 0;
    
done:
    return result;
}

void
restore_signal_handlers()
{   
    if (ms_reg_handlers == 0)
    {
        if (sigaction(SIGILL, &saved_sigaction, NULL) == -1)
        {
            TRACE("Failed to restore signal handler\n", 0);
        } 
        ms_reg_handlers = -1;
    }
}


static
int
find_function_index(
    unsigned int ioctl,
    unsigned int * index_out)
{
    int result = -1;
    unsigned int i = 0;
    int index_local = -1;
    
   
    TRACE("Entries in func_ptr_table: %d\n", return_table_size());
    for (i = 0; i < return_table_size(); ++i)
    {
        if (ioctl == func_table[i].code)
        {
            TRACE("Found function!: ioctl: 0x%x, entry: %d\n", func_table[i].code,  i);
            index_local = i;
            break;
        }
    }
    
    if (index_local == -1)
    {
        TRACE("Could not find function ptr\n", 0);
        goto done;
    }
    
    result = 0;
    *index_out = index_local;
    
done:
    return result;
}

static
void
set_status_flag(co15_result * out)
{
    if (out == NULL)
    {
        goto done;
    }   
    
    if (g_illsig >= 1)
    {
        TRACE("Setting flag to fail\n", 0);
        out->status = MS_ERROR_ILLINST;
        g_illsig = 0;
    }
    else
    {
        TRACE("Setting flag to success\n", 0);
        out->status = MS_ERROR_SUCCESS;
    }

done:
    return;
}

static
int
call_func_ptr(
    unsigned int func_index,
    co15_result * result_out)
{   
    int result = -1;    
        
    func_ptr_arch_01 ptr_arch_01 = NULL;
    func_ptr_arch_02 ptr_arch_02 = NULL;
    func_ptr_64_01 ptr_64_01 = NULL;
    unsigned long long cmd_result_64 = 0;   
    func_ptr_32_01 ptr_32_01 = NULL;
    func_ptr_32_02 ptr_32_02 = NULL;
    unsigned int cmd_result_1 = 0;
    unsigned int cmd_result_2 = 0;  
    
    if (result_out == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    /* zero out result structure */ 
    result_out->cpu = 0;
    result_out->midr = 0;
    result_out->out64 = 0;
    result_out->out1 = 0;
    result_out->out2 = 0;   

    
    if (func_index >= return_table_size())
    {
        TRACE("table index error\n", 0);
        goto done;
    }
    
    /* Cast to correct function type based off table data */    
    
    switch (func_table[func_index].function_prototype)
    {
        case FUNCTYPE_OUT_1:
            switch (func_table[func_index].data_size)
            {
                case FUNCTYPE_BITS_32:
                    /* 32 bit register */
                    ptr_32_01 = (func_ptr_32_01)(func_table[func_index].func_ptr);
                    try_command_1_32(ptr_32_01, &cmd_result_1);
                    result_out->out1 = cmd_result_1;
                    set_status_flag(result_out);
                    break;
                case FUNCTYPE_BITS_64:
                    /* 64 bit register */
                    ptr_64_01 = (func_ptr_64_01)(func_table[func_index].func_ptr);
                    try_command_1_64(ptr_64_01, &cmd_result_64);
                    result_out->out64 = cmd_result_64;
                    set_status_flag(result_out);
                    break;
                case FUNCTYPE_BITS_ARCH:
                    /* arch dependent sized register */
                    ptr_arch_01 = (func_ptr_arch_01)(func_table[func_index].func_ptr);
#ifdef __aarch64__
                    try_command_1_64(ptr_arch_01, &cmd_result_64);
                    result_out->out64 = cmd_result_64;
#else
                    try_command_1_32(ptr_arch_01, &cmd_result_1);                   
                    result_out->out1 = cmd_result_1;
#endif
                    set_status_flag(result_out);
                    break;                  
                default:
                    result_out->status = MS_ERROR_TABLE;
                    break;                  
            }           
            break;
            
        case FUNCTYPE_OUT_2:        
            switch (func_table[func_index].data_size)
            {
                case FUNCTYPE_BITS_32:
                    ptr_32_02 = (func_ptr_32_02)(func_table[func_index].func_ptr);
                    try_command_2_32(ptr_32_02, &cmd_result_1, &cmd_result_2);
                    result_out->out1 = cmd_result_1;
                    result_out->out2 = cmd_result_2;
                    set_status_flag(result_out);
                    break;
                                        
                case FUNCTYPE_BITS_ARCH:
                    ptr_arch_02 = (func_ptr_arch_02)(func_table[func_index].func_ptr);                  
#ifdef __aarch64__
                    try_command_arch_02(ptr_arch_02, &cmd_result_64, &cmd_result_2);
                    result_out->out64 = cmd_result_64;                  
#else
                    try_command_arch_02(ptr_arch_02, &cmd_result_1, &cmd_result_2);             
                    result_out->out1 = cmd_result_1;                    
#endif      
                    result_out->out2 = cmd_result_2;
                    set_status_flag(result_out);
                    break;
                    
                default:
                    result_out->status = MS_ERROR_TABLE;
                    break;
            }
            break;
        default:
            result_out->status = MS_ERROR_TABLE;
            break;          
    }
    
    result = 0;
done:    
    return result;  
}


/******************************************************
 * Passes the results to the caller
 *  
 * @param [in] cmd IOCTL code 
 * @param [out] Results from the processor
 * 
 * ***************************************************/
int
usermode_results(
    unsigned int cmd,
    co15_result * result_out)
{
    int result = -1;
    unsigned int index_out = -1;
    if (result_out == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }       
    /* initialize status */
    result_out->status = MS_ERROR_NORESULT;
    if (find_function_index(cmd, &index_out) != 0)
    {
        TRACE("failed to locate function ptr\n", 0);
        goto done;
    }

    if (call_func_ptr(index_out, result_out) != 0)
    {
         TRACE("failed to call function ptr\n", 0);
         goto done;
    }
        
    result = 0;
    
done:
    return result;
}
