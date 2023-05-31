//
// Created by tobin on 5/26/2023.
//
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include "WindowRenderer.h"

#define WM_SET_PIXEL_COLOR (WM_USER + 1)

#define COLOR_ATTRIBUTE_INDEX GWLP_USERDATA

HINSTANCE hInstance;

HWND CreateCustomWindow(char *name, int xpos, int ypos, int xScale, int yScale, const char *className, int style) {

    // Adjust the window size to include the border and padding
    RECT windowRect = {0, 0, xScale, yScale};
    AdjustWindowRectEx(&windowRect, WS_POPUP | WS_SYSMENU, FALSE, 0);
    int adjustedWidth = windowRect.right - windowRect.left;
    int adjustedHeight = windowRect.bottom - windowRect.top;

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles
            className,                     // Window class name
            name,                           // Window text
            style,          // Window style
            xpos, ypos,                     // Position
            adjustedWidth, adjustedHeight,   // Size including border and padding
            NULL,                           // Parent window
            NULL,                           // Menu
            hInstance,                      // Instance handle
            NULL                            // Additional application data
    );

    return hwnd;
}

HWND background;
HWND **winGrid;
int colCount;
int rowCount;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

        //Set minimum window size
        case WM_GETMINMAXINFO: {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
            lpMMI->ptMinTrackSize.x = 1;
            lpMMI->ptMinTrackSize.y = 1;
        }


        case WM_NCPAINT: {
            // Draw the custom title bar with the desired color
            HDC hdc = GetWindowDC(hwnd);
            RECT rcTitleBar;
            GetWindowRect(hwnd, &rcTitleBar);
            rcTitleBar.bottom = rcTitleBar.top + GetSystemMetrics(SM_CYCAPTION);
            FillRect(hdc, &rcTitleBar, CreateSolidBrush(RGB(255, 0, 0)));
            ReleaseDC(hwnd, hdc);
            return 0;
        }

        //We do this to unhide the windows when they get clicked on
        case WM_LBUTTONDOWN: {
            SetForegroundWindow(background);
            for (int x = 0; x < colCount; x++) {
                for (int y = 0; y < rowCount; y++) {
                    SetForegroundWindow(winGrid[x][y]);
                }
            }
        }

        default:
            // Window procedure code goes here...
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


HWND InternalMakeBackground(int adjustedXOffset, int adjustedYOffset) {

    int xBorderWidth = (WS_WIDTH * colCount) + (colCount * WS_PADDING);
    int yBorderWidth = (WS_HEIGHT * rowCount) + (rowCount * WS_PADDING);
    int borderX = adjustedXOffset - WS_PADDING;
    int borderY = adjustedYOffset - WS_PADDING;

    // Register the window class
    WNDCLASS borderWindowClass = {0};
    borderWindowClass.lpfnWndProc = WindowProc;
    borderWindowClass.hInstance = hInstance;
    borderWindowClass.lpszClassName = "Background";
    borderWindowClass.hbrBackground = GetSysColorBrush(COLOR_DESKTOP);
    RegisterClass(&borderWindowClass);

    //Old opaque version
//    HWND border = CreateCustomWindow("Windows Tetris", borderX, borderY, xBorderWidth + (2 * WS_PADDING),
//                                     yBorderWidth + (2 * WS_PADDING), borderWindowClass.lpszClassName,
//                                     WS_POPUP);

    //Fully transparent version
    HWND border = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, borderWindowClass.lpszClassName, "Windows Tetris",
                                 WS_POPUPWINDOW, borderX, borderY, xBorderWidth + (2 * WS_PADDING),
                                 yBorderWidth + (2 * WS_PADDING), NULL, NULL, hInstance, NULL);


    SetLayeredWindowAttributes(border, RGB(255, 0, 255), 80, LWA_ALPHA);

    ShowWindow(border, SW_SHOW);
    UpdateWindow(border);
}


void WT_InitWindowRenderer(HINSTANCE hInst, int rrowCount, int ccolCount) {


    rowCount = rrowCount;
    colCount = ccolCount;

    int xOffset = GetSystemMetrics(SM_CXSCREEN) / 2;
    int yOffset = GetSystemMetrics(SM_CYSCREEN) / 2;

    //Center the game
    int adjustedXOffset = (int) round(xOffset - (WS_WIDTH * colCount) / 2.0f) - colCount * WS_PADDING;
    int adjustedYOffset = (int) round(yOffset - (WS_HEIGHT * rowCount) / 2.0f) - rowCount * WS_PADDING;

    // Store the instance handle in the global variable
    hInstance = hInst;

    //Make background window
    background = InternalMakeBackground(adjustedXOffset, adjustedYOffset);

    //Register the window class
    WNDCLASS blockWindowClass = {0};
    blockWindowClass.lpfnWndProc = WindowProc;
    blockWindowClass.hInstance = hInstance;
    blockWindowClass.lpszClassName = "Block";

    RegisterClass(&blockWindowClass);

    winGrid = malloc(colCount * sizeof(HWND *));

    for (int x = 0; x < colCount; x++) {
        winGrid[x] = malloc(rowCount * sizeof(HWND));

        for (int y = 0; y < rowCount; y++) {


            HWND hwnd = CreateCustomWindow(".", x * WS_WIDTH + WS_PADDING * x + adjustedXOffset,
                                           y * WS_HEIGHT + WS_PADDING * y + adjustedYOffset, WS_WIDTH,
                                           WS_HEIGHT, blockWindowClass.lpszClassName, WS_POPUP | WS_SYSMENU);

            winGrid[x][y] = hwnd;

            ShowWindow(hwnd, SW_SHOW);
            UpdateWindow(hwnd);
        }
    }
}

COLORREF occupiedColor = RGB(36, 58, 94); // White color (you can modify this as needed)


HBRUSH emptyBrush = NULL;
HBRUSH snakeBrush = NULL;
HBRUSH appleBrush;

void WT_SetPixel(PCG_Point point, int newState) {
    int x = (int) point.x;
    int y = (int) point.y;

    if (emptyBrush == NULL) {
        emptyBrush = GetSysColorBrush(COLOR_SCROLLBAR);
    }

    if (snakeBrush == NULL) {
        snakeBrush = CreateSolidBrush(RGB(0, 255, 0));
    }

    if (appleBrush == NULL) {
        appleBrush = CreateSolidBrush(RGB(255, 0, 0));
    }

    if (x >= 0 && y >= 0 && x < colCount && y < rowCount) {

        var current = winGrid[x][y];

        switch (newState) {
            case 0: {
                ShowWindow(current, SW_HIDE);
                break;
            }

            case 1: {

                var v = (HBRUSH) GetClassLongPtr(current, GCLP_HBRBACKGROUND);
                if (v != snakeBrush) {
                    SetClassLongPtr(current, GCLP_HBRBACKGROUND, (LONG_PTR) snakeBrush);
                }

                if (!IsWindowVisible(current)) {
                    ShowWindow(current, SW_SHOW);
                }
                break;
            }

            case 2: {

                SetClassLongPtr(current, GCLP_HBRBACKGROUND, (LONG_PTR) appleBrush);

                if (!IsWindowVisible(current)) {
                    ShowWindow(current, SW_SHOW);
                }

                break;
            }
        }

        // Redraw the window
        InvalidateRect(winGrid[x][y], NULL, TRUE);
        UpdateWindow(winGrid[x][y]);
    }
}

void WT_RelocateWindow(PCG_Point windowIndex, PCG_Point newPosition) {
    SetWindowPos(winGrid[(int) windowIndex.x][(int) windowIndex.y], NULL, (int) newPosition.x, (int) newPosition.y,
                 WS_WIDTH, WS_HEIGHT, SWP_SHOWWINDOW);
}


//     // Set the background color for the window class
//    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)CreateSolidBrush(backgroundColor));
