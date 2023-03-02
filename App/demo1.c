#include "demo1.h"
#include "memory.h"
#include "syscall.h"
#include "list.h"

typedef struct
{
    ListNode head;
    char product;
} Product;

enum { NUM = 60 };

static List g_store = {0};
static uint g_mutex = 0;
static int g_num = 0;
static int g_get = NUM;

static void Store(char c)
{
    Product* p = Malloc(sizeof(Product));
            
    p->product = c;
            
    List_AddTail(&g_store, (ListNode*)p);
}

static int Fetch(char type, char* c)
{
    int ret = 0;
    ListNode* pos = NULL;
    
    List_ForEach(&g_store, pos)
    {
        Product* p = (Product*)pos;
        
        if( type == 'A' )
        {
            ret = ('A' <= p->product) && (p->product <= 'Z');
        }
        else if( type == 'B' )
        {
            ret = ('a' <= p->product) && (p->product <= 'z');
        }
        
        if( ret )
        {
            *c = p->product; 
             
            List_DelNode(pos);
                    
            Free(pos);
            
            break;
        }
    }
    
    return ret;
}

void ProducerA()
{
    int next = 0;
    int run = 1;
    
    g_mutex = CreateMutex(Strict);
    
    List_Init(&g_store);
    
    SetPrintPos(0, 12);
    
    PrintString(__FUNCTION__);
    
    while(1)
    {
        EnterCritical(g_mutex);
        
        if( run = (g_num < NUM) )
        {
            char p = 'A' + next % 26;
            
            Store(p);
            
            g_num++;
            
            SetPrintPos(12 + next, 12);
            PrintChar(p);
            
            next++;
        }
        
        ExitCritical(g_mutex);
        
        if( run )
            Delay(1);
        else
            break;
    }
}

void ProducerB()
{
    int next = 0;
    int run = 1;
    
    SetPrintPos(0, 14);
    
    PrintString(__FUNCTION__);
    
    while(1)
    {
        EnterCritical(g_mutex);
        
        if( run = (g_num < NUM) )
        {
            char p = 'a' + next % 26;
            
            Store(p);
            
            g_num++;
            
            SetPrintPos(12 + next, 14);
            PrintChar(p);
            
            next++;
        }
        
        ExitCritical(g_mutex);
        
        if( run )
            Delay(next % 2 + 1);
        else
            break;
    }
}

void ConsumerA()
{
    int next = 0;
    int run = 1;
    
    SetPrintPos(0, 16);
    
    PrintString(__FUNCTION__);
    
    while(1)
    {
        char p = 0;
        
        EnterCritical(g_mutex);
        
        if( (run = (g_get > 0)) && Fetch('A', &p) )
        {
            SetPrintPos(12 + next++, 16);
            PrintChar(p);
            
            g_get--;
        }
        
        ExitCritical(g_mutex);
        
        if( run )
            Delay(next % 2 + 1);
        else
            break;
    }
}

void ConsumerB()
{
    int next = 0;
    int run = 1;
    
    SetPrintPos(0, 18);
    
    PrintString(__FUNCTION__);
    
    while(1)
    {
        char p = 0;
        
        EnterCritical(g_mutex);
        
        if( (run = (g_get > 0)) && Fetch('B', &p) )
        {
            SetPrintPos(12 + next++, 18);
            PrintChar(p);
            
            g_get--;
        }
        
        ExitCritical(g_mutex);
        
        if( run )
            Delay(2);
        else
            break;
    }
}
