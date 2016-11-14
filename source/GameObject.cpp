#include "GameObject.h"
#include "GCRender.h"

GameObject::GameObject()
{
    pGCRender = NULL;
}

void GameObject::Update()
{
}

void GameObject::Draw(GORenderData* pRenderData)
{
    if (pGCRender) pGCRender->DrawMesh(pRenderData);
}