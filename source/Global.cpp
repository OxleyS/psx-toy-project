#include "Global.h"

namespace Memory
{
    void* Copy(void* pDest, const void* pSrc, int nBytes)
    {
        // TODO: Test

        u_long* pLongDest = (u_long*)pDest;
        u_long* pLongSrc = (u_long*)pSrc;
        int nLongCopies = ((unsigned)nBytes) >> 2;

        for (int i = 0; i < nLongCopies; i++) *pLongDest++ = *pLongSrc++;
        
        u_char* pByteDest = (u_char*)pLongDest;
        u_char* pByteSrc = (u_char*)pLongSrc;
        int nByteCopies = nBytes - (nLongCopies << 2);

        for (int i = 0; i < nByteCopies; i++) *pByteDest++ = *pByteSrc++; 

        return pDest;
    }

    void* Set(void* pDest, int value, int nBytes)
    {
        // TODO: Test

        u_long* pLongDest = (u_long*)pDest;
        int nLongSets = ((unsigned)nBytes) >> 2;
        u_char converted = (u_char)value; 
        value = (int)converted;
        int duplicatedVal = (value) | (value << 8) | (value << 16) | (value << 24);

        for (int i = 0; i < nLongSets; i++) *pLongDest++ = duplicatedVal;

        u_char* pByteDest = (u_char*)pLongDest;
        int nByteSets = nBytes - (nLongSets << 2);

        for (int i = 0; i < nByteSets; i++) *pByteDest++ = converted;

        return pDest;
    }
}