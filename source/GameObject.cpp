#include "GameObject.h"
#include "GCRender.h"

GameObject::GameObject()
{
    m_Id = INVALID_GAMEOBJ_ID;
    m_pGCRender = NULL;
}

void GameObject::Update()
{
}

void GameObject::Draw(const GORenderData& renderData)
{
    if (m_pGCRender) m_pGCRender->DrawMesh(renderData);
}
