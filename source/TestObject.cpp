#include "TestObject.h"
#include "GameObject.h"
#include "GCRender.h"
#include "Mesh.h"
#include "Resources.h"

TestObject::TestObject()
{
    m_pGCRender = new GCRender;

	const char* pMeshName = "TEST.OXM";
	Mesh* pMesh = Resources::GetLoadedMesh(pMeshName);
	if (!pMesh) pMesh = Resources::BlockingMeshLoad(pMeshName);
    m_pGCRender->m_pMesh = pMesh;
}
