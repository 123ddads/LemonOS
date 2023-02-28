
#ifndef MUTEX_H
#define MUTEX_H

#include "type.h"

typedef void Mutex;

void MutexModInit();
void MutexCallHandler(uint cmd, uint param);

Mutex* SysCreateMutex();
void SysDestroyMutex(Mutex* mutex);
void SysEnterCritical(Mutex* mutex);
void SysExitCritical(Mutex* mutex);

#endif
