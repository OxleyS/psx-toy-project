#include "Checkerboard.h"

Texture g_CheckerboardTex;

void InitializeCheckboardTexture()
{
    int colors[] = { 0x1F001F, 0x3E003E0, 0x7C007C00 };
    int nColors = sizeof(colors) / sizeof(int);
    int colorIdx;

    int texelWidth = 64;
    int texelHeight = 64;
    u_long* pRgb = new u_long[(texelWidth * texelHeight) / 2];

    int i, j, startIdx = 0, horIdx = 0, verIdx = 0;
    for (i = 0; i < texelHeight; i++)
    {
        colorIdx = startIdx;
        for (j = 0; j < texelWidth / 2; j++)
        {
            pRgb[horIdx] = colors[colorIdx]; 
            horIdx++;
            if (horIdx % 4 == 0) colorIdx = (colorIdx + 1) % nColors;
        }
        verIdx++;
        if (verIdx % 4 == 0) startIdx = (startIdx + 1) % nColors; // Black to white, white to black
    }

    g_CheckerboardTex.InitFromMemoryColor(Texture::SEMITRANS_DONTCARE,
            pRgb, texelWidth, texelHeight, 384, 0);
}
