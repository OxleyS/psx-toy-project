#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Global.h"

#include <libgte.h>
#include <libgpu.h>

struct Camera
{
    short yaw;
    short pitch;
    short roll;
    MATRIX cameraMtx;
    SVECTOR position;
    u_char bDirty;
};

void Camera_Construct(Camera* pSelf);
void Camera_ClampRotations(Camera* pSelf);
MATRIX* Camera_GetMatrix(Camera* pSelf);

#endif