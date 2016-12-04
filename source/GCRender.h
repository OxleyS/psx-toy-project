#ifndef _GC_RENDER_H_
#define _GC_RENDER_H_

#include "Global.h"
#include "Math.h"

class GORenderData;
class Mesh;

class GCRender
{
    public:

    GCRender();

    void DrawMesh(GORenderData* pRenderData);

    Mesh* m_pMesh;
    Matrix m_WorldMtx;
};

#endif