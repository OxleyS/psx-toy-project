#ifndef _MESH_H_
#define _MESH_H_

#include "Global.h"
#include "MeshPoly.h"

struct MeshAttr
{
	enum Flag
	{
		MESHATTR_DOUBLESIDED = 1
	};

	u_short nPolys;

	u_char polyType; // MeshPoly::Type
	u_char lightType; // MeshPoly::Lighting
	u_char semitransparentCode; // 0xFF == opaque
	u_char flags;
	
	u_short clutId;
	u_short tpageId;
};

class Mesh
{
	public:

	Mesh();
	~Mesh();

	void InitFromLoadedOxm(u_long* pOxmBuf, int nBytes);

	void InitPrimBufs();

	void AllocateBuffers(int nMeshPolyWords, int nPrimWords, int nAttrs);
	void Draw(int frameBufIdx, OrderingTable* pOrderTbl);
	void FreeBuffers();	

	// Static at load time
	u_long* m_pMeshPolys;
	int m_nMeshPolyWords;
	MeshAttr* m_pAttrs;
	int m_nAttrs;

	// Re-used per frame
	u_long* m_pPrims[2];
};

#endif
