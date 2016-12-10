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

void Texture::InitFromMemoryClut(SemitransparentCode semiTransCode, u_long* pTexels,
    int texelWidth, int texelHeight, int vramX, int vramY,
    Type clutType, u_long* pClut, int clutX, int clutY)
{
    assert(clutType == TEXTYPE_CLUT4 || clutType == TEXTYPE_CLUT8);

    int tpRatio, clutCode;
    if (clutType == TEXTYPE_CLUT4)
    {
        tpRatio = 8;
        clutCode = 0;
        this->m_ClutId = LoadClut2(pClut, clutX, clutY);
    }
    else
    {
        tpRatio = 4;
        clutCode = 1;
        this->m_ClutId = LoadClut(pClut, clutX, clutY);
    }

    this->m_Type = clutType;
    this->m_SemiTransCode = semiTransCode;
    this->m_TexelWidth = texelWidth;
    this->m_TexelHeight = texelHeight;
    this->m_PixelWidth = texelWidth / tpRatio;
    this->m_PixelHeight = texelHeight / tpRatio;
    this->m_TpageId = LoadTPage(pTexels, clutCode, (int)semiTransCode,
        vramX, vramY, texelWidth, texelHeight);
}