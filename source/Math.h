#ifndef _MATH_H_
#define _MATH_H_

#include "Global.h"

// TODO: Consider wrapping the libGPU stuff in custom classes instead
namespace Math
{
    void ZeroVector(SVECTOR* pOutVec);
    int DotProduct(const SVECTOR* pVec1, const SVECTOR* pVec2);

    void IdentityMatrix(MATRIX* pOutMtx);
    void IdentityMatrixRot(MATRIX* pOutMtx);
    void ZeroMatrixTrans(MATRIX* pOutMtx);
    void MulMatrixTrans(MATRIX* pMtx1, MATRIX* pMtx2);
    void MulMatrixTransOut(MATRIX* pMtx1, MATRIX* pMtx2, MATRIX* pOut);
}


#endif