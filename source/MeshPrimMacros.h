#ifndef _MESHPRIMMACROS_H_
#define _MESHPRIMMACROS_H_

#define DEF_INIT_PRIM(suffix) \
void Init##suffix##Prims(const MeshAttr& attr, u_long*& pIn, u_long*& pOut) \
{ \
	POLY_##suffix* pOutPrim = (POLY_##suffix*)pOut; \
	int inputStrideWords = StrideFromAttribute(attr) / sizeof(u_long); \
    u_long deref; (void)deref; \
    u_char* pByte; (void)pByte; \
	for (int i = 0; i < attr.nPolys; i++) \
	{ \
		SetPoly##suffix(pOutPrim); \
        u_long* pInPoly = pIn;

#define END_DEF_INIT_PRIM \
		pIn += inputStrideWords; \
        pOutPrim++; \
	} \
    pOut = (u_long*)pOutPrim; \
}

#define INIT_PRIM_COLOR(idx) deref = *(pInPoly + idx); \
pOutPrim->r##idx = deref & 0xFF; \
pOutPrim->g##idx = (deref >> 8) & 0xFF; \
pOutPrim->b##idx = (deref >> 16) & 0xFF; \

#define INIT_PRIM_UV(idx) pByte = (u_char*)pInPoly; \
pOutPrim->u##idx = pByte[(idx * 2)]; \
pOutPrim->v##idx = pByte[(idx * 2) + 1];

#define INIT_PRIM_TEX pOutPrim->clut = attr.clutId; \
pOutPrim->tpage = attr.tpageId;

#define PRIM_TRANSFORM_TRI(posWordOffset) \
SVECTOR* pPositions = (SVECTOR*)(pIn + posWordOffset); \
if (attr.flags & MeshAttr::MESHATTR_DOUBLESIDED) \
{ \
    otz = RotAverage3(&pPositions[0], &pPositions[1], &pPositions[2], \
        (long*)&pOutPrim->x0, (long*)&pOutPrim->x1, (long*)&pOutPrim->x2, \
        &p, &flag); \
    pOrderTbl->AddPrim(pOutPrim, otz); \
} \
else \
{ \
    clipVal = RotAverageNclip3(&pPositions[0], &pPositions[1], &pPositions[2], \
        (long*)&pOutPrim->x0, (long*)&pOutPrim->x1, (long*)&pOutPrim->x2, \
        &p, &otz, &flag); \
    if (clipVal >= 0) pOrderTbl->AddPrim(pOutPrim, otz); \
}

#define PRIM_TRANSFORM_QUAD(posWordOffset) \
SVECTOR* pPositions = (SVECTOR*)(pIn + posWordOffset); \
if (attr.flags & MeshAttr::MESHATTR_DOUBLESIDED) \
{ \
    otz = RotAverage4(&pPositions[0], &pPositions[1], &pPositions[2], &pPositions[3], \
        (long*)&pOutPrim->x0, (long*)&pOutPrim->x1, (long*)&pOutPrim->x2, (long*)&pOutPrim->x3, \
        &p, &flag); \
    pOrderTbl->AddPrim(pOutPrim, otz); \
} \
else \
{ \
    clipVal = RotAverageNclip4(&pPositions[0], &pPositions[1], &pPositions[2], &pPositions[3], \
        (long*)&pOutPrim->x0, (long*)&pOutPrim->x1, (long*)&pOutPrim->x2, (long*)&pOutPrim->x3, \
        &p, &otz, &flag); \
    if (clipVal >= 0) pOrderTbl->AddPrim(pOutPrim, otz); \
}

#endif