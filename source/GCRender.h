#ifndef _GC_RENDER_H_
#define _GC_RENDER_H_

#include "Global.h"

class GCRender
{
    public:

    GCRender();

    void DrawMesh(GORenderData* pRenderData);

    Mesh* pMesh;
    MATRIX worldMtx;
};

#endif