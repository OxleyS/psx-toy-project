#include "Mesh.h"
#include "Math.h"
#include "OrderingTable.h"
#include "MeshPrimMacros.h"
#include "Texture.h"
#include "Resources.h"

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

void DrawPolyF3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyF4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyG3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyG4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyFT3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyFT4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyGT3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
void DrawPolyGT4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);

typedef void (*DrawPrimFunc)(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut);
DrawPrimFunc g_DrawPrimFuncTable[] =
{
	DrawPolyF3, DrawPolyF4,
	DrawPolyG3, DrawPolyG4,
	DrawPolyFT3, DrawPolyFT4,
	DrawPolyGT3, DrawPolyGT4
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

void Mesh::InitFromLoadedOxm(u_long* pOxmBuf, int nBytes)
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
	char** ppTexturePaths = new char*[nTextures];
	for (int i = 0; i < nTextures; i++)
	{
		int nChars = *pByteBuf++;

		char* pTexPath = new char[nChars + 1];
		String::CopyCount(pTexPath, (char*)pByteBuf, nChars);
		pTexPath[nChars] = '\0';
		printf("TEXTURE %d: %s\n", i, pTexPath);
		ppTexturePaths[i] = pTexPath;

		pByteBuf += nChars;
	}

	// Start at next 32-bit boundary
	pByteBuf = (u_char*)(((u_long)pByteBuf + 3) & ~3);
	pOxmBuf = (u_long*)pByteBuf;

	// Attribute table size
	m_nAttrs = (int)*pOxmBuf++;
	m_pAttrs = new MeshAttr[m_nAttrs];
	printf("ATTR TABLE (%d entries):\n", m_nAttrs);
	u_char* pTexIndices = new u_char[m_nAttrs];

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
		
		pTexIndices[i] = *pByteBuf++;
		printf("\t\tTex table idx %hhu\n", pTexIndices[i]);

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
		totalPrimBytes += primStride * attr.nPolys;
	}

	printf("TOTAL POLY BYTES: %d\n", totalPolyBytes);
	m_pMeshPolys = new u_long[totalPolyBytes / sizeof(u_long)];
	Memory::Copy(m_pMeshPolys, pOxmBuf, totalPolyBytes);
	pOxmBuf += totalPolyBytes / sizeof(u_long);

	printf("TOTAL PRIM BYTES: %d\n", totalPrimBytes);
	m_pPrims[0] = new u_long[totalPrimBytes / sizeof(u_long)];
	m_pPrims[1] = new u_long[totalPrimBytes / sizeof(u_long)];

	// Now that we don't need the load buffer anymore, we can resolve textures
	// TODO: Come up with something better. This does not
	// support asynchronous loading
	for (int i = 0; i < m_nAttrs; i++)
	{
		MeshAttr& attr = m_pAttrs[i];
		u_char texTableIdx = pTexIndices[i];
		if (texTableIdx != 0xFF)
		{
			Texture* pTex = Resources::GetLoadedTexture(ppTexturePaths[texTableIdx]);
			if (!pTex) pTex = Resources::BlockingTextureLoad(ppTexturePaths[texTableIdx]);

			attr.tpageId = pTex->m_TpageId;
			attr.clutId = pTex->m_ClutId;
		}
		else
		{
			attr.tpageId = 0xFFFF;
			attr.clutId = 0xFFFF;
		}
	}

	delete[] pTexIndices;
	for (int i = 0; i < nTextures; i++) delete[] ppTexturePaths[i];
	delete[] ppTexturePaths;

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

void DrawPolyF3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_F3* pOutPrim = (POLY_F3*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_TRI(1);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyF4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_F4* pOutPrim = (POLY_F4*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_QUAD(1);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyG3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_G3* pOutPrim = (POLY_G3*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_TRI(3);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyG4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_G4* pOutPrim = (POLY_G4*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_QUAD(4);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyFT3(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_FT3* pOutPrim = (POLY_FT3*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_TRI(1);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyFT4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_FT4* pOutPrim = (POLY_FT4*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_QUAD(1);

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
		PRIM_TRANSFORM_TRI(3);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}

void DrawPolyGT4(const MeshAttr& attr, OrderingTable* pOrderTbl, u_long*& pIn, u_long*& pOut)
{
	POLY_GT4* pOutPrim = (POLY_GT4*)pOut;
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long);
    long otz, p, flag, clipVal;

	for (int i = 0; i < attr.nPolys; i++) 
	{
		PRIM_TRANSFORM_QUAD(4);

		pIn += inputStrideWords; 
		pOutPrim++;
	}

	pOut = (u_long*)pOutPrim;
}
