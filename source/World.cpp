#include "World.h"
#include "GameObject.h"
#include "Camera.h"
#include "FrameBuffer.h"

World::World()
{
    for (int i = 0; i < MAX_WORLD_OBJECTS; i++)
    {
        m_Objects[0].pObject = NULL;
        m_FreeIdList[i] = i;
    }
    m_nObjects = 0;
}

World::~World()
{
    for (int i = 0; i < MAX_WORLD_OBJECTS; i++)
    {
        delete m_Objects[i].pObject;
    }
}

void World::Update()
{
    for (int i = 0; i < MAX_WORLD_OBJECTS; i++)
    {
        GameObjectEntry& entry = m_Objects[i];
        if (entry.pObject) m_Objects[i].pObject->Update();
    }
}

void World::Draw(Camera* pCamera, FrameBuffer* pFrameBuf)
{
    GORenderData renderData;
    renderData.pCamera = pCamera;
    renderData.pFrameBuf = pFrameBuf;

    for (int i = 0; i < MAX_WORLD_OBJECTS; i++)
    {
        GameObjectEntry& entry = m_Objects[i];
        if (entry.pObject) entry.pObject->Draw(renderData);
    }
}

bool World::AddObject(GameObject* pGameObj)
{
    assert(pGameObj);
    if (m_nObjects == MAX_WORLD_OBJECTS) return false;

    u_char freeId = m_FreeIdList[m_nObjects++];
    m_Objects[freeId].pObject = pGameObj;
    pGameObj->m_Id = freeId;

    return true;
}

void World::DeleteObject(u_char id)
{
    assert(id < MAX_WORLD_OBJECTS);

    if (m_Objects[id].pObject)
    {
        delete m_Objects[id].pObject;
        m_Objects[id].pObject = NULL;
        m_FreeIdList[--m_nObjects] = id;
    }
}
