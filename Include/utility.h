
#ifndef UTILITY_H
#define UTILITY_H

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

void Delay(int n);
char* StrCpy(char* dst, const char* src, int n);
#endif
