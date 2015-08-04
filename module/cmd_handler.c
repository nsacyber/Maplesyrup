#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/smp.h>
#include <linux/cpu.h>
#include <linux/spinlock.h>

#include "cmd_handler.h"
#include "parse.h"
#include "undef_hook.h"

#include "ms_debug.h"
#include "ms_error.h"

extern unsigned int g_undef_flag;
extern spinlock_t g_undef_flag_lock;

//#pragma GCC diagnostic ignored "-Wunused-function"
//#pragma GCC diagnostic ignored "-Wunused-variable"

/******************************************************
 * Copy user mode structure to kernel mode structure
 *  
 * @param [in] userin Input data
 * @param [out] kernelout Output data
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int
unpack_result_wrapper(
    ms_data_cpu * userin,
    co15_result_cpu_wrapper * kernelout)
{
    unsigned int i = 0;
    int result = -1;
    if ((userin == NULL) || (kernelout == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    /* copy user structure values to kernel version of the 
     * same structure */     
     for (i = 0; i < MS_MAX_CPUS; ++i)
     {       
         kernelout->results.cpu[i].in1 = userin->cpu[i].in1;
         kernelout->results.cpu[i].in2 = userin->cpu[i].in2;
         kernelout->results.cpu[i].out64 = userin->cpu[i].out64;
         kernelout->results.cpu[i].out1 = userin->cpu[i].out1;
         kernelout->results.cpu[i].out2 = userin->cpu[i].out2;
         kernelout->results.cpu[i].cpu = userin->cpu[i].cpu;
         kernelout->results.cpu[i].midr = userin->cpu[i].midr;
         kernelout->results.cpu[i].status  = userin->cpu[i].status;
     }
     kernelout->pcpu = userin->pcpu;
     kernelout->results.code = userin->code;
       
    result = 0;
done:
    return result;
}

/******************************************************
 * Copy kernel mode structure back to user mode
 * structure
 *  
 * @param [in] kernelin Input data
 * @param [out] userout Output data
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int
repack_result_wrapper(
    co15_result_cpu_wrapper * kernelin,
    ms_data_cpu * userout)
{
    int result = -1;
    unsigned int i = 0;
    if ((kernelin == NULL) || (userout == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    /* copy kernel structure back to usermode structure */   
    for (i = 0; i < MS_MAX_CPUS; ++i)
    {           
        userout->cpu[i].in1 = kernelin->results.cpu[i].in1;
        userout->cpu[i].in2 = kernelin->results.cpu[i].in2;
        userout->cpu[i].out64 = kernelin->results.cpu[i].out64;
        userout->cpu[i].out1 = kernelin->results.cpu[i].out1;
        userout->cpu[i].out2 = kernelin->results.cpu[i].out2;   
        userout->cpu[i].cpu =  kernelin->results.cpu[i].cpu;
        userout->cpu[i].midr =  kernelin->results.cpu[i].midr;
        userout->cpu[i].status =  kernelin->results.cpu[i].status;
    }
    
    result = 0;
    
done:
    return result;  
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
    
    for (i = 0; i < return_table_size(); i++)
    {
        if (ioctl == func_table[i].code)
        {
            TRACE("Found function!: ioctl: 0x%x, entry: %d/%d\n",
                        func_table[i].code, i, return_table_size());
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
get_midr(unsigned int * val)
{
#ifdef __aarch64__
    asm volatile("MRS %0, MIDR_EL1" : "+r" (*val));
#else
    asm volatile("MRC p15, 0, %0, c0, c0, 0" : "+r" (*val));
#endif
}

static
void
call_func_ptr(
    void * result_out)
{   
    unsigned int cpu_index = 0;
    unsigned int func_index = 0;
    ms_data * co15_result_entry = NULL;
    co15_result_cpu_wrapper * result_out_cast = NULL;
    unsigned int midr_result = 0;
    unsigned long long cmd_result_64 = 0;   
    unsigned int cmd_result_1 = 0;
    unsigned int cmd_result_2 = 0;  
    
    if (result_out == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    result_out_cast = (co15_result_cpu_wrapper *)result_out;

#ifdef CONFIG_SMP
    cpu_index = get_cpu();
#else
    cpu_index = 0;
#endif

    func_index = result_out_cast->found_index;  
    
    co15_result_entry = &result_out_cast->results.cpu[cpu_index];   
    
    /* zero out result structure */
    co15_result_entry->out64 = 0;
    co15_result_entry->out1 = 0;
    co15_result_entry->out2 = 0;    

    /* Save CPU & midr info */
    co15_result_entry->cpu = cpu_index; 
    get_midr(&midr_result); 
    co15_result_entry->midr = midr_result;
    
    /* Cast to correct function type based off table data */    
    switch (func_table[func_index].function_prototype)
    {
        case FUNCTYPE_OUT_1:
            switch (func_table[func_index].data_size)
            {
                case FUNCTYPE_BITS_32:
                    /* 32 bit register */                    
                    ((func_ptr_32_01)(func_table[func_index].func_ptr))(&cmd_result_1);                  
                    co15_result_entry->out1 = cmd_result_1;
                    set_undef_flag(co15_result_entry);
                    break;
                case FUNCTYPE_BITS_64:
                    /* 64 bit register */
                    ((func_ptr_64_01)(func_table[func_index].func_ptr))(&cmd_result_64);                    
                    co15_result_entry->out64 = cmd_result_64;
                    set_undef_flag(co15_result_entry);
                    break;
                case FUNCTYPE_BITS_ARCH:
                    /* arch dependent sized register */                    
#ifdef __aarch64__
                    ((func_ptr_arch_01)(func_table[func_index].func_ptr))(&cmd_result_64);                    
                    co15_result_entry->out64 = cmd_result_64;
#else
                    ((func_ptr_arch_01)(func_table[func_index].func_ptr))(&cmd_result_1);
                    co15_result_entry->out1 = cmd_result_1;
#endif
                    set_undef_flag(co15_result_entry);
                    break;                  
                default:
                    co15_result_entry->status = MS_ERROR_TABLE;
                    break;                  
            }           
            break;
            
        case FUNCTYPE_OUT_2:         
            switch (func_table[func_index].data_size)
            {
                case FUNCTYPE_BITS_32:                    
                    ((func_ptr_32_02)(func_table[func_index].func_ptr))(&cmd_result_1, &cmd_result_2);
                    co15_result_entry->out1 = cmd_result_1;
                    co15_result_entry->out2 = cmd_result_2;
                    set_undef_flag(co15_result_entry);
                    break;
                    
                case FUNCTYPE_BITS_ARCH:
#ifdef __aarch64__
                    ((func_ptr_arch_02)(func_table[func_index].func_ptr))(&cmd_result_64, &cmd_result_2);
                    co15_result_entry->out64 = cmd_result_64;
#else
                    ((func_ptr_arch_02)(func_table[func_index].func_ptr))(&cmd_result_1, &cmd_result_2);
                    co15_result_entry->out1 = cmd_result_1;
#endif
                    co15_result_entry->out2 = cmd_result_2;
                    set_undef_flag(co15_result_entry);
                    break;
                    
                default:
                    co15_result_entry->status = MS_ERROR_TABLE;
                    break;
            }
            break;
        default:
            co15_result_entry->status = MS_ERROR_TABLE;
            break;          
    }
#ifdef CONFIG_SMP
    put_cpu();
#endif   
            
done:    
    return; 
}

static
int
cpu_online_wrapper(unsigned int cpu,
                    void (*threadfn)(void * data),
                    void * data)
{   
    int result = -1;
    co15_result_cpu_wrapper * data_cast = NULL;
    ms_data * entry = NULL;
        
    if ((threadfn == NULL) || (data == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    
    data_cast = (co15_result_cpu_wrapper *)data;
    entry = &data_cast->results.cpu[cpu];
    
    if (cpu >= MS_MAX_CPUS)
    {
        TRACE("CPU index too large\n", 0);
        entry->status = MS_ERROR_SMP;
        goto done;
    }
    
#ifdef CONFIG_SMP
    /* Bring CPU online */
    if (!cpu_online(cpu))
    {
        if (cpu_up(cpu))
        {
            TRACE("Could not bring CPU %d up\n", cpu);
            entry->status = MS_ERROR_SMP;
            goto done;
        }
        else
        {
            TRACE("CPU %d up\n", cpu);
        }
    }   
    
    if (smp_call_function_single(cpu, threadfn, data, 1))
    {
        TRACE("Function call fail\n", 0);
        entry->status = MS_ERROR_SMP_SINGLE;
        goto done;
    }   
#else
    threadfn(data);
#endif
    result = 0;
done:
    
    return result;
}

int
on_each_cpu_kick(void (*funcptr)(void * funcdata),
                    co15_result_cpu_wrapper * data)
{
    unsigned int i = 0;
    int status = -1;
    if ((data == NULL) || (funcptr == NULL))
    {
        goto done;
    }
    
    if (data->pcpu == MS_CPU_ALL)
    {
        // All CPUS
        for (i = 0; i < MS_MAX_CPUS; ++i)
        {   
            if (cpu_online_wrapper(i, funcptr, (void*)data) != 0)
            {
                TRACE("Problem calling function on cpu\n", 0);
            }
        }
    }
    else
    {
        if (data->pcpu > MS_MAX_CPUS)
        {
            TRACE("CPU index error\n", 0);
            goto done;
        }
        else
        {
            // Single CPU           
            if (cpu_online_wrapper(data->pcpu, funcptr, (void*)data) != 0)
            {
                TRACE("Problem calling function on cpu\n", 0);
            }
        }
    }
    status = 0;
    
done:
    return status;
}

/******************************************************
 * This function will find the requsted ioctl handler,
 * call the function, and package the results
 *  
 * @param [in] cmd IOCTL code 
 * @param [out] Result with either one or two values
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int
call_func_return_results(
    ms_data_cpu * result_out)
{
    int result = -1;
    unsigned int index_out = 0;
    co15_result_cpu_wrapper local_result;
    
    if (result_out == NULL)
    {
        goto done;
    }
    memset(&local_result, 0x0, sizeof(co15_result_cpu_wrapper));
    TRACE("Command received: %x\n", result_out->code);
    
    /* locate function in global table */   
    if (find_function_index(result_out->code, &index_out) != 0)
    {
        TRACE("Failed to find function\n", 0);
        goto done;      
    }
    TRACE("Found function\n", 0);
    
    /* convert structure to temporary kernel
     * version */   
    if (unpack_result_wrapper(result_out, &local_result) != 0)
    {
        TRACE("Failed to unpack\n", 0);
        goto done;
    }
    local_result.found_index = index_out;
    
    /* call function and get results */     
    if (on_each_cpu_kick(call_func_ptr, &local_result) != 0)
    {
        TRACE("Failed on_each_cpu_kick\n", 0);
        goto done;
    }   

    /* convert back to usermode version */  
    if (repack_result_wrapper(&local_result, result_out) != 0)
    {
        TRACE("Failed to repack results\n", 0);
        goto done;
    }
    TRACE("Function called successfully and results saved\n", 0);
    
    
    result = 0;
done:
    return result;
}

/******************************************************
 * Locates the requested IOCTL and packages the results
 *  
 * @param [out] Result with either one or two values
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int
call_cmd(void * buffer,
            int (*pcall)(ms_data_cpu * result_out))
{
    int result = -1;
    ms_data_cpu * user_buffer = NULL;
    ms_data_cpu user_buffer_local;
    
    if ((buffer == NULL) || (pcall == NULL))
    {
        TRACE("param error\n", 0);
        goto done;
    }
    user_buffer = (ms_data_cpu *)buffer;    
    
    spin_lock_init(&g_undef_flag_lock);
    spin_lock(&g_undef_flag_lock);
    g_undef_flag = 0;    
    spin_unlock(&g_undef_flag_lock);
    
    memset(&user_buffer_local, 0x0, sizeof(ms_data_cpu));
        
    /* Get local kernel copy */ 
    if (copy_from_user(&user_buffer_local, user_buffer, sizeof(ms_data_cpu)) != 0)
    {
        TRACE("Could not get data from userspace: %d\n", result);
        goto done;
    }
        
    /* Call function & get results */
    if (pcall(&user_buffer_local) != 0)
    {
        TRACE("Could not call function & get results\n", 0);
        goto done;
    }
    
    /* Copy results back to user space */   
    if (copy_to_user(user_buffer, &user_buffer_local, sizeof(ms_data_cpu)) != 0)
    {
        TRACE("Could not send data back to userspace: %d\n", result);
        goto done;
    }
    
    result = 0;
    
done:
    return result;
}
