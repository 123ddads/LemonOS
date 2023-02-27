
#include "memory.h"
#include "utility.h"
#include "list.h"

#define FM_ALLOC_SIZE    32
#define FM_NODE_SIZE     sizeof(FMemNode)
#define VM_HEAD_SIZE     sizeof(VMemHead)

typedef byte(FMemUnit)[FM_ALLOC_SIZE];
typedef union _FMemNode  FMemNode;

union _FMemNode
{
    FMemNode* next;
    FMemUnit* ptr;
};

typedef struct
{
    FMemNode* node;
    FMemNode* nbase;
    FMemUnit* ubase;
    uint max;
} FMemList;


typedef struct
{
    ListNode head;
    uint used;
    uint free;
    byte* ptr;
} VMemHead;


static FMemList gFMemList = {0};
static List gVMemList = {0};

static void FMemInit(byte* mem, uint size)
{
    FMemNode* p = NULL;
    int i = 0;
    uint max = 0;
    
    max = size / (FM_NODE_SIZE + FM_ALLOC_SIZE);
    
    gFMemList.max = max;
    gFMemList.nbase = (FMemNode*)mem;
    gFMemList.ubase = (FMemUnit*)((uint)mem + max * FM_NODE_SIZE);
    gFMemList.node = (FMemNode*)mem;
    
    p = gFMemList.node;
    
    for(i=0; i<max-1; i++)
    {
        FMemNode* current = (FMemNode*)AddrOff(p, i);
        FMemNode* next = (FMemNode*)AddrOff(p, i+1);
        
        current->next = next;
    }
    
    ((FMemNode*)AddrOff(p, i))->next = NULL;
}

static void* FMemAlloc()
{
    void* ret = NULL;
    
    if( gFMemList.node )
    {
        FMemNode* alloc = gFMemList.node;
        int index = AddrIndex(alloc, gFMemList.nbase);
        
        ret = AddrOff(gFMemList.ubase, index);
        
        gFMemList.node = alloc->next;
        
        alloc->ptr = ret;
    }
    
    return ret;
}

static int FMemFree(void* ptr)
{
    int ret = 0;
    
    if( ptr )
    {
        uint index = AddrIndex((FMemUnit*)ptr, gFMemList.ubase);
        FMemNode* node = AddrOff(gFMemList.nbase, index);
        
        if( (index < gFMemList.max) && IsEqual(node->ptr, ptr) )
        {
            node->next = gFMemList.node;
            
            gFMemList.node = node;
            
            ret = 1;
        }
    }
    
    return ret;
}

static void VMemInit(byte* mem, uint size)
{
    List_Init((List*)&gVMemList);
    VMemHead* head = (VMemHead*)mem;
    
    head->used = 0;
    head->free = size - VM_HEAD_SIZE;
    head->ptr = AddrOff(head, 1);
    
    List_AddTail(&gVMemList, (ListNode*)head);
} 

static void* VMemAlloc(uint size)
{
    ListNode* pos = NULL;
    VMemHead* ret = NULL;
    uint alloc = size + VM_HEAD_SIZE;
    
    List_ForEach(&gVMemList, pos)
    {
        VMemHead* current = (VMemHead*)pos;
        
        if( current->free >= alloc )
        {
            byte* mem = (byte*)((uint)current->ptr + (current->used + current->free) - alloc);
            
            ret = (VMemHead*)mem;
            ret->used = size;
            ret->free = 0;
            ret->ptr = AddrOff(ret, 1);
            
            current->free -= alloc;
            
            List_AddAfter((ListNode*)current, (ListNode*)ret);
            
            break;
        }
    }
    
    return ret ? ret->ptr : NULL;
}

static int VMemFree(void* ptr)
{
    int ret = 0;
    
    if( ptr )
    {
        ListNode* pos = NULL;
        
        List_ForEach(&gVMemList, pos)
        {
            VMemHead* current = (VMemHead*)pos;
            
            if( IsEqual(current->ptr, ptr) )
            {
                VMemHead* prev = (VMemHead*)(current->head.prev);
                
                prev->free += current->used + current->free + VM_HEAD_SIZE;
                
                List_DelNode((ListNode*)current);
                
                ret = 1;
                
                break;
            }
        }
    }
    
    return ret;
}

void MemModInit(byte* mem, uint size)
{
    byte* fmem = mem;
    uint fsize = size / 2;
    byte* vmem = AddrOff(fmem, fsize);
    uint vsize = size - fsize;
    
    FMemInit(fmem, fsize);
    VMemInit(vmem, vsize);
}

void* Malloc(uint size)
{
    void* ret = NULL;
    
    if( size <= FM_ALLOC_SIZE )
    {
        ret = FMemAlloc();
    }
    
    if( !ret ) 
    {
        ret = VMemAlloc(size);
    }
    
    return ret;
}

void Free(void* ptr)
{
    if( ptr )
    {
        if( !FMemFree(ptr) )
        {
            VMemFree(ptr);
        }
    }
}


