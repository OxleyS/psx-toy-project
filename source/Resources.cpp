#include "Resources.h"
#include "Mesh.h"
#include "Texture.h"

#include <libds.h>
#include <libapi.h>

Resources::Resource Resources::m_sMeshes[MAX_LOADED_MESHES];
int Resources::m_snMeshes = 0;
Resources::Resource Resources::m_sTextures[MAX_LOADED_TEXTURES];
int Resources::m_snTextures = 0;

Resources::Resource* Resources::m_spLoadingResource = NULL;
Resources::ResourceType Resources::m_sLoadingResourceType;
char Resources::m_sPathBuffer[MAX_PATH_SIZE];
int Resources::m_snLoadingResourceBytes;
u_long* Resources::m_spLoadBuffer = NULL;
int Resources::m_snLoadBufferWords = 0;

Mesh* Resources::GetLoadedMesh(const char* pName)
{
    return (Mesh*)GetResourceByName(m_sMeshes, m_snMeshes, pName);
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

    int length = String::Copy(m_sPathBuffer, "\\ASSET\\OXM\\");
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
    delete (Mesh*)RemoveResourceByName(m_sMeshes, m_snMeshes, pName);
}

Texture* Resources::GetLoadedTexture(const char* pName)
{
    return (Texture*)GetResourceByName(m_sTextures, m_snTextures, pName);
}

Texture* Resources::BlockingTextureLoad(const char* pName)
{
    Resources::BlockForResourceLoad();
    Resources::BeginTextureLoad(pName);
    return (Texture*)Resources::BlockForResourceLoad();
}

bool Resources::BeginTextureLoad(const char* pName)
{
    assert(!GetLoadedTexture(pName));

    if (m_spLoadingResource) return false;
    if (m_snTextures == MAX_LOADED_TEXTURES) return false;

    int length = String::Copy(m_sPathBuffer, "\\ASSET\\TEX\\");
    length += String::Copy(m_sPathBuffer + length, pName);
    length += String::Copy(m_sPathBuffer + length, ";1");
    assert(length < MAX_PATH_SIZE);

    m_spLoadingResource = &m_sTextures[m_snTextures++];
    m_spLoadingResource->pName = String::NewDup(pName);
    m_sLoadingResourceType = RT_TEXTURE;
    m_spLoadingResource->pResource = new Texture;
    BeginResourceLoad(m_sPathBuffer);

    return true;
}

void Resources::UnloadTexture(const char* pName)
{
    delete (Texture*)RemoveResourceByName(m_sTextures, m_snTextures, pName);
}

void* Resources::GetResourceByName(Resource* pResArray, int nRes, const char* pName)
{
    for (int i = 0; i < nRes; i++)
    {
        Resource& res = pResArray[i];
        if (String::Compare(res.pName, pName) == 0 && res.nRemainingSectors == 0)
            return res.pResource;
    }

    return NULL;
}

void* Resources::RemoveResourceByName(Resource* pResArray, int& nRes, const char* pName)
{
    for (int i = 0; i < nRes; i++)
    {
        Resource& res = pResArray[i];
        if (String::Compare(res.pName, pName) == 0)
        {
            if (&res == m_spLoadingResource)
            {
                DsReadBreak();
                m_spLoadingResource = NULL;
            }

            void* pResPtr = res.pResource;
            delete[] res.pName;
            Memory::Move(pResArray + i, pResArray + i + 1, (nRes - i - 1) * sizeof(Resource));
            nRes--;

            return pResPtr;
        }
    }

    return NULL;
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
            m_spLoadingResource = NULL;

            switch (m_sLoadingResourceType)
            {
                case RT_MESH:
                    ((Mesh*)pRes)->InitFromLoadedOxm(m_spLoadBuffer, m_snLoadingResourceBytes);
                    break;
                case RT_TEXTURE:
                    ((Texture*)pRes)->InitFromLoadedTim(m_spLoadBuffer, m_snLoadingResourceBytes);
                    break;
            }

            
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
