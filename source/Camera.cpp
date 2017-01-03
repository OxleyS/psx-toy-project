#include "Camera.h"

Camera::Camera()
{
    m_Yaw = 0;
    m_Pitch = 0;
    m_Roll = 0;
    m_CameraMtx = Matrix::Identity; 
    m_Position = Vec3Long::Zero;
    m_bDirty = false;
}

void Camera::ClampRotations()
{
    if (m_Yaw < 0) m_Yaw += 4096;
    if (m_Yaw > 4096) m_Yaw -= 4096;
    if (m_Pitch < 0) m_Pitch += 4096;
    if (m_Pitch > 4096) m_Pitch -= 4096;
    if (m_Roll < 0) m_Roll += 4096;
    if (m_Roll > 4096) m_Roll -= 4096;
    m_bDirty = true;
}

Matrix* Camera::GetCameraMatrix() 
{
    if (m_bDirty)
    {
        Vec3Short rotAmt(m_Pitch, m_Yaw, m_Roll);
        RotMatrixYXZ_gte(&rotAmt, &m_CameraMtx);
        TransposeMatrix(&m_CameraMtx, &m_CameraMtx);

        Vec3Long xformed;
        ApplyMatrixLV(&m_CameraMtx, &m_Position, &xformed);
        xformed = -xformed;
        TransMatrix(&m_CameraMtx, &xformed);

        m_bDirty = false;
    }

    return &m_CameraMtx;
}
