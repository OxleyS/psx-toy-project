#include "GCRender.h"
#include "MathUtil.h"
#include "Camera.h"
#include "Mesh.h"
#include "FrameBuffer.h"
#include "GameObject.h"

void GCRender_Construct(GCRender* pSelf)
{
    pSelf->pMesh = NULL;
    M_IdentityMatrix(&pSelf->worldMtx);
}

void GCRender_DrawMesh(GCRender* pSelf, GORenderData* pRenderData)
{
    MATRIX mtx;

    if (!pSelf->pMesh) return;
    
    M_MulMatrixTransOut(&pSelf->worldMtx, Camera_GetMatrix(pRenderData->pCamera), &mtx);
	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);

	Mesh_Draw(pSelf->pMesh, pRenderData->frameBufIdx, &pRenderData->pFrameBuf->ot);
}