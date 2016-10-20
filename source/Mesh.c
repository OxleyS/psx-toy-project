#include "Mesh.h"

void Mesh_Construct(Mesh* pSelf)
{
	memset(pSelf, 0, sizeof(Mesh));
}

void Mesh_AllocateBuffers(Mesh* pSelf, int nVerts, int numPrimBufWords, int orderTblLength)
{
	pSelf->pVerts = malloc3(sizeof(SVECTOR) * nVerts);
	pSelf->pPrimBufs[0] = malloc3(sizeof(u_long) * numPrimBufWords);
	pSelf->pPrimBufs[1] = malloc3(sizeof(u_long) * numPrimBufWords);
	pSelf->pOrderTbls[0] = malloc3(sizeof(u_long) * orderTblLength);
	pSelf->pOrderTbls[1] = malloc3(sizeof(u_long) * orderTblLength);
	
	pSelf->nVerts = nVerts;
	pSelf->orderTblLength = orderTblLength;
}

void Mesh_PrepareDrawing(Mesh* pSelf, int frameBufIdx)
{
	ClearOTagR(pSelf->pOrderTbls[frameBufIdx], pSelf->orderTblLength);
	pSelf->nUsedPrimWords[frameBufIdx] = 0;
}

void Mesh_InsertOrderTbl(Mesh* pSelf, int frameBufIdx, u_long* pDestOtLoc)
{
	AddPrims(pDestOtLoc, pSelf->pOrderTbls[frameBufIdx] + (pSelf->orderTblLength - 1), pSelf->pOrderTbls[frameBufIdx]);
}

void Mesh_Destruct(Mesh* pSelf)
{
	if (pSelf->pVerts) free(pSelf->pVerts);
	if (pSelf->pPrimBufs[0]) free(pSelf->pPrimBufs[0]);
	if (pSelf->pPrimBufs[1]) free(pSelf->pPrimBufs[1]);
	if (pSelf->pOrderTbls[0]) free(pSelf->pOrderTbls[0]);
	if (pSelf->pOrderTbls[1]) free(pSelf->pOrderTbls[1]);
}
