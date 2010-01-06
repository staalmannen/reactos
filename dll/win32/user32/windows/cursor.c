/*
 *  ReactOS kernel
 *  Copyright (C) 1998, 1999, 2000, 2001 ReactOS Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/* $Id$
 *
 * PROJECT:         ReactOS user32.dll
 * FILE:            lib/user32/windows/cursor.c
 * PURPOSE:         Cursor
 * PROGRAMMER:      Casper S. Hornstrup (chorns@users.sourceforge.net)
 * UPDATE HISTORY:
 *      09-05-2001  CSH  Created
 */

/* INCLUDES ******************************************************************/

#include <user32.h>

#include <wine/debug.h>

WINE_DEFAULT_DEBUG_CHANNEL(user32);

#undef CopyCursor

HBITMAP
CopyBitmap(HBITMAP bmp);

/* INTERNAL ******************************************************************/

/* This callback routine is called directly after switching to gui mode */
NTSTATUS
WINAPI
User32SetupDefaultCursors(PVOID Arguments,
                          ULONG ArgumentLength)
{
    BOOL *DefaultCursor = (BOOL*)Arguments;
    LRESULT Result = TRUE;

    if(*DefaultCursor)
    {
        /* set default cursor */
        SetCursor(LoadCursorW(0, (LPCWSTR)IDC_ARROW));
    }
    else
    {
        /* FIXME load system cursor scheme */
        SetCursor(0);
        SetCursor(LoadCursorW(0, (LPCWSTR)IDC_ARROW));
    }

    return(ZwCallbackReturn(&Result, sizeof(LRESULT), STATUS_SUCCESS));
}

/* FUNCTIONS *****************************************************************/


/*
 * @implemented
 */
HCURSOR
WINAPI
CopyCursor(HCURSOR pcur)
{
    ICONINFO IconInfo;

    if(GetIconInfo((HANDLE)pcur, &IconInfo))
    {
        return (HCURSOR)NtUserCreateCursorIconHandle(&IconInfo, FALSE);
    }
    return (HCURSOR)0;
}

/*
 * @implemented
 */
HCURSOR
WINAPI
CreateCursor(HINSTANCE hInst,
             int xHotSpot,
             int yHotSpot,
             int nWidth,
             int nHeight,
             CONST VOID *pvANDPlane,
             CONST VOID *pvXORPlane)
{
    ICONINFO IconInfo;
    BYTE BitmapInfoBuffer[sizeof(BITMAPINFOHEADER) + 2 * sizeof(RGBQUAD)];
    BITMAPINFO *bwBIH = (BITMAPINFO *)BitmapInfoBuffer;
    HDC hScreenDc;

    hScreenDc = CreateCompatibleDC(NULL);
    if (hScreenDc == NULL)
        return NULL;

    bwBIH->bmiHeader.biBitCount = 1;
    bwBIH->bmiHeader.biWidth = nWidth;
    bwBIH->bmiHeader.biHeight = -nHeight * 2;
    bwBIH->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bwBIH->bmiHeader.biPlanes = 1;
    bwBIH->bmiHeader.biSizeImage = 0;
    bwBIH->bmiHeader.biCompression = BI_RGB;
    bwBIH->bmiHeader.biClrImportant = 0;
    bwBIH->bmiHeader.biClrUsed = 0;
    bwBIH->bmiHeader.biXPelsPerMeter = 0;
    bwBIH->bmiHeader.biYPelsPerMeter = 0;

    bwBIH->bmiColors[0].rgbBlue = 0;
    bwBIH->bmiColors[0].rgbGreen = 0;
    bwBIH->bmiColors[0].rgbRed = 0;
    bwBIH->bmiColors[0].rgbReserved = 0;

    bwBIH->bmiColors[1].rgbBlue = 0xff;
    bwBIH->bmiColors[1].rgbGreen = 0xff;
    bwBIH->bmiColors[1].rgbRed = 0xff;
    bwBIH->bmiColors[1].rgbReserved = 0;

    IconInfo.hbmMask = CreateDIBitmap(hScreenDc, &bwBIH->bmiHeader, 0,
                                      NULL, bwBIH, DIB_RGB_COLORS);
    if (IconInfo.hbmMask)
    {
        SetDIBits(hScreenDc, IconInfo.hbmMask, 0, nHeight,
                  pvXORPlane, bwBIH, DIB_RGB_COLORS);
        SetDIBits(hScreenDc, IconInfo.hbmMask, nHeight, nHeight,
                  pvANDPlane, bwBIH, DIB_RGB_COLORS);
    }
    else
    {
        return NULL;
    }

    DeleteDC(hScreenDc);

    IconInfo.fIcon = FALSE;
    IconInfo.xHotspot = xHotSpot;
    IconInfo.yHotspot = yHotSpot;
    IconInfo.hbmColor = 0;

    return (HCURSOR)NtUserCreateCursorIconHandle(&IconInfo, FALSE);
}


/*
 * @implemented
 */
BOOL
WINAPI
DestroyCursor(HCURSOR hCursor)
{
    return (BOOL)NtUserDestroyCursor((HANDLE)hCursor, 0);
}


/*
 * @implemented
 */
HCURSOR
WINAPI
GetCursor(VOID)
{
    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    if(NtUserGetCursorInfo(&ci))
        return ci.hCursor;
    else
        return (HCURSOR)0;
}


/*
 * @implemented
 */
BOOL
WINAPI
GetCursorPos(LPPOINT lpPoint)
{
    BOOL res;
    /* Windows doesn't check if lpPoint == NULL, we do */
    if(!lpPoint)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    res = NtUserGetCursorPos(lpPoint);

    return res;
}


/*
 * @implemented
 */
HCURSOR
WINAPI
LoadCursorA(HINSTANCE hInstance,
            LPCSTR lpCursorName)
{
    return(LoadImageA(hInstance,
                      lpCursorName,
                      IMAGE_CURSOR,
                      0,
                      0,
                      LR_SHARED | LR_DEFAULTSIZE));
}


/*
 * @implemented
 */
HCURSOR
WINAPI
LoadCursorFromFileA(LPCSTR lpFileName)
{
    UNICODE_STRING FileName;
    HCURSOR Result;
    RtlCreateUnicodeStringFromAsciiz(&FileName, (LPSTR)lpFileName);
    Result = LoadImageW(0,
                        FileName.Buffer,
                        IMAGE_CURSOR,
                        0,
                        0,
                        LR_LOADFROMFILE | LR_DEFAULTSIZE);
    RtlFreeUnicodeString(&FileName);
    return(Result);
}


/*
 * @implemented
 */
HCURSOR
WINAPI
LoadCursorFromFileW(LPCWSTR lpFileName)
{
    return(LoadImageW(0,
           lpFileName,
           IMAGE_CURSOR,
           0,
           0,
           LR_LOADFROMFILE | LR_DEFAULTSIZE));
}


/*
 * @implemented
 */
HCURSOR
WINAPI
LoadCursorW(HINSTANCE hInstance,
            LPCWSTR lpCursorName)
{
    return(LoadImageW(hInstance,
                      lpCursorName,
                      IMAGE_CURSOR,
                      0,
                      0,
                      LR_SHARED | LR_DEFAULTSIZE));
}


/*
 * @implemented
 */
BOOL
WINAPI
SetCursorPos(int X,
             int Y)
{
    return NtUserSetCursorPos(X,Y);
}


/*
 * @unimplemented
 */
BOOL
WINAPI
SetSystemCursor(HCURSOR hcur,
                DWORD id)
{
    UNIMPLEMENTED;
    return FALSE;
}


/*
 * @implemented
 */
int
WINAPI
ShowCursor(BOOL bShow)
{
    return NtUserShowCursor(bShow);
}

HCURSOR
CursorIconToCursor(HICON hIcon,
                   BOOL SemiTransparent)
{
    UNIMPLEMENTED;
    return 0;
}
