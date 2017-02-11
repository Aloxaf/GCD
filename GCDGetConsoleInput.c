#include "GCD.h"

#define IRMOUSE ir.Event.MouseEvent
#define IRKEY ir.Event.KeyEvent
#define IRBUF ir.Event.WindowBufferSizeEvent
#define IRMENU ir.Event.MenuEvent
#define IRFOC ir.Event.FocusEvent

DLL_EXPORT void EnableAllEvent()
{
    DWORD OldMode;
    HANDLE StdInput  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE StdOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(StdInput, &OldMode);
    SetConsoleMode(StdInput, OldMode | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT	| ENABLE_MOUSE_INPUT);
}

DLL_EXPORT void GetConsoleInput(char *s)
{
    DWORD OldMode, res;
    HANDLE StdInput  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE StdOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(StdInput, &OldMode);
    SetConsoleMode(StdInput, OldMode | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT	| ENABLE_MOUSE_INPUT);
    INPUT_RECORD ir;
    
    ReadConsoleInput(StdInput, &ir, 1, &res);
    
    switch (ir.EventType) {
        case KEY_EVENT:
            sprintf(s, "%d-%d-%d-%d-%d-%c-%d", ir.EventType, IRKEY.bKeyDown, IRKEY.wRepeatCount, IRKEY.wVirtualKeyCode, IRKEY.wVirtualScanCode, IRKEY.uChar.AsciiChar, IRKEY.dwControlKeyState);
            break;    
        case MOUSE_EVENT:
            sprintf(s, "%d-%d-%d-%d-%d-%d", ir.EventType, IRMOUSE.dwMousePosition.X, IRMOUSE.dwMousePosition.Y, IRMOUSE.dwButtonState, IRMOUSE.dwControlKeyState, IRMOUSE.dwEventFlags);
            break;
        case WINDOW_BUFFER_SIZE_EVENT:
            sprintf(s, "%d-%d-%d", ir.EventType, IRBUF.dwSize.X, IRBUF.dwSize.Y);
            break;
        case MENU_EVENT:
            sprintf(s, "%d-%u", ir.EventType, IRMENU.dwCommandId);
            break;
        case FOCUS_EVENT:
            sprintf(s, "%d-%d", ir.EventType, IRFOC.bSetFocus);
    }
    return;
}