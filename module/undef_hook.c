#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/spinlock.h>
#include <linux/list.h>

#include <asm/traps.h>

#include "parse.h"
#include "ms_debug.h"
#include "ms_error.h"
#include "undef_hook.h"

unsigned int g_undef_flag;
spinlock_t g_undef_flag_lock;

register_undef_hook_t reg_undef_hook_p = NULL;
unregister_undef_hook_t unreg_undef_hook_p = NULL;


void
set_undef_flag(ms_data * out)
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

