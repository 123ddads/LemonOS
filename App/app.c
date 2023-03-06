
#include "utility.h"
#include "memory.h"
#include "syscall.h"

#include "demo1.h"
#include "demo2.h"

void AppMain()
{
    // RunDemo1();
    // RunDemo2();
    uint m = CreateMutex(Normal);
    EnterCritical(m);
    Wait("AppMain");
    PrintString("AppMain()...");
    ExitCritical(m);
    DestroyMutex(m);
}
