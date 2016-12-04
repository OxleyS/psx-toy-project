#include "Cube.h"
#include "GameObject.h"
#include "GCRender.h"
#include "Mesh.h"

#define S 64
#define RED { 255, 0, 0 }
#define GREEN { 0, 255, 0 }
#define BLUE { 0, 0, 255 } 

struct MeshTriGour
{
    MeshPoly::Color c0, c1, c2;
    MeshPoly::Position p0, p1, p2;
};

MeshTriGour g_CubeTris[] =
{
    // Top face
    { 
        RED, GREEN, BLUE,
        { -S, -S, -S, 0 },
	    { -S, -S, S, 0 },
	    { S, -S, -S, 0 }
    },
	{
        GREEN, RED, BLUE,
        { -S, -S, S, 0 },
	    { S, -S, S, 0 },
	    { S, -S, -S, 0 }
    },

    // Bottom face
    {  
        GREEN, RED, BLUE,
        { -S, S, S, 0 },
	    { -S, S, -S, 0 },
	    { S, S, -S, 0 }
    },
	{
        GREEN, BLUE, RED,
        { -S, S, S, 0 },
        { S, S, -S, 0 },
	    { S, S, S, 0 }
    },

    // Front face
    {
        RED, GREEN, BLUE,
        { -S, S, -S, 0 },
        { -S, -S, -S, 0 },
        { S, S, -S, 0 }
    },
    {
        GREEN, RED, BLUE,
        { -S, -S, -S, 0 },
        { S, -S, -S, 0 },
        { S, S, -S, 0 }
    },

    // Back face
    {
        GREEN, RED, BLUE,
        { -S, -S, S, 0 },
        { -S, S, S, 0 },
        { S, S, S, 0 }
    },
    {
        GREEN, BLUE, RED,
        { -S, -S, S, 0 },
        { S, S, S, 0 },
        { S, -S, S, 0 }
    },

    // Left face
    {
        GREEN, RED, BLUE,
        { -S, S, S, 0 },
        { -S, -S, S, 0 },
        { -S, -S, -S, 0 }
    },
    {
        BLUE, RED, GREEN,
        { -S, -S, -S, 0 },
        { -S, S, -S, 0 },
        { -S, S, S, 0 }
    },

    // Right face
    {
        GREEN, RED, BLUE,
        { S, S, -S, 0 },
        { S, -S, -S, 0 },
        { S, -S, S, 0 }
    },
    {
        BLUE, RED, GREEN,
        { S, -S, S, 0 },
        { S, S, S, 0 },
        { S, S, -S, 0 }
    },
};
int g_nCubeTris = sizeof(g_CubeTris) / sizeof(MeshTriGour);

Cube::Cube()
{
    m_pGCRender = new GCRender;

    Mesh* pMesh = new Mesh;
	pMesh->AllocateBuffers((sizeof(MeshTriGour) * g_nCubeTris) / sizeof(u_long),
		(sizeof(POLY_G3) * g_nCubeTris) / sizeof(u_long),
		1);
        
    Memory::Copy(pMesh->m_pMeshPolys, g_CubeTris, sizeof(MeshTriGour) * g_nCubeTris);
    pMesh->m_pAttrs[0].nPolys = g_nCubeTris;
	pMesh->m_pAttrs[0].polyType = MeshPoly::MESHPT_TRI_GOUR;
    pMesh->m_pAttrs[0].lightType = MeshPoly::MESHPL_NONE;
    pMesh->m_pAttrs[0].semitransparentCode = 0xFF;
    pMesh->m_pAttrs[0].flags = 0; 
	pMesh->m_pAttrs[0].clutId = 0;
    pMesh->m_pAttrs[0].tpageId = 0;

	pMesh->InitPrimBufs();
    m_pGCRender->m_pMesh = pMesh;
}
