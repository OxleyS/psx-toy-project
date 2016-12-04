#include "Camera.h"

Camera::Camera()
{
    m_Yaw = 0;
    m_Pitch = 0;
    m_Roll = 0;
    m_CameraMtx = Matrix::Identity; 
    m_Position = Vec3Short::Zero;
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

Matrix* Camera::GetCameraMatrix() 
{
    if (m_bDirty)
    {
        Vec3Short rotAmt(m_Pitch, m_Yaw, m_Roll);
        RotMatrixYXZ_gte(&rotAmt, &m_CameraMtx);
        TransposeMatrix(&m_CameraMtx, &m_CameraMtx);

        // TODO: Won't really work with position being Vec3Short
        // May need to regen cam matrix per-object using position
        // relative to the camera (but transposed rotation can be cached)
        m_CameraMtx.t[0] = -(((const Vec3Short&)m_CameraMtx.m[0][0]).Dot(m_Position) >> 12);
        m_CameraMtx.t[1] = -(((const Vec3Short&)m_CameraMtx.m[1][0]).Dot(m_Position) >> 12);
        m_CameraMtx.t[2] = -(((const Vec3Short&)m_CameraMtx.m[2][0]).Dot(m_Position) >> 12);

        m_bDirty = 0;
    }

    return &m_CameraMtx;
}
