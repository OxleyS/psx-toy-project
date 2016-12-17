#include "Global.h"

namespace Memory
{
    void* Move(void* pDest, const void* pSrc, int nBytes)
    {
        if ((u_char*)pDest > (u_char*)pSrc && (u_char*)pDest < (u_char*)pSrc + nBytes)
        {
            u_long* pLongDest = (u_long*)((u_char*)pDest + nBytes) - 1;
            u_long* pLongSrc = (u_long*)((u_char*)pSrc + nBytes) - 1;
            int nLongCopies = ((unsigned)nBytes) >> 2;

            for (int i = 0; i < nLongCopies; i++) *pLongDest-- = *pLongSrc--;
            
            u_char* pByteDest = (u_char*)pLongDest;
            u_char* pByteSrc = (u_char*)pLongSrc;
            int nByteCopies = nBytes - (nLongCopies << 2);

            for (int i = 0; i < nByteCopies; i++) *pByteDest-- = *pByteSrc--;
        }
        else
        {
            u_long* pLongDest = (u_long*)pDest;
            u_long* pLongSrc = (u_long*)pSrc;
            int nLongCopies = ((unsigned)nBytes) >> 2;

            for (int i = 0; i < nLongCopies; i++) *pLongDest++ = *pLongSrc++;
            
            u_char* pByteDest = (u_char*)pLongDest;
            u_char* pByteSrc = (u_char*)pLongSrc;
            int nByteCopies = nBytes - (nLongCopies << 2);

            for (int i = 0; i < nByteCopies; i++) *pByteDest++ = *pByteSrc++;
        } 

        return pDest;
    }

    void* Copy(void* pDest, const void* pSrc, int nBytes)
    {
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

namespace String
{
    int Compare(const char* pStr1, const char* pStr2)
    {
        char a, b;
        while (((a = *pStr1++) & (b = *pStr2++)) && (a == b)) {}
        return b - a;
    }

    int Length(const char* pStr)
    {
        const char* pCur = pStr;
        while (*pCur++) {}
        return (int)(pCur - pStr - 1);
    }

    int Copy(char* pDest, const char* pSrc)
    {
        const char* pDestStart = pDest;
        while ((*pDest++ = *pSrc++)) {}
        return (int)(pDest - pDestStart - 1);
    }

    int CopyCount(char* pDest, const char* pSrc, int count)
    {
        const char* pDestStart = pDest;
        while (count > 0) { *pDest++ = *pSrc++; count--; }
        return (int)(pDest - pDestStart - 1);
    }

    char* NewDup(const char* pStr)
    {
        char* pCopy = new char[Length(pStr) + 1];
        Copy(pCopy, pStr);
        return pCopy;
    }
}
