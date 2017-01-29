#include "GCRender.h"
#include "Math.h"
#include "Camera.h"
#include "Mesh.h"
#include "FrameBuffer.h"
#include "GameObject.h"

GCRender::GCRender()
{
    m_pMesh = NULL;
    m_WorldMtx = Matrix::Identity;
}

void GCRender::DrawMesh(const GORenderData& renderData)
{
    Matrix mtx;

    if (!m_pMesh) return;
    
    m_WorldMtx.Mul(*renderData.pCamera->GetCameraMatrix(), mtx);
	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);

	m_pMesh->Draw(renderData.pFrameBuf->m_Index, &renderData.pFrameBuf->m_Ot);
}
