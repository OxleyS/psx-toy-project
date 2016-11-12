#include "GameObject.h"
#include "GCRender.h"

void GameObject_Construct(GameObject* pSelf)
{
    pSelf->updateFunc = GameObject_DefaultUpdate;
    pSelf->drawFunc = GameObject_DefaultDraw;
    pSelf->pGCRender = NULL;
}

void GameObject_DefaultUpdate(GameObject* pSelf)
{
}

void GameObject_DefaultDraw(GameObject* pSelf, GORenderData* pRenderData)
{
    if (pSelf->pGCRender) GCRender_DrawMesh(pSelf->pGCRender, pRenderData);
}