
#ifndef NC_KE_MAIN_H_
#define NC_KE_MAIN_H_

#define KEXP
#define KAPI
#define KPRIV
#define KPACK __attribute__((packed))
#define KALIGN(x) __attribute__((aligned (x)))

#define KERNEL_PBASE 0x100000
#define KERNEL_VBASE 0xc0000000
#define CODE_SEG 0x08
#define DATA_SEG 0x10

#endif


