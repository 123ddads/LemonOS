
#ifndef UTILITY_H
#define UTILITY_H

#include "type.h"

#define AddrOff(a, i)    ((void*)((uint)(a) + (i) * sizeof(*(a))))
#define AddrIndex(b, a)  (((uint)(b) - (uint)(a))/sizeof(*(b)))

#define IsEqual(a, b)           \
({                              \
    unsigned ta = (unsigned)(a);\
    unsigned tb = (unsigned)(b);\
    !(ta - tb);                 \
})

#define OffsetOf(type, member)  ((unsigned)&(((type*)0)->member))

#define ContainerOf(ptr, type, member)                  \
({                                                      \
      const typeof(((type*)0)->member)* __mptr = (ptr); \
      (type*)((char*)__mptr - OffsetOf(type, member));  \
})

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))

void Delay(int n);
char* StrCpy(char* dst, const char* src, int n);
int StrLen(const char* s);
int StrCmp(const char* left, const char* right, uint n);
#endif
