#ifndef _WORLD_H_
#define _WORLD_H_

#define MAX_WORLD_OBJECTS (100)

#include "Global.h"

class GameObject;
class Camera;
class FrameBuffer;

class World
{
    public:
        World();
        ~World();

        bool AddObject(GameObject* pObj);
        void DeleteObject(u_char id);

        void Update();
        void Draw(Camera* pCamera, FrameBuffer* pFrameBuf);

        struct GameObjectEntry
        {
            GameObject* pObject;
        };

        GameObjectEntry m_Objects[MAX_WORLD_OBJECTS];
        u_char m_FreeIdList[MAX_WORLD_OBJECTS];
        u_char m_nObjects;
};

#endif
