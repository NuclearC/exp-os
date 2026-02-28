
#ifndef NC_STRING_STRING_H_
#define NC_STRING_STRING_H_

#include "ke_main.h"
#include "typedefs.h"

#define KSTR_INIT_CAP 16
#define KSTR_CAP_GROW 2

typedef struct {
    union {
        struct {
            size_t length;
            size_t cap;
            char *buf;
        } big;
        struct {
            size_t length;
            char buf[KSTR_INIT_CAP];
        } sm;
        size_t length;
    };
} KString;

int KAPI KeCreateString(KString *string, const char *init);
int KAPI KeCreateStringLength(KString *string, size_t len);

void KAPI KeCopyString(KString *dest, const KString *source);
void KAPI KeMoveString(KString *dest, KString *source);

void KAPI KeEnsureCapacity(KString *string, size_t capacity);

void KAPI KeDestroyString(const KString *string);

#endif
