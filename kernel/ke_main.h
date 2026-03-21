
#ifndef NC_KE_MAIN_H_
#define NC_KE_MAIN_H_

#define KEXP  /* public, exported kernel function */
#define KAPI  /* kernel API function */
#define KPRIV /* private kernel function */
#define KPACK __attribute__((packed))
#define KALIGN(x) __attribute__((aligned(x)))
#define KNORET __attribute__((noreturn))

/* the physical base address of the kernel */
#define KERNEL_PBASE 0x100000
/* the virtual base address of the kernel */
#define KERNEL_VBASE 0xffffffffc0000000
/* size of the stack allocated for the kernel */
#define KERNEL_STACK_SIZE 0x20000
/* top of the stack for the kernel */
#define KERNEL_STACK_TOP KERNEL_PBASE + 0x10000 + KERNEL_STACK_SIZE
/* kernel image end, or where the useable memory starts */
#define KERNEL_PEND KERNEL_STACK_TOP + 0x10000

#define KSUCCESS 0

#endif
