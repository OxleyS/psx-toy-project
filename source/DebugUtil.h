#ifndef _DEBUG_UTIL_H_
#define _DEBUG_UTIL_H_

#include "Global.h"

#include <libgte.h>

void Debug_PrintHeaderToName(const char* pType, const char* pName);
void Debug_PrintHeader(const char* pType, const char* pName);
void Debug_PrintFooter();
void Debug_PrintUshort(u_short val, const char* pName);
void Debug_PrintUshortHex(u_short val, const char* pName);
void Debug_PrintUlong(u_long val, const char* pName);
void Debug_PrintUlongHex(u_long val, const char* pName);
void Debug_PrintMatrix(const MATRIX* pMtx, const char* pName);
void Debug_PrintOt(const OrderingTable* pOt, const char* pName);

#endif
