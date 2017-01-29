#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#define INVALID_GAMEOBJ_ID (0xFF)

#include "Global.h"

class Camera;
class FrameBuffer;
class GCRender;

struct GORenderData
{
    Camera* pCamera;
    FrameBuffer* pFrameBuf;
};

class GameObject
{
    public:

    GameObject();

    virtual void Update();
    virtual void Draw(const GORenderData& renderData);

    u_char m_Id;
    GCRender* m_pGCRender;
};

#endif
