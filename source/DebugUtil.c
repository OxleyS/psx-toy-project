#include "DebugUtil.h"

void DebugUtil_PrintHeaderToName(const char* pType, const char* pName)
{
	printf("====================\n");
	if (pName) printf("%s \"%s\"", pType, pName);
}

void DebugUtil_PrintHeader(const char* pType, const char* pName)
{
	DebugUtil_PrintHeaderToName(pType, pName);
	if (pName) printf("\n=========================\n");
}

void DebugUtil_PrintFooter()
{
	printf("++++++++++++++++++++\n");
}

void DebugUtil_PrintUlong(u_long val, const char* pName)
{
	DebugUtil_PrintHeaderToName("Ulong", pName);
	printf(": %d\n", val);
	DebugUtil_PrintFooter();
}

void DebugUtil_PrintMatrix(const MATRIX* pMtx, const char* pName)
{
	DebugUtil_PrintHeader("Matrix", pName);
	printf("%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n",
		pMtx->m[0][0], pMtx->m[0][1], pMtx->m[0][2],
		pMtx->m[1][0], pMtx->m[1][1], pMtx->m[1][2],
		pMtx->m[2][0], pMtx->m[2][1], pMtx->m[2][2],
		pMtx->t[0], pMtx->t[1], pMtx->t[2]);
	DebugUtil_PrintFooter();
}
