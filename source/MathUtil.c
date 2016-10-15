#include "MathUtil.h"

void MathUtil_IdentityMatrix(MATRIX* pOutMtx)
{
	memset(pOutMtx, 0, sizeof(MATRIX));
	pOutMtx->m[0][0] = 0x1000;
	pOutMtx->m[1][1] = 0x1000;
	pOutMtx->m[2][2] = 0x1000;
}

void MathUtil_ZeroMatrixTrans(MATRIX* pOutMtx)
{
	pOutMtx->t[0] = 0; pOutMtx->t[1] = 0; pOutMtx->t[2] = 0;
}
