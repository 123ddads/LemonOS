
#ifndef TASK_H
#define TASK_H

#include "kernel.h"
#include "queue.h"
#include "event.h"
#include "app.h"

typedef struct 
{
    uint gs;
    uint fs;
    uint es;
    uint ds;
    uint edi;
    uint esi;
    uint ebp;
    uint kesp;
    uint ebx;
    uint edx;
    uint ecx;
    uint eax;
    uint error_code;
    uint eip;
    uint cs;
    uint eflags;
    uint esp;
    uint ss;
} RegValue;

typedef struct
{
    uint   previous;
    uint   esp0;
    uint   ss0;
    uint   unused[22];
    ushort reserved;
    ushort iomb;
} TSS;

typedef struct
{
    RegValue   rv;
    Descriptor ldt[3];
    ushort     ldtSelector;
    ushort     tssSelector;
    void (*tmain)();
    uint       id;
    ushort     current;
    ushort     total;
    char       name[16]; 
    Queue      wait;
    byte*      stack;
    Event*     event;
} Task;

typedef struct
{
    QueueNode head;
    Task task;
} TaskNode;

typedef struct
{
    QueueNode head;
    AppInfo app;
} AppNode;

enum
{
    WAIT,
    NOTIFY
};

extern void (* const RunTask)(volatile Task* pt);
extern void (* const LoadTask)(volatile Task* pt);

void TaskModInit();
void LaunchTask();
void Schedule();
void TaskCallHandler(uint cmd, uint param1, uint param2);
void EventSchedule(uint action, Event* event);
void KillTask();
void WaitTask(const char* name);

const char* CurrentTaskName();
uint CurrentTaskId();

#endif
