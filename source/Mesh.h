#ifndef _MESH_H_
#define _MESH_H_

#include "Global.h"

#include <libgte.h>
#include <libgpu.h>

typedef struct _Mesh
{
	SVECTOR* pVerts;
	int nVerts;
	
	u_long* pPrimBufs[2];
	int nUsedPrimWords[2];
	
	// Actually initialized to length + 1, so it can be inserted in another table
	// while allowing primitives to use the expected length
	unsigned long* pOrderTbls[2]; 
	
	int orderTblLength;
} Mesh;

void Mesh_Construct(Mesh* pSelf);
void Mesh_AllocateBuffers(Mesh* pSelf, int nVerts, int numPrimBufWords, int orderTblLength);
void Mesh_PrepareDrawing(Mesh* pSelf, int frameBufIdx);
void Mesh_InsertOrderTbl(Mesh* pSelf, int frameBufIdx, u_long* pDestOtLoc);
void Mesh_Destruct(Mesh* pSelf);

#endif