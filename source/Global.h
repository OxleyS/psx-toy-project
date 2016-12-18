#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>

#include <libgte.h>
#include <libgpu.h>

#define FORCE_INLINE __attribute__((always_inline))

#define OT_LENGTH (512)

#define SCREEN_WIDTH (320)
#define SCREEN_HEIGHT (240)

#define SCREEN_CENTER_X (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)

// Custom replacements of the crappy ones provided by the BIOS
namespace Memory
{
    void* Move(void* pDest, const void* pSrc, int nBytes);
    void* Copy(void* pDest, const void* pSrc, int nBytes);
    void* Set(void* pDest, int value, int nBytes);
}
namespace String
{
    int Compare(const char* pStr1, const char* pStr2); // strcmp
    int Length(const char* pStr); // strlen
    int Copy(char* pDest, const char* pSource); // strcpy
    int CopyCount(char* pDest, const char* pSource, int count); // strncpy 
    char* NewDup(const char* pStr); // strdup, but uses new instead of malloc
}

class Color : public CVECTOR
{
};

class OrderingTable;
class Vec3Short;
class Matrix;
namespace Debug
{
    void PrintHeaderToName(const char* pType, const char* pName);
    void PrintHeader(const char* pType, const char* pName);
    void PrintFooter();
    void PrintShort(short val, const char* pName);
    void PrintLong(long val, const char* pName);
    void PrintUshort(u_short val, const char* pName);
    void PrintUshortHex(u_short val, const char* pName);
    void PrintUlong(u_long val, const char* pName);
    void PrintUlongHex(u_long val, const char* pName);
    void PrintMatrix(const Matrix* pMtx, const char* pName);
    void PrintVec3Short(const Vec3Short* pVec, const char* pName);
    void PrintOt(const OrderingTable* pOt, const char* pName);
    void PrintPolyGT3(const POLY_GT3* pPoly, const char* pName);
    void PrintColor(const Color* pColor, const char* pName);
}

#endif
