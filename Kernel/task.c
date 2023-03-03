#include "utility.h"
#include "task.h"
#include "queue.h"
#include "app.h"

#define MAX_TASK_NUM        4
#define MAX_RUNNING_TASK    2
#define MAX_READY_TASK      (MAX_TASK_NUM - MAX_RUNNING_TASK)
#define MAX_TASK_BUFF_NUM   (MAX_TASK_NUM + 1)
#define PID_BASE            0x10
#define MAX_TIME_SLICE      260

static AppInfo* (*GetAppToRun)(uint index) = NULL;
static uint (*GetAppNum)() = NULL;

void (* const RunTask)(volatile Task* pt) = NULL;
void (* const LoadTask)(volatile Task* pt) = NULL;

volatile Task* gCTaskAddr = NULL;
static TaskNode gTaskBuff[MAX_TASK_BUFF_NUM] = {0};
static Queue gFreeTaskNode = {0};
static Queue gReadyTask = {0};
static Queue gRunningTask = {0};
static Queue gWaittingTask = {0};
static TSS gTSS = {0};
static TaskNode* gIdleTask = NULL;
static uint gAppToRunIndex = 0;
static uint gPid = PID_BASE;

static void TaskEntry()
{
    if( gCTaskAddr )
    {
        gCTaskAddr->tmain();
    }
    
    // to destory current task here
    asm volatile(
        "movl  $0,  %eax \n"    // type
        "int   $0x80     \n"
    );
}

static void IdleTask()
{
    while( 1 );
}

static void InitTask(Task* pt, uint id, const char* name, void(*entry)(), ushort pri)
{
    pt->rv.cs = LDT_CODE32_SELECTOR;
    pt->rv.gs = LDT_VIDEO_SELECTOR;
    pt->rv.ds = LDT_DATA32_SELECTOR;
    pt->rv.es = LDT_DATA32_SELECTOR;
    pt->rv.fs = LDT_DATA32_SELECTOR;
    pt->rv.ss = LDT_DATA32_SELECTOR;
    
    pt->rv.esp = (uint)pt->stack + AppStackSize;
    pt->rv.eip = (uint)TaskEntry;
    pt->rv.eflags = 0x3202;
    
    pt->tmain = entry;
    pt->id = id;
    pt->current = 0;
    pt->total = MAX_TIME_SLICE - pri;
    
    StrCpy(pt->name, name, sizeof(pt->name)-1);
    
    Queue_Init(&pt->wait);
    
    SetDescValue(AddrOff(pt->ldt, LDT_VIDEO_INDEX),  0xB8000, 0x07FFF, DA_DRWA + DA_32 + DA_DPL3);
    SetDescValue(AddrOff(pt->ldt, LDT_CODE32_INDEX), 0x00,    KernelHeapBase - 1, DA_C + DA_32 + DA_DPL3);
    SetDescValue(AddrOff(pt->ldt, LDT_DATA32_INDEX), 0x00,    KernelHeapBase - 1, DA_DRW + DA_32 + DA_DPL3);
    
    pt->ldtSelector = GDT_TASK_LDT_SELECTOR;
    pt->tssSelector = GDT_TASK_TSS_SELECTOR;
}

static Task* FindTaskByName(const char* name)
{
    Task* ret = NULL;
    
    if( !StrCmp(name, "IdleTask", -1) )
    {
        int i = 0;
        
        for(i=0; i<MAX_TASK_BUFF_NUM; i++)
        {
            TaskNode* tn = AddrOff(gTaskBuff, i);
            
            if( tn->task.id && StrCmp(tn->task.name, name, -1) )
            {
                ret = &tn->task;
                break;
            }
        }
    }
    
    return ret;
}

static void PrepareForRun(volatile Task* pt)
{
    pt->current++;
    
    gTSS.ss0 = GDT_DATA32_FLAT_SELECTOR;
    gTSS.esp0 = (uint)&pt->rv + sizeof(pt->rv);
    gTSS.iomb = sizeof(TSS);
    
    SetDescValue(AddrOff(gGdtInfo.entry, GDT_TASK_LDT_INDEX), (uint)&pt->ldt, sizeof(pt->ldt)-1, DA_LDT + DA_DPL0);
}

static void CreateTask()
{
    uint num = GetAppNum();
    
    while( (gAppToRunIndex < num) && (Queue_Length(&gReadyTask) < MAX_READY_TASK) )
    {
        TaskNode* tn = (TaskNode*)Queue_Remove(&gFreeTaskNode);
        
        if( tn )
        {
            AppInfo* app = GetAppToRun(gAppToRunIndex);
            
            InitTask(&tn->task, gPid++, app->name, app->tmain, app->priority);
            
            Queue_Add(&gReadyTask, (QueueNode*)tn);
        }
        else
        {
            break;
        }
        
        gAppToRunIndex++;
    }
}

static void CheckRunningTask()
{
    if( Queue_Length(&gRunningTask) == 0 )
    {
        Queue_Add(&gRunningTask, (QueueNode*)gIdleTask);
    }
    else if( Queue_Length(&gRunningTask) > 1 )
    {
        if( IsEqual(Queue_Front(&gRunningTask), (QueueNode*)gIdleTask) )
        {
            Queue_Remove(&gRunningTask);
        }
    }
}

static void ReadyToRunning()
{
    QueueNode* node = NULL;
    
    if( Queue_Length(&gReadyTask) < MAX_READY_TASK )
    {
        CreateTask();
    }
    
    while( (Queue_Length(&gReadyTask) > 0) && (Queue_Length(&gRunningTask) < MAX_RUNNING_TASK) )
    {
        node = Queue_Remove(&gReadyTask);
        
        ((TaskNode*)node)->task.current = 0;
        
        Queue_Add(&gRunningTask, node);
    }
}

static void RunningToReady()
{
    if( Queue_Length(&gRunningTask) > 0 )
    {
        TaskNode* tn = (TaskNode*)Queue_Front(&gRunningTask);
        
        if( !IsEqual(tn, (QueueNode*)gIdleTask) )
        {
            if( tn->task.current == tn->task.total )
            {
                Queue_Remove(&gRunningTask);
                Queue_Add(&gReadyTask, (QueueNode*)tn);
            }
        }
    }
}

static void RunningToWaitting(Queue* wq)
{
    if( Queue_Length(&gRunningTask) > 0 )
    {
        TaskNode* tn = (TaskNode*)Queue_Front(&gRunningTask);
        
        if( !IsEqual(tn, (QueueNode*)gIdleTask) )
        {
            Queue_Remove(&gRunningTask);
            Queue_Add(wq, (QueueNode*)tn);
        }
    }
}

static void WaittingToReady(Queue* wq)
{
    while( Queue_Length(wq) > 0 )
    {
        TaskNode* tn = (TaskNode*)Queue_Front(wq);
        
        Queue_Remove(wq);
        Queue_Add(&gReadyTask, (QueueNode*)tn);
    }
}

void TaskModInit()
{
    int i = 0;
    byte* pStack = (byte*)(AppHeapBase - (AppStackSize * MAX_TASK_BUFF_NUM));
    
    for(i=0; i<MAX_TASK_BUFF_NUM; i++)
    {
        TaskNode* tn = (void*)AddrOff(gTaskBuff, i);
        
        tn->task.stack = (void*)AddrOff(pStack, i * AppStackSize);
    }
    
    gIdleTask = (void*)AddrOff(gTaskBuff, MAX_TASK_NUM);
    
    GetAppToRun = (void*)(*((uint*)GetAppToRunEntry));
    GetAppNum = (void*)(*((uint*)GetAppNumEntry));
    
    Queue_Init(&gFreeTaskNode);
    Queue_Init(&gRunningTask);
    Queue_Init(&gReadyTask);
    Queue_Init(&gWaittingTask);
    
    for(i=0; i<MAX_TASK_NUM; i++)
    {
        Queue_Add(&gFreeTaskNode, (QueueNode*)AddrOff(gTaskBuff, i));
    }
    
    SetDescValue(AddrOff(gGdtInfo.entry, GDT_TASK_TSS_INDEX), (uint)&gTSS, sizeof(gTSS)-1, DA_386TSS + DA_DPL0);
    
    InitTask(&gIdleTask->task, 0, "IdleTask", IdleTask, 255);
    
    ReadyToRunning();
    
    CheckRunningTask();
}

static void ScheduleNext()
{
    ReadyToRunning();
    
    CheckRunningTask();
    
    Queue_Rotate(&gRunningTask);
    
    gCTaskAddr = &((TaskNode*)Queue_Front(&gRunningTask))->task;
    
    PrepareForRun(gCTaskAddr);
    
    LoadTask(gCTaskAddr);
}

void LaunchTask()
{
    gCTaskAddr = &((TaskNode*)Queue_Front(&gRunningTask))->task;
    
    PrepareForRun(gCTaskAddr);
    
    RunTask(gCTaskAddr);
}

void MtxSchedule(uint action)
{
    if( IsEqual(action, NOTIFY) )
    {
        WaittingToReady(&gWaittingTask);
    }
    else if( IsEqual(action, WAIT) )
    {
        RunningToWaitting(&gWaittingTask);
        ScheduleNext();
    }
}

void Schedule()
{
    RunningToReady();
    ScheduleNext();
}

void KillTask()
{
    QueueNode* node = Queue_Remove(&gRunningTask);
    Task* task = &((TaskNode*)node)->task;
    
    WaittingToReady(&task->wait);
    
    task->id = 0;
    
    Queue_Add(&gFreeTaskNode, node);
    
    Schedule();
}

void WaitTask(const char* name)
{
    Task* task = FindTaskByName(name);
    
    if( task )
    {
        RunningToWaitting(&task->wait);
        ScheduleNext();
    }
}

void TaskCallHandler(uint cmd, uint param1, uint param2)
{
    switch(cmd)
    {
        case 0:
            KillTask();
            break;
        case 1:
            WaitTask((char*)param1);
            break;
        default:
            break;
    }
}
