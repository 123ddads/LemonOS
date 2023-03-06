#include "shell.h"
#include "syscall.h"
#include "screen.h"

static uint IsKeyDown(uint kc)
{
    return !!(kc & 0xFF000000);
}

static char GetChar(uint kc)
{
    return (char)kc;
}

static byte GetKeyCode(uint kc)
{
    return (byte)(kc >> 8);
}

void Shell()
{
    SetPrintPos(0, 9);
    PrintString("LemonOS >> ");
    
    while(1)
    {
        uint key = ReadKey();
        
        if( IsKeyDown(key) )
        {
            char ch = GetChar(key);
            uint vk = GetKeyCode(key);
            
            if( ch )
            {
                PrintChar(ch);
            }
        }
    }
}
