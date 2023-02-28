
#include "mutex.h"

void MutexModInit()
{
}

void MutexCallHandler(uint cmd, uint param)
{
    if( cmd == 0 )
    {
        PrintString("Mutex Create\n");
        PrintIntHex(param);
        PrintChar('\n');
    }
    else if( cmd == 1 )
    {
        PrintString("Mutex Enter\n");
    }
    else if( cmd == 2 )
    {
        PrintString("Mutex Exit\n");
    }
    else 
    {
        PrintString("Mutex Destroy\n");
    }
}
