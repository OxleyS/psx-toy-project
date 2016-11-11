#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Global.h"

typedef void (*GO_UpdateFunc)(GameObject* pSelf);
typedef void (*GO_DrawFunc)(GameObject* pSelf);

struct GameObject
{
    GO_UpdateFunc updateFunc;
    GO_DrawFunc drawFunc;
    void* pSubclass;
};

#endif