#include "DebugUtil.h"

void DebugUtil_PrintMatrix(const MATRIX* pMtx, const char* pName)
{
	printf("====================\n");
	if (pName) printf("Matrix \"%s\"\n=========================\n", pName);
	printf("%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n%d\t%d\t%d\n",
		pMtx->m[0][0], pMtx->m[0][1], pMtx->m[0][2],
		pMtx->m[1][0], pMtx->m[1][1], pMtx->m[1][2], 
		pMtx->m[2][0], pMtx->m[2][1], pMtx->m[2][2], 
		pMtx->t[0], pMtx->t[1], pMtx->t[2]);
	printf("++++++++++++++++++++\n");
}