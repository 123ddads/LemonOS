
#include "interrupt.h"
#include "keyboard.h"
#include "task.h"
#include "mutex.h"
#include "screen.h"
#include "sysinfo.h"

extern byte ReadPort(ushort port);

void TimerHandler()
{
    static uint i = 0;
    
    i = (i + 1) % 5;
    
    if( i == 0 )
    {
        Schedule();
    }
    
    SendEOI(MASTER_EOI_PORT);
}

void KeyboardHandler()
{
    byte sc = ReadPort(0x60);
    
    PutScanCode(sc);
    
    NotifyKeyCode();
    
    SendEOI(MASTER_EOI_PORT);
}

void SysCallHandler(uint type, uint cmd, uint param1, uint param2)   // __cdecl__
{  
    switch(type)
    {
        case 0:
            TaskCallHandler(cmd, param1, param2);
            break;
        case 1:
            MutexCallHandler(cmd, param1, param2);
            break;
        case 2:
            KeyCallHandler(cmd, param1, param2);
            break;
        case 3:
            SysInfoCallHandler(cmd, param1, param2);
            break;
        default:
            break;
    }
}

void PageFaultHandler()
{
    SetPrintPos(ERR_START_W, ERR_START_H);
    
    PrintString("Page Fault: kill ");
    PrintString(CurrentTaskName());
    
    KillTask();
}

void SegmentFaultHandler()
{
    SetPrintPos(ERR_START_W, ERR_START_H);
    
    PrintString("Segment Fault: kill ");
    PrintString(CurrentTaskName());
    
    KillTask();
}
