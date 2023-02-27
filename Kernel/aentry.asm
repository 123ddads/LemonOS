%include "common.asm"

global _start
global AppModInit

extern AppMain
extern GetAppToRun
extern GetAppNum
extern MemModInit

[section .text]
[bits 32]
_start:
AppModInit: ; 0xF000
    push ebp
    mov ebp, esp
    
    mov dword [GetAppToRunEntry], GetAppToRun
    mov dword [GetAppNumEntry], GetAppNum
    
    push HeapSize
    push AppHeapBase
    
    call MemModInit
    
    add esp, 8

    call AppMain
    
    leave
    
    ret
    
