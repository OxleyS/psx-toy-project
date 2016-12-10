#include "Checkerboard.h"

Texture g_CheckerboardTex;

void InitializeCheckboardTexture(int texX, int texY, int clutX, int clutY)
{
    //int colors[] = { 0x001F001F, 0x03E003E0, 0x7C007C00 };
    int colors[] = { 0x00000000, 0x11111111, 0x22222222 };
    int nColors = sizeof(colors) / sizeof(int);
    int colorIdx;

    int texelsPerLong = 8;
    int texelWidth = 64;
    int texelHeight = 64;
    u_long* pRgb = new u_long[(texelWidth * texelHeight) / 8];

    int i, j, startIdx = 0, horIdx = 0, verIdx = 0;
    for (i = 0; i < texelHeight; i++)
    {
        colorIdx = startIdx;
        for (j = 0; j < texelWidth / texelsPerLong; j++)
        {
            pRgb[horIdx] = colors[colorIdx]; 
            horIdx++;
            if (horIdx % (8 / texelsPerLong) == 0) colorIdx = (colorIdx + 1) % nColors;
        }
        verIdx++;
        if (verIdx % 4 == 0) startIdx = (startIdx + 1) % nColors; // Black to white, white to black
    }

    //g_CheckerboardTex.InitFromMemoryColor(Texture::SEMITRANS_DONTCARE,
            //pRgb, texelWidth, texelHeight, texX, texY);

    u_long clut[] = { 0x03E0001F, 0xFFFF7C00, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };  
    g_CheckerboardTex.InitFromMemoryClut(Texture::SEMITRANS_DONTCARE,
            pRgb, texelWidth, texelHeight, texX, texY,
            Texture::TEXTYPE_CLUT4, clut, clutX, clutY);
}
