#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Global.h"

struct GORenderData
{
    Camera* pCamera;
    FrameBuffer* pFrameBuf;
    int frameBufIdx;
};

typedef void (*GO_UpdateFunc)(GameObject* pSelf);
typedef void (*GO_DrawFunc)(GameObject* pSelf, GORenderData* pRenderData);

struct GameObject
{
    GO_UpdateFunc updateFunc;
    GO_DrawFunc drawFunc;
    GCRender* pGCRender;
};

void GameObject_Construct(GameObject* pSelf);
void GameObject_DefaultUpdate(GameObject* pSelf);
void GameObject_DefaultDraw(GameObject* pSelf, GORenderData* pRenderData);

#endif