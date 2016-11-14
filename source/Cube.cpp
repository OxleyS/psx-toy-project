#include "Cube.h"
#include "GameObject.h"
#include "GCRender.h"
#include "Mesh.h"

#define S 64
#define RED { 255, 0, 0 }
#define GREEN { 0, 255, 0 }
#define BLUE { 0, 0, 255 } 

MeshTriGour g_CubeTris[] =
{
    // Top face
    {  
        { -S, -S, -S, 0 }, RED,
	    { -S, -S, S, 0 }, GREEN,
	    { S, -S, -S, 0 }, BLUE
    },
	{
        { -S, -S, S, 0 }, GREEN,
	    { S, -S, S, 0 }, RED,
	    { S, -S, -S, 0 }, BLUE
    },

    // Bottom face
    {  
        { -S, S, S, 0 }, GREEN,
	    { -S, S, -S, 0 }, RED,
	    { S, S, -S, 0 }, BLUE
    },
	{
        { -S, S, S, 0 }, GREEN,
        { S, S, -S, 0 }, BLUE,
	    { S, S, S, 0 }, RED
    },

    // Front face
    {
        { -S, S, -S, 0 }, RED,
        { -S, -S, -S, 0 }, GREEN,
        { S, S, -S, 0 }, BLUE
    },
    {
        { -S, -S, -S, 0 }, GREEN,
        { S, -S, -S, 0 }, RED,
        { S, S, -S, 0 }, BLUE
    },

    // Back face
    {
        { -S, -S, S, 0 }, GREEN,
        { -S, S, S, 0 }, RED,
        { S, S, S, 0 }, BLUE
    },
    {
        { -S, -S, S, 0 }, GREEN,
        { S, S, S, 0 }, BLUE,
        { S, -S, S, 0 }, RED
    },

    // Left face
    {
        { -S, S, S, 0 }, GREEN,
        { -S, -S, S, 0 }, RED,
        { -S, -S, -S, 0 }, BLUE
    },
    {
        { -S, -S, -S, 0 }, BLUE,
        { -S, S, -S, 0 }, RED,
        { -S, S, S, 0 }, GREEN
    },

    // Right face
    {
        { S, S, -S, 0 }, GREEN,
        { S, -S, -S, 0 }, RED,
        { S, -S, S, 0 }, BLUE
    },
    {
        { S, -S, S, 0 }, BLUE,
        { S, S, S, 0 }, RED,
        { S, S, -S, 0 }, GREEN
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
        
    memcpy(pMesh->m_pModelTris, g_CubeTris, sizeof(MeshTriGour) * g_nCubeTris);
	pMesh->m_pAttrs[0].attrCode = MESHPT_TRI_GOUR;
	pMesh->m_pAttrs[0].nPrims = g_nCubeTris;

	pMesh->InitPrimBufs();
    m_pGCRender->m_pMesh = pMesh;
}
