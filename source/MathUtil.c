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
	long* pT;
	long newT[3]; 

	// Multiplies just the rotation part of the matrix
	MulMatrix(pMtx1, pMtx2);

	// A row 4 dot B col 1, A row 4 dot B col 2, etc.
	pT = pMtx1->t;
	newT[0] = ((pT[0] * pMtx2->m[0][0] + pT[1] * pMtx2->m[0][1] + pT[2] * pMtx2->m[0][2]) >> 12) + pMtx2->t[0];
	newT[1] = ((pT[0] * pMtx2->m[1][0] + pT[1] * pMtx2->m[1][1] + pT[2] * pMtx2->m[1][2]) >> 12) + pMtx2->t[1];
	newT[2] = ((pT[0] * pMtx2->m[2][0] + pT[1] * pMtx2->m[2][1] + pT[2] * pMtx2->m[2][2]) >> 12) + pMtx2->t[2];
	pT[0] = newT[0];
	pT[1] = newT[1];
	pT[2] = newT[2];
}
