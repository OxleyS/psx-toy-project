#include "GCRender.h"
#include "MathUtil.h"
#include "Camera.h"
#include "Mesh.h"
#include "FrameBuffer.h"
#include "GameObject.h"

GCRender::GCRender()
{
    pMesh = NULL;
    M_IdentityMatrix(&worldMtx);
}

void GCRender::DrawMesh(GORenderData* pRenderData)
{
    MATRIX mtx;

    if (!pMesh) return;
    
    M_MulMatrixTransOut(&worldMtx, pRenderData->pCamera->GetCameraMatrix(), &mtx);
	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);

	pMesh->Draw(pRenderData->frameBufIdx, &pRenderData->pFrameBuf->ot);
}