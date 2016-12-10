#include "Resources.h"
#include "Mesh.h"

#include <libds.h>
#include <libapi.h>

Resources::Resource Resources::m_sMeshes[MAX_LOADED_MESHES];
int Resources::m_snMeshes = 0;
Resources::Resource* Resources::m_spLoadingResource = NULL;
Resources::ResourceType Resources::m_sLoadingResourceType;
char Resources::m_sPathBuffer[MAX_PATH_SIZE];
int Resources::m_snLoadingResourceBytes;
u_long* Resources::m_spLoadBuffer = NULL;
int Resources::m_snLoadBufferWords = 0;

Mesh* Resources::GetLoadedMesh(const char* pName)
{
    for (int i = 0; i < m_snMeshes; i++)
    {
        Resource& res = m_sMeshes[i];
        if (String::Compare(res.pName, pName) == 0 && res.nRemainingSectors == 0)
            return (Mesh*)res.pResource;
    }

    return NULL;
}

Mesh* Resources::BlockingMeshLoad(const char* pName)
{
    Resources::BlockForResourceLoad();
    Resources::BeginMeshLoad(pName);
    return (Mesh*)Resources::BlockForResourceLoad();
}

bool Resources::BeginMeshLoad(const char* pName)
{
    assert(!GetLoadedMesh(pName));

    if (m_spLoadingResource) return false;
    if (m_snMeshes == MAX_LOADED_MESHES) return false;

    int length = String::Copy(m_sPathBuffer, "\\OXM\\");
    length += String::Copy(m_sPathBuffer + length, pName);
    length += String::Copy(m_sPathBuffer + length, ";1");
    assert(length < MAX_PATH_SIZE);

    m_spLoadingResource = &m_sMeshes[m_snMeshes++];
    m_spLoadingResource->pName = String::NewDup(pName);
    m_sLoadingResourceType = RT_MESH;
    m_spLoadingResource->pResource = new Mesh;
    BeginResourceLoad(m_sPathBuffer);

    return true;
}

void Resources::UnloadMesh(const char* pName)
{
    for (int i = 0; i < m_snMeshes; i++)
    {
        Resource& res = m_sMeshes[i];
        if (String::Compare(res.pName, pName) == 0)
        {
            if (&res == m_spLoadingResource)
            {
                DsReadBreak();
                m_spLoadingResource = NULL;
            }

            delete (Mesh*)res.pResource;
            delete[] res.pName;
            Memory::Move(m_sMeshes + i, m_sMeshes + i + 1, (m_snMeshes - i - 1) * sizeof(Resource));
            m_snMeshes--;

            return;
        }
    }
}

void Resources::BeginResourceLoad(const char* pFilePath)
{
    DslFILE fileInfo;
	if (DsSearchFile(&fileInfo, (char*)pFilePath) == 0)
	{
		printf("FATAL: MISSING FILE (%s)\n", pFilePath);
		SystemError('f', 666);
	}
	
	int nSectors = (fileInfo.size + 2047) / 2048;
    int neededWords = (nSectors * 2048) / sizeof(u_long);
    if (neededWords > m_snLoadBufferWords)
    {
        delete[] m_spLoadBuffer;
        m_spLoadBuffer = new u_long[neededWords];
        m_snLoadBufferWords = neededWords;
    }
	
	DsRead(&fileInfo.pos, nSectors, m_spLoadBuffer, DslModeSpeed);
    m_spLoadingResource->nRemainingSectors = nSectors;
    m_snLoadingResourceBytes = fileInfo.size;
}

void* Resources::Update()
{
    u_char execResult[8];

    if (m_spLoadingResource)
    {
        int nRemainingSectors = DsReadSync(execResult);
        m_spLoadingResource->nRemainingSectors = nRemainingSectors;
        if (nRemainingSectors <= 0)
        {
            if (nRemainingSectors != 0)
            {
                printf("FATAL: READ FAIL (%s)", m_sPathBuffer);
                SystemError('r', 70);
            }

            void* pRes = m_spLoadingResource->pResource;

            switch (m_sLoadingResourceType)
            {
                case RT_MESH:
                    ((Mesh*)pRes)->InitFromLoadedOxm(m_spLoadBuffer, m_snLoadingResourceBytes);
                    break;
            }

            m_spLoadingResource = NULL;
            return pRes;
        }
    }

    return NULL;
}

void* Resources::BlockForResourceLoad()
{
    void* pResource = NULL;
    while (m_spLoadingResource) pResource = Update();
    return pResource;
}
