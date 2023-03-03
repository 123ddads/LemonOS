
#ifndef TASK_H
#define TASK_H

#include "kernel.h"
#include "queue.h"

typedef struct {
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
} Task;

typedef struct
{
    QueueNode head;
    Task task;
} TaskNode;

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
void MtxSchedule(uint action);
void KillTask();
void WaitTask(const char* name);

#endif
