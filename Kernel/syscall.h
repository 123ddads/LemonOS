#ifndef SYSCALL_H
#define SYSCALL_H

#include "type.h"

enum
{
    Normal,
    Strict
};

void Exit();
void Wait(const char* name);

uint CreateMutex(uint type);
void EnterCritical(uint mutex);
void ExitCritical(uint mutex);
uint DestroyMutex(uint mutex);

#endif
