#include "Cube.h"

#define S 64
#define RED { 255, 0, 0 }
#define GREEN { 0, 255, 0 }
#define BLUE { 0, 0, 255 } 

MeshTriGour gCubeTris[] =
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
int gnCubeTris = sizeof(gCubeTris) / sizeof(MeshTriGour);

Mesh* Cube_AllocateMesh()
{
    Mesh* pMesh = malloc3(sizeof(Mesh));
    Mesh_Construct(pMesh);
	Mesh_AllocateBuffers(pMesh, (sizeof(MeshTriGour) * gnCubeTris) / sizeof(u_long),
		(sizeof(POLY_G3) * gnCubeTris) / sizeof(u_long),
		1);
        
    memcpy(pMesh->pModelTris, gCubeTris, sizeof(MeshTriGour) * gnCubeTris);
	pMesh->pAttrs[0].attrCode = MESHPT_TRI_GOUR;
	pMesh->pAttrs[0].nPrims = gnCubeTris;

	Mesh_InitPrimBufs(pMesh);
    return pMesh;
}
