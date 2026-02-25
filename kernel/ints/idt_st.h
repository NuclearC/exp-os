
#ifndef NC_UTIL_IDT_ST_H_
#define NC_UTIL_IDT_ST_H_

extern int* _idt_setup(void);
extern int* _idt_addr(void);
extern void _idt_enable(void);

#endif 

