#include "Mesh.h"
#include "OrderingTable.h"
#include "MeshPrimMacros.h"

int g_BasePolySizeTable[] =
{
	28, 36, 36, 48, 36, 44, 44, 56
};
int g_PrimSizeTable[] =
{
	sizeof(POLY_F3), sizeof(POLY_F4),
	sizeof(POLY_G3), sizeof(POLY_G4),
	sizeof(POLY_FT3), sizeof(POLY_FT4),
	sizeof(POLY_GT3), sizeof(POLY_GT4)
};

inline bool IsQuadType(MeshPoly::Type type)
{
	// Odd-numbered types
	return type & 0x1; 
}

int StrideFromAttribute(const MeshAttr& attr)
{
	int base = g_BasePolySizeTable[attr.polyType];
	switch (attr.lightType)
	{
		case MeshPoly::MESHPL_FLAT:
			return base + 8;
		case MeshPoly::MESHPL_SMOOTH:
			return base + IsQuadType((MeshPoly::Type)attr.polyType) ? 32 : 24;
		default:
			return base;
	}
}

DEF_INIT_PRIM(F3)
	INIT_PRIM_COLOR(0)
END_DEF_INIT_PRIM

DEF_INIT_PRIM(F4)
	INIT_PRIM_COLOR(0)
END_DEF_INIT_PRIM

DEF_INIT_PRIM(G3)
	INIT_PRIM_COLOR(0)
	INIT_PRIM_COLOR(1)
	INIT_PRIM_COLOR(2)
END_DEF_INIT_PRIM

DEF_INIT_PRIM(G4)
	INIT_PRIM_COLOR(0)
	INIT_PRIM_COLOR(1)
	INIT_PRIM_COLOR(2)
	INIT_PRIM_COLOR(3)
END_DEF_INIT_PRIM

DEF_INIT_PRIM(FT3)
	INIT_PRIM_COLOR(0)
	pInPoly += 7;
	INIT_PRIM_UV(0)
	INIT_PRIM_UV(1)
	INIT_PRIM_UV(2)
	INIT_PRIM_TEX
END_DEF_INIT_PRIM

DEF_INIT_PRIM(FT4)
	INIT_PRIM_COLOR(0)
	pInPoly += 9;
	INIT_PRIM_UV(0)
	INIT_PRIM_UV(1)
	INIT_PRIM_UV(2)
	INIT_PRIM_UV(3)
	INIT_PRIM_TEX
END_DEF_INIT_PRIM

DEF_INIT_PRIM(GT3)
	INIT_PRIM_COLOR(0)
	INIT_PRIM_COLOR(1)
	INIT_PRIM_COLOR(2)
	pInPoly += 9;
	printf("Copying UV from offset %d, value %lu\n", (pInPoly - pIn), *pInPoly);
	INIT_PRIM_UV(0)
	INIT_PRIM_UV(1)
	INIT_PRIM_UV(2)
	INIT_PRIM_TEX
END_DEF_INIT_PRIM

DEF_INIT_PRIM(GT4)
	INIT_PRIM_COLOR(0)
	INIT_PRIM_COLOR(1)
	INIT_PRIM_COLOR(2)
	INIT_PRIM_COLOR(3)
	pInPoly += 12;
	INIT_PRIM_UV(0)
	INIT_PRIM_UV(1)
	INIT_PRIM_UV(2)
	INIT_PRIM_UV(3)
	INIT_PRIM_TEX
END_DEF_INIT_PRIM

typedef void (*InitPrimFunc)(const MeshAttr& attr, u_long*& pIn, u_long*& pOut);
InitPrimFunc g_InitPrimFuncTable[] =
{
	InitF3Prims, InitF4Prims,
	InitG3Prims, InitG4Prims,
	InitFT3Prims, InitFT4Prims,
	InitGT3Prims, InitGT4Prims
};

void DrawPolyG3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyGT3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);

typedef void (*DrawPrimFunc)(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
DrawPrimFunc g_DrawPrimFuncTable[] =
{
	NULL, NULL,//DrawPolyF3, DrawPolyF4,
	DrawPolyG3, NULL,//DrawPolyG4,
	NULL, NULL,//DrawPolyFT3, DrawPolyFT4,
	DrawPolyGT3, NULL//DrawPolyGT4
};

Mesh::Mesh()
{
	Memory::Set(this, 0, sizeof(Mesh));
}

Mesh::~Mesh()
{
	FreeBuffers();
}

void Mesh::FreeBuffers()
{
	if (m_pMeshPolys) delete[] m_pMeshPolys;
	if (m_pAttrs) delete[] m_pAttrs;
	if (m_pPrims[0]) delete[] m_pPrims[0];
	if (m_pPrims[1]) delete[] m_pPrims[1];
}

void Mesh::InitFromLoadedOxm(u_long* pOxmBuf)
{
	u_char* pByteBuf;

	printf("PARSING OXM BUFFER\n");

	// Version
	printf("VERSION: %lu\n", *pOxmBuf++);

	// Scale factor
	printf("SCALE: %lu\n", *pOxmBuf++);

	// Texture table
	pByteBuf = (u_char*)pOxmBuf;
	int nTextures = *pByteBuf++;
	for (int i = 0; i < nTextures; i++)
	{
		int nChars = *pByteBuf++;
		printf("TEXTURE %d: %.*s\n", i, nChars, pByteBuf);
		pByteBuf += nChars;
	} 

	// Start at next 32-bit boundary
	pByteBuf = (u_char*)(((u_long)pByteBuf + 3) & ~3);
	pOxmBuf = (u_long*)pByteBuf;

	// Attribute table size
	m_nAttrs = (int)*pOxmBuf++;
	m_pAttrs = new MeshAttr[m_nAttrs];
	printf("ATTR TABLE (%d entries):\n", m_nAttrs);

	// Attribute table
	for (int i = 0; i < m_nAttrs; i++)
	{
		printf("\tATTR %d:\n", i);
		MeshAttr& attr = m_pAttrs[i];
		attr.nPolys = *pOxmBuf++;

		printf("\t\t%hu Polys\n", attr.nPolys);

		pByteBuf = (u_char*)pOxmBuf;
		attr.polyType = *pByteBuf++;
		attr.lightType = *pByteBuf++;
		attr.semitransparentCode = *pByteBuf++;

		printf("\t\tPoly code %hhu\n", attr.polyType);
		printf("\t\tLight code %hhu\n", attr.lightType);
		printf("\t\tST code %hhu\n", attr.semitransparentCode);
		
		u_char texTableIdx = *pByteBuf++;
		printf("\t\tTex table idx %hhu\n", texTableIdx);
		// TODO: Resolve the associated CLUT and TPAGE ID

		attr.flags = *pByteBuf++;
		printf("\t\tFlags %hhu\n", attr.flags);
		pOxmBuf = (u_long*)(pByteBuf + 3); // To boundary
	}

	// Polygon buffer
	int totalPolyBytes = 0;
	int totalPrimBytes = 0;
	printf("POLY DATA:\n");
	for (int i = 0; i < m_nAttrs; i++)
	{
		MeshAttr& attr = m_pAttrs[i];
		int polyStride = StrideFromAttribute(attr);
		int primStride = g_PrimSizeTable[attr.polyType];

		printf("\tATTR %d: %d bytes per poly, %d bytes per prim\n", i, polyStride, primStride);
		totalPolyBytes += polyStride * attr.nPolys;
		totalPrimBytes += primStride;
	}

	printf("TOTAL POLY BYTES: %d\n", totalPolyBytes);
	m_pMeshPolys = new u_long[totalPolyBytes / sizeof(u_long)];
	Memory::Copy(m_pMeshPolys, pOxmBuf, totalPolyBytes);

	printf("TOTAL PRIM BYTES: %d\n", totalPrimBytes);
	m_pPrims[0] = new u_long[totalPrimBytes / sizeof(u_long)];
	m_pPrims[1] = new u_long[totalPrimBytes / sizeof(u_long)];

	printf("LOAD COMPLETE. INITIALIZING\n");
	InitPrimBufs();
}

void Mesh::AllocateBuffers(int nMeshPolyWords, int nPrimWords, int nAttrs)
{
	m_pMeshPolys = (u_long*)malloc3(sizeof(u_long) * nMeshPolyWords);
	m_nMeshPolyWords = nMeshPolyWords;
	m_pAttrs = (MeshAttr*)malloc3(sizeof(MeshAttr) * nAttrs);
	m_nAttrs = nAttrs;
	m_pPrims[0] = (u_long*)malloc3(sizeof(u_long) * nPrimWords);
	m_pPrims[1] = (u_long*)malloc3(sizeof(u_long) * nPrimWords);
}

void Mesh::InitPrimBufs()
{
	for (int frameBufIdx = 0; frameBufIdx < 2; frameBufIdx++)
	{
		u_long* pInPoly = m_pMeshPolys;
		u_long* pOutPrim = m_pPrims[frameBufIdx];
		for (int i = 0; i < m_nAttrs; i++)
		{
			MeshAttr& attr = m_pAttrs[i];
			g_InitPrimFuncTable[attr.polyType](attr, pInPoly, pOutPrim);
		}
	}
}

void Mesh::Draw(int frameBufIdx, OrderingTable* pOrderTbl)
{
	u_long* pInPoly = m_pMeshPolys;
	u_long* pOutPrim = m_pPrims[frameBufIdx];

	for (int i = 0; i < m_nAttrs; i++)
	{
		MeshAttr& attr = m_pAttrs[i];
		g_DrawPrimFuncTable[attr.polyType](attr, pOrderTbl, pInPoly, pOutPrim);
	}
}

void DrawPolyG3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_G3* pOutPrim = (POLY_G3*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		SVECTOR* pPositions = (SVECTOR*)(pIn + 3); // Skip the colors 
		clipVal = RotAverageNclip3(&pPositions[0], &pPositions[1], &pPositions[2],
			(long*)&pOutPrim->x0, (long*)&pOutPrim->x1, (long*)&pOutPrim->x2,
			&p, &otz, &flag);
		if (clipVal >= 0) pOrderTbl->AddPrim(pOutPrim, otz);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyGT3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_GT3* pOutPrim = (POLY_GT3*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		//printf("0: idx %d, U %hhu, V %hhu\n", i, pOutPrim->u0, pOutPrim->v0);
		//printf("1: idx %d, U %hhu, V %hhu\n", i, pOutPrim->u1, pOutPrim->v1);
		//printf("2: idx %d, U %hhu, V %hhu\n", i, pOutPrim->u2, pOutPrim->v2);
		//printf("idx %d, deref %lu, red %hhu, green %hhu, blue %hhu\n", idx, deref, pPrimOut->r##idx, pPrimOut->g##idx, pPrimOut->b##idx);
		SVECTOR* pPositions = (SVECTOR*)(pIn + 3); // Skip the colors 
		clipVal = RotAverageNclip3(&pPositions[0], &pPositions[1], &pPositions[2],
			(long*)&pOutPrim->x0, (long*)&pOutPrim->x1, (long*)&pOutPrim->x2,
			&p, &otz, &flag);
		if (clipVal >= 0) pOrderTbl->AddPrim(pOutPrim, otz);
		//pt.gt3->clut = pAttr->clutId;
						//pt.gt3->tpage = pAttr->tpageId;

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}
