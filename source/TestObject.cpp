#include "TestObject.h"
#include "GameObject.h"
#include "GCRender.h"
#include "Mesh.h"

#include <libds.h>
#include <libapi.h>

TestObject::TestObject()
{
    m_pGCRender = new GCRender;

    DslFILE fileInfo;
	const char* pFilePath = "\\OXM\\TEST.OXM;1";
	// TODO: Avoid unsigned issues
	if (DsSearchFile(&fileInfo, (char*)pFilePath) <= 0)
	{
		printf("MISSING FILE %s\n", pFilePath);
		SystemError('f', 666);
	}
	
	int nSectors = (fileInfo.size + 2047) / 2048;
	int nRemainingSectors;
	u_char execResult[8];
	u_long* pCdBuf = new u_long[(nSectors * 2048) / sizeof(u_long)];
	DsRead(&fileInfo.pos, nSectors, pCdBuf, DslModeSpeed);
	do { nRemainingSectors = DsReadSync(execResult); } while(nRemainingSectors);

    Mesh* pMesh = new Mesh;
    pMesh->InitFromLoadedOxm(pCdBuf, fileInfo.size);
    m_pGCRender->m_pMesh = pMesh;
}
