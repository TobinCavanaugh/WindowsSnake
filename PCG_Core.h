//
// Created by tobin on 5/4/2023.
//

#ifndef PIXELDRAWER_PCG_CORE_H
#define PIXELDRAWER_PCG_CORE_H
//TODO Rework EVERYTHING to use PCG_Point

#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define var __auto_type

#define Action(arg) void (*arg)(void)

#define PCG_PointZero (PCG_Point) {0, 0}
#define PCG_PointOne (PCG_Point) {1, 1}

#define PCG_PointUp (PCG_Point) {0, 1}
#define PCG_PointDown (PCG_Point) {0, -1}

#define PCG_PointLeft (PCG_Point) {-1, 0}
#define PCG_PointRight (PCG_Point) {1, 0}


typedef uint32_t u32;

typedef struct PCG_Point {
    float x;
    float y;
} PCG_Point;

#ifndef PCG_WINDOW
#define PCG_WINDOW
typedef struct PCG_Window {

    int BitmapWidth;
    int BitmapHeight;

    int ClientWidth;
    int ClientHeight;

    void *BitmapMemory;
    HWND *Window;
    BITMAPINFO *BitmapInfo;
    HDC *DeviceContext;

    WNDCLASS *WindowClass;

    int fullscreenState;

} PCG_Window;

#endif

float PCG_PointDistance(const PCG_Point *a, const PCG_Point *b);

PCG_Point *PCG_Closest(const PCG_Point *point, PCG_Point *a, PCG_Point *b);

#endif //PIXELDRAWER_PCG_CORE_H
