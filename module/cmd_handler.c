#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/smp.h>
#include <linux/cpu.h>
#include <linux/string.h>
#include <linux/kallsyms.h>
#include <linux/spinlock.h>
#include <linux/list.h>

#include <asm/io.h>
#include <asm/traps.h>

#include "cmd_handler.h"
#include "parse.h"

#include "ms_debug.h"
#include "ms_error.h"

static unsigned int g_undef_flag;
static spinlock_t g_undef_flag_lock;
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
static
int
unpack_result_wrapper(
    co15_result_cpu * userin,
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
static
int
repack_result_wrapper(
    co15_result_cpu_wrapper * kernelin,
    co15_result_cpu * userout)
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
set_undef_flag(co15_result * out)
{
    if (out == NULL)
    {
        goto done;
    }   
    
    spin_lock(&g_undef_flag_lock);
    if (g_undef_flag == 1)
    {
        out->status = MS_ERROR_UNDEFINST;
        g_undef_flag = 0;
    }
    else
    {
        out->status = MS_ERROR_SUCCESS;
    }
    spin_unlock(&g_undef_flag_lock);
    
done:
    return;
}

static
void
call_func_ptr(
    void * result_out)
{   
    unsigned int cpu_index = 0;
    unsigned int func_index = 0;
    co15_result * co15_result_entry = NULL;
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
    co15_result * entry = NULL;
        
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
    
    if (smp_call_function_single(cpu, call_func_ptr, data, 1))
    {
        TRACE("Function call fail\n", 0);
        entry->status = MS_ERROR_SMP_SINGLE;
        goto done;
    }   
#else
    call_func_ptr(data);
#endif
    result = 0;
done:
    
    return result;
}

static
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
static
int
call_func_return_results(
    unsigned int cmd,
    co15_result_cpu * result_out)
{
    int result = -1;
    unsigned int index_out = 0;
    co15_result_cpu_wrapper local_result;
    
    if (result_out == NULL)
    {
        goto done;
    }
    memset(&local_result, 0x0, sizeof(co15_result_cpu_wrapper));
    TRACE("Command received: %x\n", cmd);
    
    /* locate function in global table */   
    if (find_function_index(cmd, &index_out) != 0)
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

register_undef_hook_t reg_undef_hook_p = NULL;
unregister_undef_hook_t unreg_undef_hook_p = NULL;
void ms_kernel_return(struct pt_regs * regs);

/******************************************************
 * Handle cases of incomplete implementations to avoid
 * kernel OOPS. This may not work on all systems.
 * @todo -- add flag to optionally disable
 *   
 * @param [in] void 
 * 
 * @return 0 on success, -1 on error
 * 
 * ***************************************************/
int
ms_find_undef_hook(void)
{
    int result = -1;
    
    reg_undef_hook_p = (register_undef_hook_t)kallsyms_lookup_name("register_undef_hook");
    if (reg_undef_hook_p == NULL)
    {
        TRACE("Failed to find symbol\n", 0);
        goto done;
    }
    TRACE("Found reg_undef_hook @ %p\n", reg_undef_hook_p);

    unreg_undef_hook_p = (unregister_undef_hook_t)kallsyms_lookup_name("unregister_undef_hook");
    if (unreg_undef_hook_p == NULL)
    {
        TRACE("Failed to find symbol\n", 0);
    }
    TRACE("Found unreg_undef_hook @ %p\n", unreg_undef_hook_p);
    
    result = 0;
done:
    return result;
}

static
int
undef_hook(
    struct pt_regs * regs,
    unsigned int instr)
{       
    g_undef_flag = 1;   
    
#ifdef __aarch64__
    ms_kernel_return(regs);
#else
    regs->ARM_pc += 4;
#endif
    return 0;
}

static struct undef_hook hook_struct = 
{
    .instr_mask = 0x0,
    .instr_val = 0x0,
#ifdef __aarch64__
    .pstate_mask = 0x0,
    .pstate_val = 0x0,
#else
    .cpsr_mask = UND_MODE,
    .cpsr_val = SVC_MODE,   
#endif
    .fn = undef_hook
};

int
ms_add_undef_instr_hook(void)
{
    int result = -1;
        
    if (reg_undef_hook_p == NULL)
    {
        goto done;
    }
    reg_undef_hook_p(&hook_struct);
    
    TRACE("Undef_instr hook added\n", 0);

    result = 0;
done: 
    return result;   
}

void
ms_del_undef_instr_hook(void)
{
    if (unreg_undef_hook_p != NULL)
    {
        unreg_undef_hook_p(&hook_struct);
    }
    else
    {       
        list_del(&hook_struct.node);
    }
    TRACE("Unregistered undef_instr hook\n", 0);
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
call_cmd_co(void * buffer)
{
    int result = -1;
    co15_result_cpu * user_buffer = NULL;
    co15_result_cpu user_buffer_local;
    
    if (buffer == NULL)
    {
        TRACE("param error\n", 0);
        goto done;
    }
    user_buffer = (co15_result_cpu *)buffer;    
    
    spin_lock_init(&g_undef_flag_lock);
    spin_lock(&g_undef_flag_lock);
    g_undef_flag = 0;    
    spin_unlock(&g_undef_flag_lock);
    
    memset(&user_buffer_local, 0x0, sizeof(co15_result_cpu));
        
    /* Get local kernel copy */ 
    if (copy_from_user(&user_buffer_local, user_buffer, sizeof(co15_result_cpu)) != 0)
    {
        TRACE("Could not get data from userspace: %d\n", result);
        goto done;
    }
        
    /* Call function & get results */   
    if (call_func_return_results(user_buffer_local.code, &user_buffer_local) != 0)
    {
        TRACE("Could not call function & get results\n", 0);
        goto done;
    }
    
    /* Copy results back to user space */   
    if (copy_to_user(user_buffer, &user_buffer_local, sizeof(co15_result_cpu)) != 0)
    {
        TRACE("Could not send data back to userspace: %d\n", result);
        goto done;
    }
    
    result = 0;
    
done:
    return result;
}
