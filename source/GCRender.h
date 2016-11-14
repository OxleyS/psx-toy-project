#ifndef _GC_RENDER_H_
#define _GC_RENDER_H_

#include "Global.h"

class GCRender
{
    public:

    GCRender();

    void DrawMesh(GORenderData* pRenderData);

    Mesh* m_pMesh;
    MATRIX m_WorldMtx;
};

#endif