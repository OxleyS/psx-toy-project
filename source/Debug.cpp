#include "Global.h"
#include "OrderingTable.h"
#include "Math.h"

#include <stdio.h>

namespace Debug
{
	void PrintHeaderToName(const char* pType, const char* pName)
	{
		printf("=========================\n");
		if (pName) printf("%s \"%s\"", pType, pName);
	}

	void PrintHeader(const char* pType, const char* pName)
	{
		PrintHeaderToName(pType, pName);
		if (pName) printf("\n=========================\n");
	}

	void PrintFooter()
	{
		printf("+++++++++++++++++++++++++\n");
	}

	void PrintShort(short val, const char* pName)
	{
		PrintHeaderToName("Short", pName);
		printf(": %hd\n", val);
		PrintFooter();
	}

	void PrintLong(long val, const char* pName)
	{
		PrintHeaderToName("Long", pName);
		printf(": %ld\n", val);
		PrintFooter();
	}

	void PrintUshort(u_short val, const char* pName)
	{
		PrintHeaderToName("Ushort", pName);
		printf(": %hu\n", val);
		PrintFooter();
	}

	void PrintUshortHex(u_short val, const char* pName)
	{
		PrintHeaderToName("Ushort", pName);
		printf(": %hx\n", val);
		PrintFooter();
	}

	void PrintUlong(u_long val, const char* pName)
	{
		PrintHeaderToName("Ulong", pName);
		printf(": %lu\n", val);
		PrintFooter();
	}

	void PrintUlongHex(u_long val, const char* pName)
	{
		PrintHeaderToName("Ulong", pName);
		printf(": 0x%lx\n", val);
		PrintFooter();
	}

	void PrintMatrix(const MATRIX* pMtx, const char* pName)
	{
		PrintHeader("Matrix", pName);
		printf("%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n%ld\t%ld\t%ld\n",
			pMtx->m[0][0], pMtx->m[0][1], pMtx->m[0][2],
			pMtx->m[1][0], pMtx->m[1][1], pMtx->m[1][2],
			pMtx->m[2][0], pMtx->m[2][1], pMtx->m[2][2],
			pMtx->t[0], pMtx->t[1], pMtx->t[2]);
		PrintFooter();
	}

	void PrintVec3Short(const Vec3Short* pVec, const char* pName)
	{
		PrintHeaderToName("Vec3Short", pName);
		printf(": %hd, %hd, %hd\n", pVec->vx, pVec->vy, pVec->vz);
		PrintFooter();
	}

	void PrintVec3Long(const Vec3Long* pVec, const char* pName)
	{
		PrintHeaderToName("Vec3Long", pName);
		printf(": %ld, %ld, %ld\n", pVec->vx, pVec->vy, pVec->vz);
		PrintFooter();
	}

	void PrintOt(const OrderingTable* pOt, const char* pName)
	{
		PrintHeader("OT", pName);
		DumpOTag(pOt->m_pEntries + (pOt->m_nEntries - 1));
		PrintFooter();
	}

	void PrintPolyG4(const POLY_G4* pPoly, const char* pName)
	{
		PrintHeader("PolyG4", pName);
		printf("XY0: %hd, %hd\n", pPoly->x0, pPoly->y0);
		printf("RGB0: %hhu, %hhu, %hhu\n", pPoly->r0, pPoly->g0, pPoly->b0);
		printf("XY1: %hd, %hd\n", pPoly->x1, pPoly->y1);
		printf("RGB1: %hhu, %hhu, %hhu\n", pPoly->r1, pPoly->g1, pPoly->b1);
		printf("XY2: %hd, %hd\n", pPoly->x2, pPoly->y2);
		printf("RGB2: %hhu, %hhu, %hhu\n", pPoly->r2, pPoly->g2, pPoly->b2);
		printf("XY3: %hd, %hd\n", pPoly->x3, pPoly->y3);
		printf("RGB3: %hhu, %hhu, %hhu\n", pPoly->r3, pPoly->g3, pPoly->b3);
		PrintFooter();
	}

	void PrintPolyGT3(const POLY_GT3* pPoly, const char* pName)
	{
		PrintHeader("PolyGT3", pName);
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
		PrintFooter();
	}

	void PrintPolyGT4(const POLY_GT4* pPoly, const char* pName)
	{
		PrintHeader("PolyGT4", pName);
		printf("XY0: %hd, %hd\n", pPoly->x0, pPoly->y0);
		printf("RGB0: %hhu, %hhu, %hhu\n", pPoly->r0, pPoly->g0, pPoly->b0);
		printf("UV0: %hhu, %hhu\n", pPoly->u0, pPoly->v0);
		printf("XY1: %hd, %hd\n", pPoly->x1, pPoly->y1);
		printf("RGB1: %hhu, %hhu, %hhu\n", pPoly->r1, pPoly->g1, pPoly->b1);
		printf("UV1: %hhu, %hhu\n", pPoly->u1, pPoly->v1);
		printf("XY2: %hd, %hd\n", pPoly->x2, pPoly->y2);
		printf("RGB2: %hhu, %hhu, %hhu\n", pPoly->r2, pPoly->g2, pPoly->b2);
		printf("UV2: %hhu, %hhu\n", pPoly->u2, pPoly->v2);
		printf("XY3: %hd, %hd\n", pPoly->x3, pPoly->y3);
		printf("RGB3: %hhu, %hhu, %hhu\n", pPoly->r3, pPoly->g3, pPoly->b3);
		printf("UV3: %hhu, %hhu\n", pPoly->u3, pPoly->v3);
		printf("CLUT: %hu, TPage: %hu\n", pPoly->clut, pPoly->tpage);
		PrintFooter();
	}

	void PrintColor(const CVECTOR* pColor, const char* pName)
	{
		PrintHeaderToName("COLOR", pName);
		printf(": %hhu, %hhu, %hhu\n", pColor->r, pColor->g, pColor->b);
		PrintFooter();
	}
}
