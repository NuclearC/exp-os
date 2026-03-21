

#include <stdint.h>

#define NORET __attribute__((noreturn))
#define PACK __attribute__((packed))

#define VWIDTH 80
#define VHEIGHT 25
#define VCOL 0x1f
#define VCOL2 0x1a
#define VERR 0x14
#define VBASE 0xb8000

extern void _v_cursor(int pos);
extern uint64_t _io_read_bytes(void *, uint64_t initsector, uint64_t num_bytes,
                               uint64_t skip_offset);
extern uint64_t _io_read_sector(void *, uint64_t initsector,
                                uint64_t numsectors);
extern int _s_strcmp(const char *, const char *, uint64_t maxlen);
extern void _v_print(const char *, uint64_t offset, char color);
extern NORET void _kernel_start(uintptr_t entry, uintptr_t stack_top,
                                void *kparams);
