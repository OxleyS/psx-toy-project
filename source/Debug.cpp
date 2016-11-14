#include "Debug.h"
#include "OrderingTable.h"

#include <stdio.h>

void Debug::PrintHeaderToName(const char* pType, const char* pName)
{
	printf("=========================\n");
	if (pName) printf("%s \"%s\"", pType, pName);
}

void Debug::PrintHeader(const char* pType, const char* pName)
{
	Debug::PrintHeaderToName(pType, pName);
	if (pName) printf("\n=========================\n");
}

void Debug::PrintFooter()
{
	printf("+++++++++++++++++++++++++\n");
}

void Debug::PrintShort(short val, const char* pName)
{
	Debug::PrintHeaderToName("Short", pName);
	printf(": %hd\n", val);
	Debug::PrintFooter();
}

void Debug::PrintLong(long val, const char* pName)
{
	Debug::PrintHeaderToName("Long", pName);
	printf(": %ld\n", val);
	Debug::PrintFooter();
}

void Debug::PrintUshort(u_short val, const char* pName)
{
	Debug::PrintHeaderToName("Ushort", pName);
	printf(": %hu\n", val);
	Debug::PrintFooter();
}

void Debug::PrintUshortHex(u_short val, const char* pName)
{
	Debug::PrintHeaderToName("Ushort", pName);
	printf(": %hx\n", val);
	Debug::PrintFooter();
}

void Debug::PrintUlong(u_long val, const char* pName)
{
	Debug::PrintHeaderToName("Ulong", pName);
	printf(": %lu\n", val);
	Debug::PrintFooter();
}

void Debug::PrintUlongHex(u_long val, const char* pName)
{
	Debug::PrintHeaderToName("Ulong", pName);
	printf(": 0x%lx\n", val);
	Debug::PrintFooter();
}

void Debug::PrintMatrix(const MATRIX* pMtx, const char* pName)
{
	Debug::PrintHeader("Matrix", pName);
	printf("%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n%ld\t%ld\t%ld\n",
		pMtx->m[0][0], pMtx->m[0][1], pMtx->m[0][2],
		pMtx->m[1][0], pMtx->m[1][1], pMtx->m[1][2],
		pMtx->m[2][0], pMtx->m[2][1], pMtx->m[2][2],
		pMtx->t[0], pMtx->t[1], pMtx->t[2]);
	Debug::PrintFooter();
}

void Debug::PrintSvector(const SVECTOR* pVec, const char* pName)
{
	Debug::PrintHeaderToName("SVECTOR", pName);
	printf(": %hd, %hd, %hd\n", pVec->vx, pVec->vy, pVec->vz);
	Debug::PrintFooter();
}

void Debug::PrintOt(const OrderingTable* pOt, const char* pName)
{
	Debug::PrintHeader("OT", pName);
	DumpOTag(pOt->m_pEntries + (pOt->m_nEntries - 1));
	Debug::PrintFooter();
}

void Debug::PrintPolyGT3(const POLY_GT3* pPoly, const char* pName)
{
	Debug::PrintHeader("PolyGT3", pName);
	printf("XY0: %hd, %hd\n", pPoly->x0, pPoly->y0);
	printf("RGB0: %hhu, %hhu, %hhu\n", pPoly->r0, pPoly->g0, pPoly->b0);
	printf("UV0: %hhu, %hhu\n", pPoly->u0, pPoly->v0);
	printf("XY1: %hd, %hd\n", pPoly->x1, pPoly->y1);
	printf("RGB1: %hhu, %hhu, %hhu\n", pPoly->r1, pPoly->g1, pPoly->b1);
	printf("UV1: %hhu, %hhu\n", pPoly->u1, pPoly->v1);
	printf("XY2: %hd, %hd\n", pPoly->x2, pPoly->y2);
	printf("RGB2: %hhu, %hhu, %hhu\n", pPoly->r2, pPoly->g2, pPoly->b2);
	printf("UV2: %hhu, %hhu\n", pPoly->u2, pPoly->v2);
	printf("CLUT: %hu, TPage: %hu\n", pPoly->clut, pPoly->tpage);
	Debug::PrintFooter();
}
