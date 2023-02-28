#ifndef SYSCALL_H
#define SYSCALL_H

#include "type.h"

void Exit();

uint CreateMutex();
void EnterCritical(uint mutex);
void ExitCritical(uint mutex);
void DestroyMutex(uint mutex);

#endif
