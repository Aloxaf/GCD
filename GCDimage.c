#include <gdiplus\gdiplus.h>
#include <shellapi.h>
#include "GCD.h"


#define DLL_EXPORT __declspec(dllexport)
#undef LoadImage

typedef struct {
    HBITMAP hBitmap;
    UINT w;
    UINT h;
} IMAGE;

wchar_t* CharToWchar(char *str)
{
    int len       = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
    wchar_t *wstr = (wchar_t *)malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, str, len, wstr, 0);
    return wstr;
}

/*TODO:可不可以直接从hBitmap中取得位图点阵*/
DLL_EXPORT void LoadImage(char *varName, char *wfile, int width, int height)
{
    GpImage     *thisImage;
    GpGraphics  *graphics = NULL;
    ULONG_PTR   gdiplusToken;
    IMAGE       _img;
    wchar_t        *file = CharToWchar(wfile);

    GdiplusStartupInput GSI = {1, NULL, false, false};
    GdiplusStartup(&gdiplusToken, &GSI, NULL);
    GdipLoadImageFromFile(file, &thisImage);

    HDC hdc = GetDC(NULL);

    GdipGetImageWidth (thisImage, &_img.w);
    GdipGetImageHeight(thisImage, &_img.h);

    HDC hdcMem = CreateCompatibleDC(hdc);

    if ((height|width) != 0) {
        _img.w = width;
        _img.h = height;
    }

    _img.hBitmap = CreateCompatibleBitmap(hdc, _img.w, _img.h);

    SelectObject(hdcMem, _img.hBitmap);

    GdipCreateFromHDC (hdcMem, &graphics);
    GdipDrawImageRectI(graphics, thisImage, 0, 0, _img.w, _img.h);

    GdipDisposeImage  (thisImage);
    GdipDeleteGraphics(graphics);

    DeleteDC (hdcMem);
    ReleaseDC(NULL, hdc);

    GdiplusShutdown(gdiplusToken);
    
    sprintf(varName, "%p:%d:%d", _img.hBitmap, _img.w, _img.h);

}

DLL_EXPORT void DrawImage(char *DC, char *ImgInfo, int x, int y, int x1, int y1, int width, int height)
{
    IMAGE img;
    HDC hdcDst;
    HDC hdcSrc;
    sscanf(ImgInfo, "%p:%d:%d", &img.hBitmap, &img.w, &img.h);
    sscanf(DC, "%p:%p", &hdcDst, &hdcSrc);
    SelectObject(hdcSrc, img.hBitmap);
    
    if ((width|height) == 0) {
        BitBlt(hdcDst, x, y, img.w - x1, img.h - y1, hdcSrc, x1, y1, SRCCOPY);
    } else {
        BitBlt(hdcDst, x, y, width, height, hdcSrc, x1, y1, SRCCOPY);
    }
}

DLL_EXPORT void DrawAlphaImage(char *DC, char *ImgInfo, int x, int y, int x1, int y1, int width, int height)
{
    IMAGE img;
    HDC hdcDst;
    HDC hdcSrc;
    sscanf(ImgInfo, "%p:%d:%d", &img.hBitmap, &img.w, &img.h);
    sscanf(DC, "%p:%p", &hdcDst, &hdcSrc);
    SelectObject(hdcSrc, img.hBitmap);
    BLENDFUNCTION ftn = {AC_SRC_OVER, 0, -1, AC_SRC_ALPHA};

    if ((width|height) == 0) {
        AlphaBlend(hdcDst, x, y, img.w - x1, img.h - y1, hdcSrc, x1, y1, img.w - x1, img.h - y1, ftn);
    } else {
        AlphaBlend(hdcDst, x, y, width, height, hdcSrc, x1, y1, width, height, ftn);
    }
}

DLL_EXPORT void DrawTranImage(char *DC, char *ImgInfo, int x, int y, int x1, int y1, int width, int height, COLORREF clrToTran)
{
    IMAGE img;
    HDC hdcDst;
    HDC hdcSrc;
    sscanf(ImgInfo, "%p:%d:%d", &img.hBitmap, &img.w, &img.h);
    sscanf(DC, "%p:%p", &hdcDst, &hdcSrc);
    SelectObject(hdcSrc, img.hBitmap);
    
    if ((width|height) == 0) {
        TransparentBlt(hdcDst, x, y, img.w - x1, img.h - y1, hdcSrc, x1, y1, img.w - x1, img.h - y1, clrToTran);
    } else {
        TransparentBlt(hdcDst, x, y, width, height, hdcSrc, x1, y1, width, height, clrToTran);
    }
}

DLL_EXPORT void NewMemDC(char *varName, int width, int height)
{
    //printf("%s:%s", varName, wh);
    RECT rc = {width, height};
    HWND hCMD  = GetConsoleWindow();
    HDC hdcCMD = GetDC(hCMD);
    HDC hdcMem = CreateCompatibleDC(hdcCMD);
    
    HDC hdcTmp = CreateCompatibleDC(hdcMem);

    if ((rc.right | rc.bottom) == 0) {
        GetClientRect(hCMD, &rc);
    } 
    HBITMAP hdcBMP = CreateCompatibleBitmap(hdcCMD, rc.right, rc.bottom);
    SelectObject(hdcMem, hdcBMP);

    sprintf(varName, "%p:%p:%p:%p:%p:%ld:%ld", hdcMem, hdcTmp, hCMD, hdcCMD, hdcBMP, rc.right, rc.bottom);
    
}

DLL_EXPORT void CleanMemDC(char *DC)
{
    HDC hdcMem;
    RECT rc;
    //sscanf(DC, "%p:%*p:%*p:%*p:%ld:%ld", &hdcMem, &rc.right, &rc.bottom);
    sscanf(DC, "%p:%*p:%*p:%*p:%*p:%ld:%ld", &hdcMem, &rc.right, &rc.bottom);
    Rectangle(hdcMem, 0, 0, rc.right, rc.bottom);
    //free(DC);
}

DLL_EXPORT void ApplyDraw(char *varDC)
{
    HDC  hdcCMD, hdcMem;
    RECT rc;
    //sscanf(varDC, "%p:%*p:%p:%*p:%ld:%ld", &hdcMem, &hdcCMD, &rc.right, &rc.bottom);
    sscanf(varDC, "%p:%*p:%*p:%p:%*p:%ld:%ld", &hdcMem, &hdcCMD, &rc.right, &rc.bottom);
    BitBlt(hdcCMD, 0, 0, rc.right, rc.bottom, hdcMem, 0, 0, SRCCOPY);
    //free(varDC);
}


DLL_EXPORT void FreeMemDC(char *varDC)
{
    HWND hCMD;
    HDC  hdcCMD, hdcMem, hdcTmp;
    HBITMAP hdcBMP;
    //sscanf(varDC, "%p:%p:%p:%p", &hdcMem, &hCMD, &hdcCMD, &hdcBMP);
    sscanf(varDC, "%p:%p:%p:%p:%p", &hdcMem, &hdcTmp, &hCMD, &hdcCMD, &hdcBMP);
    DeleteObject(hdcBMP);
    DeleteDC(hdcMem);
    DeleteDC(hdcTmp);
    ReleaseDC(hCMD, hdcCMD);
    //free(varDC);
}

DLL_EXPORT void FreeImage(char *ImgInfo)
{
    HBITMAP hBitmap;
    sscanf(ImgInfo, "%p", &hBitmap);
    DeleteObject(hBitmap);
    //free(ImgInfo);
}

DLL_EXPORT void CleanCMD(int x, int y, int width, int height)
{
    RECT rc   = {x, y, width, height};
    InvalidateRect(GetConsoleWindow(), (x | y | width | height) == 0 ? NULL : &rc, true);
}

