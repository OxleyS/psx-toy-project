#ifndef _MATH_UTIL_H_
#define _MATH_UTIL_H_

#include "Global.h"

void M_ZeroVector(SVECTOR* pOutVec);
int M_DotProduct(const SVECTOR* pVec1, const SVECTOR* pVec2);

void M_IdentityMatrix(MATRIX* pOutMtx);
void M_IdentityMatrixRot(MATRIX* pOutMtx);
void M_ZeroMatrixTrans(MATRIX* pOutMtx);
void M_MulMatrixTrans(MATRIX* pMtx1, MATRIX* pMtx2);
void M_MulMatrixTransOut(MATRIX* pMtx1, MATRIX* pMtx2, MATRIX* pOut);

#endif