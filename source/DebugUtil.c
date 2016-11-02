#include "DebugUtil.h"
#include "OrderingTable.h"

#include <stdio.h>
#include <libgte.h>
#include <libgpu.h>

void Debug_PrintHeaderToName(const char* pType, const char* pName)
{
	printf("=========================\n");
	if (pName) printf("%s \"%s\"", pType, pName);
}

void Debug_PrintHeader(const char* pType, const char* pName)
{
	Debug_PrintHeaderToName(pType, pName);
	if (pName) printf("\n=========================\n");
}

void Debug_PrintFooter()
{
	printf("++++++++++++++++++++\n");
}

void Debug_PrintUshort(u_short val, const char* pName)
{
	Debug_PrintHeaderToName("Ushort", pName);
	printf(": %hu\n", val);
	Debug_PrintFooter();
}

void Debug_PrintUshortHex(u_short val, const char* pName)
{
	Debug_PrintHeaderToName("Ushort", pName);
	printf(": %hx\n", val);
	Debug_PrintFooter();
}

void Debug_PrintUlong(u_long val, const char* pName)
{
	Debug_PrintHeaderToName("Ulong", pName);
	printf(": %lu\n", val);
	Debug_PrintFooter();
}

void Debug_PrintUlongHex(u_long val, const char* pName)
{
	Debug_PrintHeaderToName("Ulong", pName);
	printf(": 0x%lx\n", val);
	Debug_PrintFooter();
}

void Debug_PrintMatrix(const MATRIX* pMtx, const char* pName)
{
	Debug_PrintHeader("Matrix", pName);
	printf("%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n%ld\t%ld\t%ld\n",
		pMtx->m[0][0], pMtx->m[0][1], pMtx->m[0][2],
		pMtx->m[1][0], pMtx->m[1][1], pMtx->m[1][2],
		pMtx->m[2][0], pMtx->m[2][1], pMtx->m[2][2],
		pMtx->t[0], pMtx->t[1], pMtx->t[2]);
	Debug_PrintFooter();
}

void Debug_PrintOt(const OrderingTable* pOt, const char* pName)
{
	Debug_PrintHeader("OT", pName);
	DumpOTag(pOt->pEntries + (pOt->nEntries - 1));
	Debug_PrintFooter();
}
