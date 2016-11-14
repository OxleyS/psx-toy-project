#include "GameObject.h"
#include "GCRender.h"

GameObject::GameObject()
{
    m_pGCRender = NULL;
}

void GameObject::Update()
{
}

void GameObject::Draw(GORenderData* pRenderData)
{
    if (m_pGCRender) m_pGCRender->DrawMesh(pRenderData);
}