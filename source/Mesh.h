#ifndef _MESH_H_
#define _MESH_H_

#include "Global.h"

typedef enum MeshPrimType
{
	MESHPT_TRI_GOUR,
	MESHPT_TRI_GOUR_TEX
} MeshPrimType;

struct MeshAttr
{
	u_char attrCode; // MeshPrimType
	u_char nPrims;

	u_short clutId;
	u_short tpageId;
};

struct MeshTriGour
{
	SVECTOR xyz0;
	CVECTOR rgb0;
	SVECTOR xyz1;
	CVECTOR rgb1;
	SVECTOR xyz2;
	CVECTOR rgb2;
};

struct MeshTriGourTex
{
	SVECTOR xyz0;
	CVECTOR rgb0;
	u_char u0, v0;
	u_short pad0;
	SVECTOR xyz1;
	CVECTOR rgb1;
	u_char u1, v1;
	u_short pad1;
	SVECTOR xyz2;
	CVECTOR rgb2;
	u_char u2, v2;
	u_short pad2;
};

struct Mesh
{
	// Static at load time
	u_long* pModelTris;
	int nModelTriWords;
	MeshAttr* pAttrs;
	int nAttrs;

	// Re-used per frame
	u_long* pPrims[2];
	int maxPrimWords;
};

void Mesh_Construct(Mesh* pSelf);
void Mesh_AllocateBuffers(Mesh* pSelf, int nModelTriWords, int nPrimWords, int nAttrs);
void Mesh_InitPrimBufs(Mesh* pSelf);
void Mesh_Draw(Mesh* pSelf, int frameBufIdx, OrderingTable* pOrderTbl);
void Mesh_Destruct(Mesh* pSelf);

#endif
