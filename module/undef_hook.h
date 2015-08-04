#ifndef _UNDEF_HOOK_H
#define _UNDEF_HOOK_H

struct undef_hook; // forward declare

typedef void (*register_undef_hook_t)(struct undef_hook * hook);
typedef void (*unregister_undef_hook_t)(struct undef_hook * hook);

void ms_del_undef_instr_hook(void);
int ms_add_undef_instr_hook(void);
int ms_find_undef_hook(void);
void set_undef_flag(ms_data * out);
void ms_kernel_return(struct pt_regs * regs);

#endif
