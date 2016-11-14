#include "Camera.h"
#include "MathUtil.h"

Camera::Camera()
{
    yaw = 0;
    pitch = 0;
    roll = 0;
    M_IdentityMatrix(&cameraMtx);
    M_ZeroVector(&position);
    bDirty = 0;
}

void Camera::ClampRotations()
{
    if (yaw < 0) yaw += 4096;
    if (yaw > 4096) yaw -= 4096;
    if (pitch < 0) pitch += 4096;
    if (pitch > 4096) pitch -= 4096;
    if (roll < 0) roll += 4096;
    if (roll > 4096) roll -= 4096;
    bDirty = 1;
}

MATRIX* Camera::GetCameraMatrix() 
{
    if (bDirty)
    {
        SVECTOR rotAmt = { pitch, yaw, roll, 0 };
        RotMatrixYXZ_gte(&rotAmt, &cameraMtx);
        TransposeMatrix(&cameraMtx, &cameraMtx);
        cameraMtx.t[0] = -(M_DotProduct((SVECTOR*)cameraMtx.m[0], &position) >> 12);
        cameraMtx.t[1] = -(M_DotProduct((SVECTOR*)cameraMtx.m[1], &position) >> 12);
        cameraMtx.t[2] = -(M_DotProduct((SVECTOR*)cameraMtx.m[2], &position) >> 12);
        bDirty = 0;
    }

    return &cameraMtx;
}
