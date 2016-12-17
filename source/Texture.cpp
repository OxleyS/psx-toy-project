#include "Texture.h"

void Texture::InitFromLoadedTim(u_long* pLoadedTim, int nBytes)
{
    assert(*pLoadedTim == 0x10);
    pLoadedTim++;

    u_long flags = *pLoadedTim++;
    m_Type = (Type)(flags & 0x7);
    assert(m_Type < TEXTYPE_COUNT);
    
    if (UsesClut())
    {
        assert((flags & 0x8));

        u_long* pClutStart = pLoadedTim;
        u_long nClutBytes = *pLoadedTim++;
        u_long frameBufferPos = *pLoadedTim++;
        m_ClutVramX = (u_short)frameBufferPos & 0xFFFF;
        m_ClutVramY = (u_short)(frameBufferPos >> 16) & 0xFFFF;
        u_long clutSize = *pLoadedTim++;
        if (m_Type == TEXTYPE_CLUT4)
        {
            assert(clutSize == 0x10010);
            m_ClutId = LoadClut2(pLoadedTim, m_ClutVramX, m_ClutVramY);
        }
        else
        {
            assert(clutSize == 0x10100);
            m_ClutId = LoadClut(pLoadedTim, m_ClutVramX, m_ClutVramY);
        }
        
        pLoadedTim = pClutStart + (nClutBytes / sizeof(u_long));
    }
    else assert(!(flags & 0x8));

    u_long nTexelBytes = *pLoadedTim++;
    (void)nTexelBytes;
    u_long frameBufferPos = *pLoadedTim++;
    m_TexelVramX = (u_short)(frameBufferPos & 0xFFFF);
    m_TexelVramY = (u_short)((frameBufferPos >> 16) & 0xFFFF);
    u_long pixelRectSize = *pLoadedTim++;
    m_PixelWidth = (u_short)(pixelRectSize & 0xFFFF);
    m_PixelHeight = (u_short)((pixelRectSize >> 16) & 0xFFFF);
    RecalcTexelSize();

    // TODO: So is it up to the primitive or the texture? Not clear
    m_SemiTransCode = SEMITRANS_DONTCARE;
    printf("%d %d\n", m_TexelWidth, m_TexelHeight);
    m_TpageId = LoadTPage(pLoadedTim, (int)m_Type, (int)m_SemiTransCode,
        m_TexelVramX, m_TexelVramY, m_TexelWidth, m_TexelHeight);
}

void Texture::InitFromMemoryColor(SemitransparentCode semiTransCode, u_long* pTexels,
    int texelWidth, int texelHeight, int vramX, int vramY)
{
    m_Type = TEXTYPE_COLOR16;
    m_SemiTransCode = semiTransCode;
    m_TexelWidth = texelWidth;
    m_TexelHeight = texelHeight;
    RecalcPixelSize();
    m_ClutId = 0xFFFF;
    m_TpageId = LoadTPage(pTexels, 2, (int)semiTransCode,
        vramX, vramY, texelWidth, texelHeight);
    m_TexelVramX = vramX;
    m_TexelVramY = vramY;
}

void Texture::InitFromMemoryClut(SemitransparentCode semiTransCode, u_long* pTexels,
    int texelWidth, int texelHeight, int vramX, int vramY,
    Type clutType, u_long* pClut, int clutX, int clutY)
{
    assert(clutType == TEXTYPE_CLUT4 || clutType == TEXTYPE_CLUT8);

    int clutCode;
    if (clutType == TEXTYPE_CLUT4)
    {
        clutCode = 0;
        m_ClutId = LoadClut2(pClut, clutX, clutY);
    }
    else
    {
        clutCode = 1;
        m_ClutId = LoadClut(pClut, clutX, clutY);
    }

    m_Type = clutType;
    m_SemiTransCode = semiTransCode;
    m_TexelWidth = texelWidth;
    m_TexelHeight = texelHeight;
    RecalcPixelSize();
    m_TpageId = LoadTPage(pTexels, clutCode, (int)semiTransCode,
        vramX, vramY, texelWidth, texelHeight);
    m_TexelVramX = vramX;
    m_TexelVramY = vramY;
    m_ClutVramX = clutX;
    m_ClutVramY = clutY;
}

void Texture::RecalcPixelSize()
{
    int scalars[] = { 4, 2, 1 };
    m_PixelWidth = m_TexelWidth / scalars[(int)m_Type];
    m_PixelHeight = m_TexelHeight;
}

void Texture::RecalcTexelSize()
{
    int scalars[] = { 4, 2, 1 };
    m_TexelWidth = m_PixelWidth * scalars[(int)m_Type];
    m_TexelHeight = m_PixelHeight;
}
