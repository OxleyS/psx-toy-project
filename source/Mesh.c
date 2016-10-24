#include "Mesh.h"
#include "OrderingTable.h"

union InTriType
{
	MeshTriGour* mtg;
};

union OutPolyType
{
	POLY_G3* g3;
};

void Mesh_Construct(Mesh* pSelf)
{
	memset(pSelf, 0, sizeof(Mesh));
}

void Mesh_AllocateBuffers(Mesh* pSelf, int nModelTriWords, int nPrimWords, int nAttrs)
{
	pSelf->pModelTris = malloc3(sizeof(u_long) * nModelTriWords);
	pSelf->nModelTriWords = nModelTriWords;
	pSelf->pAttrs = malloc3(sizeof(MeshAttr) * nAttrs);
	pSelf->nAttrs = nAttrs;
	pSelf->pPrims[0] = malloc3(sizeof(u_long) * nPrimWords);
	pSelf->pPrims[1] = malloc3(sizeof(u_long) * nPrimWords);
	pSelf->maxPrimWords = nPrimWords;
}

void Mesh_InitPrimBufs(Mesh* pSelf)
{
	int i, j;
	MeshAttr* pAttr;
	u_long* pInTri;
	u_long* pOutPrim;
	int frameBufIdx;

	union InTriType tt;
	union OutPolyType pt;

	for (frameBufIdx = 0; frameBufIdx < 2; frameBufIdx++)
	{
		pOutPrim = pSelf->pPrims[frameBufIdx];
		pInTri = pSelf->pModelTris;
		
		for (i = 0; i < pSelf->nAttrs; i++)
		{
			pAttr = &pSelf->pAttrs[i];
			for (j = 0; j < pAttr->nPrims; j++)
			{
				switch (pAttr->attrCode)
				{
					case MESHPT_TRI_GOUR:
						pt.g3 = (POLY_G3*)pOutPrim;
						tt.mtg = (MeshTriGour*)pInTri;
						SetPolyG3(pt.g3);
						pt.g3->r0 = tt.mtg->rgb0.r;
						pt.g3->g0 = tt.mtg->rgb0.g;
						pt.g3->b0 = tt.mtg->rgb0.b;
						pt.g3->r1 = tt.mtg->rgb1.r;
						pt.g3->g1 = tt.mtg->rgb1.g;
						pt.g3->b1 = tt.mtg->rgb1.b;
						pt.g3->r2 = tt.mtg->rgb2.r;
						pt.g3->g2 = tt.mtg->rgb2.g;
						pt.g3->b2 = tt.mtg->rgb2.b;

						pInTri = (u_long*)(tt.mtg + 1);
						pOutPrim = (u_long*)(pt.g3 + 1);

						break;
				}			
			}
		} // End for attrs
	} // End for framebufs
}

void Mesh_PrepareDrawing(Mesh* pSelf, int frameBufIdx)
{
	pSelf->nUsedPrimWords[frameBufIdx] = 0;
}

void Mesh_Draw(Mesh* pSelf, int frameBufIdx, OrderingTable* pOrderTbl)
{
	int i, j;
	MeshAttr* pAttr;
	u_long* pInTri = pSelf->pModelTris;
	u_long* pOutPrim = pSelf->pPrims[frameBufIdx];
	long otz, p, flag;

	union InTriType tt;
	union OutPolyType pt;

	for (i = 0; i < pSelf->nAttrs; i++)
	{
		pAttr = &pSelf->pAttrs[i];
		for (j = 0; j < pAttr->nPrims; j++)
		{
			switch (pAttr->attrCode)
			{
				case MESHPT_TRI_GOUR:
					pt.g3 = (POLY_G3*)pOutPrim;
					tt.mtg = (MeshTriGour*)pInTri;
					otz = RotTransPers3(&tt.mtg->xyz0, &tt.mtg->xyz1, &tt.mtg->xyz2,
						(long*)&pt.g3->x0, (long*)&pt.g3->x1, (long*)&pt.g3->x2,
						&p, &flag);
					OrderingTable_AddPrim(pOrderTbl, pOutPrim, otz);
					pInTri = (u_long*)(tt.mtg + 1);
					pOutPrim = (u_long*)(pt.g3 + 1);
					break;
			}
			
			pSelf->nUsedPrimWords[frameBufIdx]++;
		}
	}
}

void Mesh_Destruct(Mesh* pSelf)
{
	if (pSelf->pModelTris) free(pSelf->pModelTris);
	if (pSelf->pAttrs) free(pSelf->pAttrs);
	if (pSelf->pPrims[0]) free(pSelf->pPrims[0]);
	if (pSelf->pPrims[1]) free(pSelf->pPrims[1]);
}
