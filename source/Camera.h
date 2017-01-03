#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Global.h"
#include "GameObject.h"
#include "Math.h"

struct Camera : GameObject
{
    public:

    Camera();

    void ClampRotations();
    Matrix* GetCameraMatrix();

    short m_Yaw;
    short m_Pitch;
    short m_Roll;
    Matrix m_CameraMtx;
    Vec3Long m_Position;
    bool m_bDirty;
};

#endif
