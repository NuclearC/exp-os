
#ifndef NC_KE_PIC_ST_H_
#define NC_KE_PIC_ST_H_

#define PIC_MASTER 0x20
#define PIC_SLAVE 0x28

extern void _pic_setup(void);

extern int _pic_getmask(void);
extern void _pic_setmask(int msk);

#endif

