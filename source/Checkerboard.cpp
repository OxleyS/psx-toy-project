#include "Checkerboard.h"

int g_CheckerboardWidth = 64;
int g_CheckerboardHeight = 64;
u_long g_CheckerboardRgb[2048];

void Checkerboard_Initialize()
{
    int colors[] = { 0x1F001F, 0x3E003E0, 0x7C007C00 };
    int nColors = sizeof(colors) / sizeof(int);
    int colorIdx;

    int i, j, startIdx = 0, horIdx = 0, verIdx = 0;
    for (i = 0; i < g_CheckerboardHeight; i++)
    {
        colorIdx = startIdx;
        for (j = 0; j < g_CheckerboardWidth / 2; j++)
        {
            g_CheckerboardRgb[horIdx] = colors[colorIdx]; 
            horIdx++;
            if (horIdx % 4 == 0) colorIdx = (colorIdx + 1) % nColors;
        }
        verIdx++;
        if (verIdx % 4 == 0) startIdx = (startIdx + 1) % nColors; // Black to white, white to black
    }
}
