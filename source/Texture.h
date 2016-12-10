#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Global.h"

class Texture
{
    public:
        enum Type { TEXTYPE_CLUT4, TEXTYPE_CLUT8, TEXTYPE_COLOR };
        enum SemitransparentCode
        {
            SEMITRANS_05B_P_05F, SEMITRANS_10B_P_10F,
            SEMITRANS_10B_M_10F, SEMITRANS_10B_P_025F,
            SEMITRANS_DONTCARE = SEMITRANS_05B_P_05F 
        };

        void InitFromLoadedTim(u_long* pLoadedTim, int nBytes);
        void InitFromMemoryColor(SemitransparentCode semiTransCode,
            u_long* pTexels, int texelWidth, int texelHeight, int vramX, int vramY);
        void InitFromMemoryClut(SemitransparentCode semiTransCode,
            u_long* pTexels, int texelWidth, int texelHeight, int vramX, int vramY,
            Type clutType, u_long* pClut, int clutX, int clutY);

        Type m_Type;
        SemitransparentCode m_SemiTransCode;
        u_short m_TexelWidth;
        u_short m_TexelHeight;
        u_short m_PixelWidth;
        u_short m_PixelHeight;
        u_short m_ClutId;
        u_short m_TpageId;
};

#endif