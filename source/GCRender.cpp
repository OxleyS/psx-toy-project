#include "GCRender.h"
#include "Math.h"
#include "Camera.h"
#include "Mesh.h"
#include "FrameBuffer.h"
#include "GameObject.h"
#include "Debug.h"

GCRender::GCRender()
{
    m_pMesh = NULL;
    Math::IdentityMatrix(&m_WorldMtx);
}

void GCRender::DrawMesh(GORenderData* pRenderData)
{
    MATRIX mtx;

    if (!m_pMesh) return;
    
    Math::MulMatrixTransOut(&m_WorldMtx, pRenderData->pCamera->GetCameraMatrix(), &mtx);
	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);

	m_pMesh->Draw(pRenderData->frameBufIdx, &pRenderData->pFrameBuf->m_Ot);
}