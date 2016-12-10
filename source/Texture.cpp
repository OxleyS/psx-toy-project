#include "Texture.h"

void Texture::InitFromMemoryColor(SemitransparentCode semiTransCode, u_long* pTexels,
    int texelWidth, int texelHeight, int vramX, int vramY)
{
    this->m_Type = TEXTYPE_COLOR;
    this->m_SemiTransCode = semiTransCode;
    this->m_TexelWidth = texelWidth;
    this->m_TexelHeight = texelHeight;
    this->m_PixelWidth = texelWidth;
    this->m_PixelHeight = texelHeight;
    this->m_ClutId = 0xFFFF;
    this->m_TpageId = LoadTPage(pTexels, 2, (int)semiTransCode,
        vramX, vramY, texelWidth, texelHeight);
}