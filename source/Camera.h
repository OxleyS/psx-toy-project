#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Global.h"
#include "GameObject.h"

struct Camera : GameObject
{
    public:

    Camera();

    void ClampRotations();
    MATRIX* GetCameraMatrix();

    short m_Yaw;
    short m_Pitch;
    short m_Roll;
    MATRIX m_CameraMtx;
    SVECTOR m_Position;
    u_char m_bDirty;
};

#endif