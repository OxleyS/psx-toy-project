#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Global.h"

class Camera;
class FrameBuffer;
class GCRender;

struct GORenderData
{
    Camera* pCamera;
    FrameBuffer* pFrameBuf;
    int frameBufIdx;
};

class GameObject
{
    public:

    GameObject();

    virtual void Update();
    virtual void Draw(GORenderData* pRenderData);

    GCRender* m_pGCRender;
};

#endif