#include "Camera.h"
#include "Math.h"

Camera::Camera()
{
    m_Yaw = 0;
    m_Pitch = 0;
    m_Roll = 0;
    Math::IdentityMatrix(&m_CameraMtx);
    Math::ZeroVector(&m_Position);
    m_bDirty = 0;
}

void Camera::ClampRotations()
{
    if (m_Yaw < 0) m_Yaw += 4096;
    if (m_Yaw > 4096) m_Yaw -= 4096;
    if (m_Pitch < 0) m_Pitch += 4096;
    if (m_Pitch > 4096) m_Pitch -= 4096;
    if (m_Roll < 0) m_Roll += 4096;
    if (m_Roll > 4096) m_Roll -= 4096;
    m_bDirty = 1;
}

MATRIX* Camera::GetCameraMatrix() 
{
    if (m_bDirty)
    {
        SVECTOR rotAmt = { m_Pitch, m_Yaw, m_Roll, 0 };
        RotMatrixYXZ_gte(&rotAmt, &m_CameraMtx);
        TransposeMatrix(&m_CameraMtx, &m_CameraMtx);
        m_CameraMtx.t[0] = -(Math::DotProduct((SVECTOR*)m_CameraMtx.m[0], &m_Position) >> 12);
        m_CameraMtx.t[1] = -(Math::DotProduct((SVECTOR*)m_CameraMtx.m[1], &m_Position) >> 12);
        m_CameraMtx.t[2] = -(Math::DotProduct((SVECTOR*)m_CameraMtx.m[2], &m_Position) >> 12);
        m_bDirty = 0;
    }

    return &m_CameraMtx;
}
