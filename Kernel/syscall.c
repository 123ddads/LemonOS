
#include "syscall.h"

#define SysCall(type, cmd, param1, param2)    asm volatile(                                  \
                                                             "movl  $" #type ",  %%eax \n"   \
                                                             "movl  $" #cmd  ",  %%ebx \n"   \
                                                             "movl  %0,          %%ecx \n"   \
                                                             "movl  %1,          %%edx \n"   \
                                                             "int   $0x80              \n"   \
                                                             :                               \
                                                             : "r"(param1), "r"(param2)      \
                                                             : "eax", "ebx", "ecx", "edx"    \
                                                          )

void Exit()
{
    SysCall(0, 0, 0, 0);
}

void Wait(const char* name)
{
    if( name )
    {
        SysCall(0, 1, name, 0);
    }
}

uint CreateMutex(uint type)
{
    volatile uint ret = 0;
    
    SysCall(1, 0, &ret, type); 
    
    return ret;
}

void EnterCritical(uint mutex)
{
    volatile uint wait = 0;
    
    do
    {
        SysCall(1, 1, mutex, &wait);
    }
    while( wait );
}

void ExitCritical(uint mutex)
{
    SysCall(1, 2, mutex, 0);
}

uint DestroyMutex(uint mutex)
{
    uint ret = 0;
    
    SysCall(1, 3, mutex, &ret);
    
    return ret;
}
