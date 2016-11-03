#include "Camera.h"
#include "MathUtil.h"

void Camera_Construct(Camera* pSelf)
{
    pSelf->yaw = 0;
    pSelf->pitch = 0;
    pSelf->roll = 0;
    M_IdentityMatrix(&pSelf->cameraMtx);
    M_ZeroVector(&pSelf->position);
    pSelf->bDirty = 0;
}

void Camera_ClampRotations(Camera* pSelf)
{
    if (pSelf->yaw < 0) pSelf->yaw += 4096;
    if (pSelf->yaw > 4096) pSelf->yaw -= 4096;
    if (pSelf->pitch < 0) pSelf->pitch += 4096;
    if (pSelf->pitch > 4096) pSelf->pitch -= 4096;
    if (pSelf->roll < 0) pSelf->roll += 4096;
    if (pSelf->roll > 4096) pSelf->roll -= 4096;
    pSelf->bDirty = 1;
}

MATRIX* Camera_GetMatrix(Camera* pSelf) 
{
    if (pSelf->bDirty)
    {
        SVECTOR rotAmt = { pSelf->pitch, pSelf->yaw, pSelf->roll, 0 };
        RotMatrixYXZ_gte(&rotAmt, &pSelf->cameraMtx);
        TransposeMatrix(&pSelf->cameraMtx, &pSelf->cameraMtx);
        pSelf->cameraMtx.t[0] = -(M_DotProduct((SVECTOR*)pSelf->cameraMtx.m[0], &pSelf->position) >> 12);
        pSelf->cameraMtx.t[1] = -(M_DotProduct((SVECTOR*)pSelf->cameraMtx.m[1], &pSelf->position) >> 12);
        pSelf->cameraMtx.t[2] = -(M_DotProduct((SVECTOR*)pSelf->cameraMtx.m[2], &pSelf->position) >> 12);
        pSelf->bDirty = 0;
    }

    return &pSelf->cameraMtx;
}
