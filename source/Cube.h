#ifndef _CUBE_H_
#define _CUBE_H_

#include "Global.h"
#include "GameObject.h"

extern MeshTriGour g_CubeTris[];
extern int g_nCubeTris;

class Cube : public GameObject
{
    public:

    Cube();
};

#endif