#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Global.h"
#include "GameObject.h"
#include "Math.h"

class Player : public GameObject
{
    public:
        Player();

        void Update();

        void SetFollowCamera(Camera* pCamera) { m_pFollowCamera = pCamera; }

    private:
        void ProcessInput();
        void UpdateFollowCamera();
        void UpdateFaceTarget();
        void TestCollision();

        Camera* m_pFollowCamera;
        Vec3Short m_TargetForward;
};

#endif
