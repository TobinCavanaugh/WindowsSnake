//
// Created by tobin on 5/26/2023.
//

#ifndef WINDOWSTETRIS_WINDOWRENDERER_H
#define WINDOWSTETRIS_WINDOWRENDERER_H


#define WS_WIDTH 30
#define WS_HEIGHT 30
#define WS_PADDING 4

#define WS_EMPTY 0
#define WS_SNAKE 1
#define WS_APPLE 2

#include <stdbool.h>
#include "PCG_Core.h"

void WT_InitWindowRenderer(HINSTANCE hInst, int rrowCount, int ccolCount);
void WT_SetPixel(PCG_Point point, int newState);
void WT_RelocateWindow(PCG_Point windowIndex, PCG_Point newPosition);

#endif //WINDOWSTETRIS_WINDOWRENDERER_H
