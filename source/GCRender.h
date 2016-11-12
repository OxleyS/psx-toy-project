#ifndef _GC_RENDER_H_
#define _GC_RENDER_H_

#include "Global.h"

struct GCRender
{
    Mesh* pMesh;
    MATRIX worldMtx;
};

void GCRender_Construct(GCRender* pSelf);
void GCRender_DrawMesh(GCRender* pSelf, GORenderData* pRenderData);

#endif