#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "Global.h"

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
    void PrintMatrix(const MATRIX* pMtx, const char* pName);
    void PrintSvector(const SVECTOR* pVec, const char* pName);
    void PrintOt(const OrderingTable* pOt, const char* pName);
    void PrintPolyGT3(const POLY_GT3* pPoly, const char* pName);
}

#endif
