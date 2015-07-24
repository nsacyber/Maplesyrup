#include <linux/init.h>
#include <linux/linkage.h>

#include <asm/assembler.h>
#include <asm/asm-offsets.h>

#ifdef __aarch64__

.section .maplesyrup,"ax"
.globl ms_kernel_return
.type ms_kernel_return, @function

ms_kernel_return:
    mov     sp, x0
    ldp     x21, x22, [sp, #S_PC]
    add     x21, x21, #0x4
    ldp     x0, x1, [sp], #0x10
    ldp     x2, x3, [sp], #0x10
    ldp     x4, x5, [sp], #0x10
    ldp     x6, x7, [sp], #0x10
    ldp     x8, x9, [sp], #0x10 
    msr     elr_el1, x21
    msr     spsr_el1, x22
    ldp     x10, x11, [sp], #0x10
    ldp     x12, x13, [sp], #0x10
    ldp     x14, x15, [sp], #0x10
    ldp     x16, x17, [sp], #0x10
    ldp     x18, x19, [sp], #0x10
    ldp     x20, x21, [sp], #0x10
    ldp     x22, x23, [sp], #0x10
    ldp     x24, x25, [sp], #0x10
    ldp     x26, x27, [sp], #0x10
    ldp     x28, x29, [sp], #0x10
    ldr     lr, [sp], #S_FRAME_SIZE - S_LR
    eret

#endif
