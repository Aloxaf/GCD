#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>

//#define MAX_VK_COUNT 128
#define MAX_TITLE_LENGTH 256
#define MOUSE_CLICK 0
/*#define CALL_WITHCHAR 2
#define CALL_WITHVK 4
#define MAX_IMG_COMMAND_LENGTH 1024*/

#define DLL_EXPORT __declspec(dllexport)
/*
typedef struct {
    int length;
    int *item;
} VK_LIST;

typedef struct {
    int Delay;
    int Default;
} CHOICE_INFO;*/

typedef struct {
    int timeout;
    CONSOLE_CURSOR_INFO cci;
} MOUSE_INFO;
/*
typedef struct {
    char *text;
    char *front;
    int x;
    int y;
    int width;
    int height;
    int weight;
    COLORREF clrBk;
    COLORREF clrFr;
    bool bktran;
    bool italic;
    bool underline;
    bool strikeout;
    bool DrawText;
    RECT rc;
    int DTParams;
} FUNC_TEXT_INFO;
*/
