#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#define MAX_LOADED_MESHES 16
#define MAX_LOADED_TEXTURES 64
#define MAX_PATH_SIZE 100

#include "Global.h"

class Mesh;
class Texture;

class Resources
{
    public:
        static void* Update();

        static Mesh* GetLoadedMesh(const char* pName);
        static Mesh* BlockingMeshLoad(const char* pName);
        static bool BeginMeshLoad(const char* pName);
        static void UnloadMesh(const char* pName);

        static Texture* GetLoadedTexture(const char* pName);
        static Texture* BlockingTextureLoad(const char* pName);
        static bool BeginTextureLoad(const char* pName);
        static void UnloadTexture(const char* pName);

        static void* BlockForResourceLoad();

    private:

        enum ResourceType { RT_MESH, RT_TEXTURE };

        struct Resource
        {
            char* pName;
            void* pResource;
            int nRemainingSectors;
        };

        static void* GetResourceByName(Resource* pResArray, int nRes, const char* pName);
        static void* RemoveResourceByName(Resource* pResArray, int& nRes, const char* pName);
        static void BeginResourceLoad(const char* pFilePath);

        static Resource m_sMeshes[MAX_LOADED_MESHES];
        static int m_snMeshes;
        static Resource m_sTextures[MAX_LOADED_TEXTURES];
        static int m_snTextures;

        static Resource* m_spLoadingResource;
        static ResourceType m_sLoadingResourceType;
        static int m_snLoadingResourceBytes;
        static char m_sPathBuffer[MAX_PATH_SIZE];
        static u_long* m_spLoadBuffer;
        static int m_snLoadBufferWords;
};

#endif
