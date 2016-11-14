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

    short yaw;
    short pitch;
    short roll;
    MATRIX cameraMtx;
    SVECTOR position;
    u_char bDirty;
};

#endif