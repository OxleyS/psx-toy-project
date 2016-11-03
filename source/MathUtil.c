#include "MathUtil.h"

void M_ZeroVector(SVECTOR* pOutVec)
{
	memset(pOutVec, 0, sizeof(SVECTOR));
}

int M_DotProduct(const SVECTOR* pVec1, const SVECTOR* pVec2)
{
	int result = pVec1->vx * pVec2->vx;
	result += pVec1->vy * pVec2->vy;
	result += pVec1->vz * pVec2->vz;
	return result;
}

void M_IdentityMatrix(MATRIX* pOutMtx)
{
	memset(pOutMtx, 0, sizeof(MATRIX));
	pOutMtx->m[0][0] = 0x1000;
	pOutMtx->m[1][1] = 0x1000;
	pOutMtx->m[2][2] = 0x1000;
}

void M_IdentityMatrixRot(MATRIX* pOutMtx)
{
	memset(pOutMtx->m, 0, sizeof(pOutMtx->m));
	pOutMtx->m[0][0] = 0x1000;
	pOutMtx->m[1][1] = 0x1000;
	pOutMtx->m[2][2] = 0x1000;
}

void M_ZeroMatrixTrans(MATRIX* pOutMtx)
{
	pOutMtx->t[0] = 0;
	pOutMtx->t[1] = 0; 
	pOutMtx->t[2] = 0;
}

void M_MulMatrixTrans(MATRIX* pMtx1, MATRIX* pMtx2)
{
	// Multiplies just the rotation part of the matrix
	MulMatrix(pMtx1, pMtx2);

	// Handle the translation part
	ApplyMatrixLV(pMtx2, pMtx1->t, pMtx1->t);
}
