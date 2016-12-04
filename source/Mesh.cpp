#include "Mesh.h"
#include "OrderingTable.h"

union InTriType
{
	MeshTriGour* mtg;
	MeshTriGourTex* mtgt;
};

union OutPolyType
{
	POLY_G3* g3;
	POLY_GT3* gt3;
};

Mesh::Mesh()
{
	Memory::Set(this, 0, sizeof(Mesh));
}

Mesh::~Mesh()
{
	if (m_pModelTris) free(m_pModelTris);
	if (m_pAttrs) free(m_pAttrs);
	if (m_pPrims[0]) free(m_pPrims[0]);
	if (m_pPrims[1]) free(m_pPrims[1]);
}

void Mesh::AllocateBuffers(int nModelTriWords, int nPrimWords, int nAttrs)
{
	m_pModelTris = (u_long*)malloc3(sizeof(u_long) * nModelTriWords);
	m_nModelTriWords = nModelTriWords;
	m_pAttrs = (MeshAttr*)malloc3(sizeof(MeshAttr) * nAttrs);
	m_nAttrs = nAttrs;
	m_pPrims[0] = (u_long*)malloc3(sizeof(u_long) * nPrimWords);
	m_pPrims[1] = (u_long*)malloc3(sizeof(u_long) * nPrimWords);
	m_MaxPrimWords = nPrimWords;
}

void Mesh::InitPrimBufs()
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
		pOutPrim = m_pPrims[frameBufIdx];
		pInTri = m_pModelTris;
		
		for (i = 0; i < m_nAttrs; i++)
		{
			pAttr = &m_pAttrs[i];
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
					case MESHPT_TRI_GOUR_TEX:
						pt.gt3 = (POLY_GT3*)pOutPrim;
						tt.mtgt = (MeshTriGourTex*)pInTri;
						SetPolyGT3(pt.gt3);
						pt.gt3->r0 = tt.mtgt->rgb0.r;
						pt.gt3->g0 = tt.mtgt->rgb0.g;
						pt.gt3->b0 = tt.mtgt->rgb0.b;
						pt.gt3->u0 = tt.mtgt->u0;
						pt.gt3->v0 = tt.mtgt->v0;
						pt.gt3->r1 = tt.mtgt->rgb1.r;
						pt.gt3->g1 = tt.mtgt->rgb1.g;
						pt.gt3->b1 = tt.mtgt->rgb1.b;
						pt.gt3->u1 = tt.mtgt->u1;
						pt.gt3->v1 = tt.mtgt->v1;
						pt.gt3->r2 = tt.mtgt->rgb2.r;
						pt.gt3->g2 = tt.mtgt->rgb2.g;
						pt.gt3->b2 = tt.mtgt->rgb2.b;
						pt.gt3->u2 = tt.mtgt->u2;
						pt.gt3->v2 = tt.mtgt->v2;

						pInTri = (u_long*)(tt.mtgt + 1);
						pOutPrim = (u_long*)(pt.gt3 + 1);

						break;
				} // End poly type switch
			} // End for prims in attr
		} // End for attrs
	} // End for framebufs
}

void Mesh::Draw(int frameBufIdx, OrderingTable* pOrderTbl)
{
	int i, j;
	MeshAttr* pAttr;
	u_long* pInTri = m_pModelTris;
	u_long* pOutPrim = m_pPrims[frameBufIdx];
	long otz, p, flag, clipVal;

	union InTriType tt;
	union OutPolyType pt;

	for (i = 0; i < m_nAttrs; i++)
	{
		pAttr = &m_pAttrs[i];
		for (j = 0; j < pAttr->nPrims; j++)
		{
			switch (pAttr->attrCode)
			{
				case MESHPT_TRI_GOUR:
					pt.g3 = (POLY_G3*)pOutPrim;
					tt.mtg = (MeshTriGour*)pInTri;				
					clipVal = RotAverageNclip3(&tt.mtg->xyz0, &tt.mtg->xyz1, &tt.mtg->xyz2,
						(long*)&pt.g3->x0, (long*)&pt.g3->x1, (long*)&pt.g3->x2,
						&p, &otz, &flag);
					if (clipVal >= 0) pOrderTbl->AddPrim(pOutPrim, otz);
					pInTri = (u_long*)(tt.mtg + 1);
					pOutPrim = (u_long*)(pt.g3 + 1);
					break;
				case MESHPT_TRI_GOUR_TEX:
					pt.gt3 = (POLY_GT3*)pOutPrim;
					tt.mtgt = (MeshTriGourTex*)pInTri;
					clipVal = RotAverageNclip3(&tt.mtgt->xyz0, &tt.mtgt->xyz1, &tt.mtgt->xyz2,
						(long*)&pt.gt3->x0, (long*)&pt.gt3->x1, (long*)&pt.gt3->x2,
						&p, &otz, &flag);
					if (clipVal >= 0)
					{
						pt.gt3->clut = pAttr->clutId;
						pt.gt3->tpage = pAttr->tpageId;
						pOrderTbl->AddPrim(pOutPrim, otz);
					}
					pInTri = (u_long*)(tt.mtgt + 1);
					pOutPrim = (u_long*)(pt.gt3 + 1);
					break;
			} // End attr switch
		} // End for every prim in attr
	} // End for every attr
}
