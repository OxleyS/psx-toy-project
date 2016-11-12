#ifndef _CUBE_H_
#define _CUBE_H_

#include "Global.h"
#include "GameObject.h"

extern MeshTriGour gCubeTris[];
extern int gnCubeTris;

typedef struct Cube
{
    GameObject gameObj;
} Cube;

void Cube_Construct(Cube* pSelf);

#endif