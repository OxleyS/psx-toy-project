#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Global.h"
#include "GameObject.h"
#include "Math.h"

struct Camera : GameObject
{
    public:
        Camera();

        void Update();

        void ClampRotations();
        Matrix* GetCameraMatrix();

        bool IsDebugMode() const { return m_bDebugMode; }
        void SetDebugMode(bool bDebugMode) { m_bDebugMode = bDebugMode; }
        
        short m_Yaw;
        short m_Pitch;
        short m_Roll;
        Matrix m_CameraMtx;
        Vec3Long m_Position;
        bool m_bDirty;
        
    private:
        void DebugUpdate();
        bool m_bDebugMode;
};

#endif
