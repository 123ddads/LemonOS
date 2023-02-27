
#ifndef KERNEL_H
#define KERNEL_H

#include "type.h"
#include "const.h"

typedef struct {
    ushort limit1;
    ushort base1;
    byte   base2;
    byte   attr1;
    byte   attr2_limit2;
    byte   base3;
} Descriptor;

typedef struct {
    Descriptor * const entry;
    const int          size;
} GdtInfo;

typedef struct {
    ushort offset1;
    ushort selector;
    byte   dcount;
    byte   attr;
    ushort offset2;
} Gate;

typedef struct {
    Gate * const entry;
    const int    size;
} IdtInfo;



extern GdtInfo gGdtInfo;
extern IdtInfo gIdtInfo;

int SetDescValue(Descriptor* pDesc, uint base, uint limit, ushort attr);
int GetDescValue(Descriptor* pDesc, uint* pBase, uint* pLimit, ushort* pAttr);
void ConfigPageTable();

#endif
