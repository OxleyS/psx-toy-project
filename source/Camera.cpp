#include "Camera.h"
#include "Player.h"
#include "GCRender.h"
#include "Input.h"

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

        Vec3Long right = Vec3Long::FromShort(m_CameraMtx.GetRight());
        Vec3Long up = Vec3Long::FromShort(m_CameraMtx.GetUp());
        Vec3Long forward = Vec3Long::FromShort(m_CameraMtx.GetForward());

        m_CameraMtx.TransposeRot();

        // TODO: I'm sure this can be optimized somehow
        m_CameraMtx.t[0] = -(right.Dot(m_Position) / GTE_ONE);
        m_CameraMtx.t[1] = -(up.Dot(m_Position) / GTE_ONE);
        m_CameraMtx.t[2] = -(forward.Dot(m_Position) / GTE_ONE);

        m_bDirty = false;
    }

    return &m_CameraMtx;
}

void Camera::Update()
{
    if (Debug::IsDebugModeEnabled()) DebugUpdate();
}

void Camera::DebugUpdate()
{
    int xMove = 0, zMove = 0;
    static const int moveScalar = 40;

    if (Input::ButtonDown(Input::BUTTON_DLEFT, 0)) xMove -= moveScalar;
    if (Input::ButtonDown(Input::BUTTON_DRIGHT, 0)) xMove += moveScalar;
    if (Input::ButtonDown(Input::BUTTON_DUP, 0)) zMove += moveScalar;
    if (Input::ButtonDown(Input::BUTTON_DDOWN, 0)) zMove -= moveScalar;

    Matrix& mtx = *GetCameraMatrix();
    Vec3Long offset;
    offset.vx = (mtx.m[0][0] * xMove);
    offset.vy = (mtx.m[0][1] * xMove);
    offset.vz = (mtx.m[0][2] * xMove);
    offset.vx += (mtx.m[2][0] * zMove);
    offset.vy += (mtx.m[2][1] * zMove);
    offset.vz += (mtx.m[2][2] * zMove);

    m_Position += offset / GTE_ONE;
    m_bDirty = true;
}
