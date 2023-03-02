
#include "app.h"
#include "utility.h"
#include "memory.h"
#include "syscall.h"

#include "demo1.h"
#include "demo2.h"

#define MAX_APP_NUM    16

static AppInfo gAppToRun[MAX_APP_NUM] = {0};
static uint gAppNum = 0;

void TaskA();
void TaskB();
void TaskC();
void TaskD();

static void RegApp(const char* name, void(*tmain)(), byte pri)
{
    if( gAppNum < MAX_APP_NUM )
    {
        AppInfo* app = AddrOff(gAppToRun, gAppNum);
        
        app->name = name;
        app->tmain = tmain;
        app->priority = pri;
        
        gAppNum++;
    }
}

void AppMain()
{
    // RegApp("Task A", TaskA, 255);
    // RegApp("Task B", TaskB, 255);
    // RegApp("Task C", TaskC, 255);
    // RegApp("Task D", TaskD, 255);
    
    // RegApp("PA", ProducerA, 255);
    // RegApp("PB", ProducerB, 255);
    // RegApp("CA", ConsumerA, 255);
    // RegApp("CB", ConsumerB, 255);
    
    RegApp("Writer",  Writer, 255);
    RegApp("ReaderA", Reader, 255);
    RegApp("ReaderB", Reader, 255);
    RegApp("ReaderC", Reader, 255);
}

AppInfo* GetAppToRun(uint index)
{
    AppInfo* ret = NULL;
    
    if( index < MAX_APP_NUM )
    {
        ret = AddrOff(gAppToRun, index);
    }
    
    return ret;
}

uint GetAppNum()
{
    return gAppNum;
}

static uint g_mutex = 0;
static int i = 0;

void TaskA()
{
    SetPrintPos(0, 12);
    
    PrintString(__FUNCTION__);
    PrintChar('\n');
    
    g_mutex = CreateMutex(Normal);
    
    EnterCritical(g_mutex);
    EnterCritical(g_mutex);
    EnterCritical(g_mutex);
    
    for(i=0; i<50; i++)
    {
        SetPrintPos(8, 12);
        PrintChar('A' + i % 26);
        Delay(1);
    }
    
    ExitCritical(g_mutex);
}

void TaskB()
{
    SetPrintPos(0, 16);
    
    PrintString(__FUNCTION__);
    
    // ExitCritical(g_mutex);
    
    EnterCritical(g_mutex);
    
    i = 0;
    
    while(0)
    {
        SetPrintPos(8, 16);
        PrintChar('0' + i);
        i = (i + 1) % 10;
        Delay(1);
    }
    
    SetPrintPos(8, 16);
    
    i = DestroyMutex(g_mutex);
    
    PrintIntDec(i);
    PrintChar('\n');
    
    ExitCritical(g_mutex);
    
    i = DestroyMutex(g_mutex);
    
    PrintIntDec(i);
    PrintChar('\n');
    
}

void TaskC()
{
    int i = 0;
    
    SetPrintPos(0, 14);
    
    PrintString(__FUNCTION__);
    
    while(1)
    {
        SetPrintPos(8, 14);
        PrintChar('a' + i);
        i = (i + 1) % 26;
        Delay(1);
    }
}

void TaskD()
{
    int i = 0;
    
    SetPrintPos(0, 15);
    
    PrintString(__FUNCTION__);
    
    while(1)
    {
        SetPrintPos(8, 15);
        PrintChar('!' + i);
        i = (i + 1) % 10;
        Delay(1);
    }
}
