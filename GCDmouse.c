#include "GCD.h"

void GCD_delay_2(void *mouse_info)
{
#define mi ((MOUSE_INFO *)(mouse_info))
    Sleep(mi->timeout);
    /*超时后恢复光标属性并释放鼠标*/
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &mi->cci);
    ClipCursor(NULL);
    exit(-1);
}

DLL_EXPORT void GetMouseClickEvent(int clickMode, int timeout, bool clip, int *varX, int *varY)
{
    HANDLE StdInput  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE StdOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hDelay    = NULL;
    
    /*是否限制鼠标位置*/
    if (clip) {
        char ConsoleTitle[MAX_TITLE_LENGTH];
        WINDOWPLACEMENT wndpl;
        GetConsoleTitle(ConsoleTitle, MAX_TITLE_LENGTH);
        HWND hConsole = FindWindow(NULL, ConsoleTitle);
        GetWindowPlacement(hConsole, &wndpl);
        ClipCursor(&wndpl.rcNormalPosition);
    }

    DWORD OldMode, Res;
    INPUT_RECORD ir;
    CONSOLE_CURSOR_INFO oldCCI, newCCI;
    
    /*调整光标大小为100,不改动Visible*/
    GetConsoleCursorInfo(StdOutPut, &oldCCI);
    newCCI.bVisible = oldCCI.bVisible;
    newCCI.dwSize   = 100;

    MOUSE_INFO mouse_info = {timeout, {oldCCI.dwSize, oldCCI.bVisible}};
    if (timeout > 0) {
        //printf("%d", Delay);
        hDelay = (HANDLE)_beginthreadex(NULL, 0, (unsigned (__stdcall *) (void *))GCD_delay_2, (void *)&mouse_info, 0, NULL);
    }
    
    GetConsoleMode(StdInput, &OldMode);
    SetConsoleMode(StdInput, OldMode | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);
    OldMode = OldMode | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT;
    SetConsoleCursorInfo(StdOutPut, &newCCI);
    
    for (; ; ) {
        ReadConsoleInput(StdInput, &ir, 1, &Res);
        if (MOUSE_MOVED == ir.Event.MouseEvent.dwEventFlags) {
            SetConsoleCursorPosition(StdOutPut, ir.Event.MouseEvent.dwMousePosition);
        } else if (MOUSE_CLICK == ir.Event.MouseEvent.dwEventFlags && clickMode == ir.Event.MouseEvent.dwButtonState) {
            if (hDelay != NULL) {
                CloseHandle(hDelay);
            }
            /*恢复控制台原属性*/
            SetConsoleMode(StdInput, OldMode);
            SetConsoleCursorInfo(StdOutPut, &oldCCI);

            /*释放鼠标*/
            if (clip) {
                ClipCursor(NULL);
            }

            /*设置返回值*/
            *varX = ir.Event.MouseEvent.dwMousePosition.X;
            *varY = ir.Event.MouseEvent.dwMousePosition.Y;
            break;
        }
    }
    
    return 0;
}

/*FIXME:SetMouseInfo(0, 0)不会隐藏光标? */
DLL_EXPORT void SetCursorInfo(int size, int visible)
{
    CONSOLE_CURSOR_INFO cci = {size, (bool)visible};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

DLL_EXPORT void SetCursorPosition(int x, int y)
{
    COORD co = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), co);
}

DLL_EXPORT void EnableAllEvent()
{
    DWORD OldMode;
    HANDLE StdInput  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE StdOutPut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(StdInput, &OldMode);
    SetConsoleMode(StdInput, OldMode | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT	| ENABLE_MOUSE_INPUT);
}

#define IRMOUSE ir.Event.MouseEvent
#define IRKEY ir.Event.KeyEvent
#define IRBUF ir.Event.WindowBufferSizeEvent
#define IRMENU ir.Event.MenuEvent
#define IRFOC ir.Event.FocusEvent

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
            break;
    }
    return;
}