
#ifndef SCREEN_H
#define SCREEN_H

#include "type.h"

#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25

typedef enum
{
    SCREEN_GRAY   = 0x07,
    SCREEN_BLUE   = 0x09,
    SCREEN_GREEN  = 0x0A,
    SCREEN_RED    = 0x0C,
    SCREEN_YELLOW = 0x0E,
    SCREEN_WHITE  = 0x0F
} PrintColor;

void ClearScreen();
int  SetPrintPos(byte w, byte h);
void SetPrintColor(PrintColor c);
int PrintChar(char c);
int PrintString(const char* s);
int PrintIntDec(int n);
int PrintIntHex(uint n);


#endif
