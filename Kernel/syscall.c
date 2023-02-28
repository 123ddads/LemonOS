
#include "syscall.h"

void Exit()
{
    asm volatile(
        "movl  $0,  %eax \n"    // type
        "int   $0x80     \n"
    );
}

uint CreateMutex()
{
    volatile uint ret = 0;
    
    asm volatile(
        "movl  $1,  %%eax \n"   // type
        "movl  $0,  %%ebx \n"   // cmd
        "movl  %0,  %%ecx \n"   // param1
        "int   $0x80      \n"
        :
        : "r"(&ret)
        : "eax", "ebx", "ecx", "edx"
    );
    
    PrintString("&ret = ");
    PrintIntHex(&ret);
    
    return ret;
}

void EnterCritical(uint mutex)
{
    asm volatile(
        "movl  $1,  %%eax \n"   // type
        "movl  $1,  %%ebx \n"   // cmd
        "movl  %0,  %%ecx \n"   // param1
        "int   $0x80      \n"
        :
        : "r"(mutex)
        : "eax", "ebx", "ecx", "edx"
    );
}

void ExitCritical(uint mutex)
{
    asm volatile(
        "movl  $1,  %%eax \n"   // type
        "movl  $2,  %%ebx \n"   // cmd
        "movl  %0,  %%ecx \n"   // param1
        "int   $0x80      \n"
        :
        : "r"(mutex)
        : "eax", "ebx", "ecx", "edx"
    );
}

void DestroyMutex(uint mutex)
{
    asm volatile(
        "movl  $1,  %%eax \n"   // type
        "movl  $3,  %%ebx \n"   // cmd
        "movl  %0,  %%ecx \n"   // param1
        "int   $0x80      \n"
        :
        : "r"(mutex)
        : "eax", "ebx", "ecx", "edx"
    );
}
