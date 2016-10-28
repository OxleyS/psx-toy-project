#ifndef _DEBUG_UTIL_H_
#define _DEBUG_UTIL_H_

#include "Global.h"

#include <libgte.h>

void DebugUtil_PrintHeaderToName(const char* pType, const char* pName);
void DebugUtil_PrintHeader(const char* pType, const char* pName);
void DebugUtil_PrintFooter();
void DebugUtil_PrintUshort(u_short val, const char* pName);
void DebugUtil_PrintUshortHex(u_short val, const char* pName);
void DebugUtil_PrintUlong(u_long val, const char* pName);
void DebugUtil_PrintUlongHex(u_long val, const char* pName);
void DebugUtil_PrintMatrix(const MATRIX* pMtx, const char* pName);
void DebugUtil_PrintOt(const OrderingTable* pOt, const char* pName);

#endif
