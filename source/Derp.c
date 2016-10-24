#include "Global.h"

#include <libgte.h>
#include <libgpu.h>

#include "MathUtil.h"
#include "DebugUtil.h"
#include "FrameBuffer.h"
#include "Mesh.h"

u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long __stacksize = 0x00004000; // force 16 kilobytes of stack
static u_long g_NumNonHeapRamWords = 0;

const u_char g_Debug = 1;

FrameBuffer g_FrameBuffers[2];
FrameBuffer* g_pCurFrameBuf = g_FrameBuffers;
u_char g_CurFrameBufIdx = 0;

static u_long g_FrameBufOtEntries1[OT_LENGTH];
static u_long g_FrameBufOtEntries2[OT_LENGTH];

Mesh mesh;
SVECTOR meshVerts[6] =
{
	{ 0, 0, 0, 0 },
	{ 100, 50, 0, 0 },
	{ 100, -50, 0, 0 },
	{ 0, 0, -10, 0 },
	{ 100, 50, -10, 0 },
	{ 100, -50, -10, 0 },
};
CVECTOR meshColors[6] =
{
	{ 0, 0, 255, 0 },
	{ 255, 0, 0, 0 },
	{ 0, 255, 0, 0 },
	{ 255, 0, 255, 0 },
	{ 255, 255, 0, 0 },
	{ 0, 255, 255, 0 }
};

int xOffset = 0;
SVECTOR rotAmt = { 0, 0, 0, 0 };

void Initialize(void);
void Update(void);
void SetupDraw(void);
void BuildDrawCommands(void);
void SwapBuffers(void);
void IssueDraw();

int main(void)
{
	Initialize();

	while (1)
	{
		Update();
		SetupDraw();
		BuildDrawCommands();
		DrawSync(0); // Wait for draw commands to finish
		VSync(0); // Wait for vertical sync
		SwapBuffers(); // Swap the front and back buffers
		IssueDraw();
	}
}

void Initialize(void)
{
	int i;

	MeshTriGour polys[2];
	int baseIdx;

	// Initialize the heap with all the RAM that's not stack or otherwise reserved
	static u_long memoryEnd = 0x80200000;
	static u_long memoryStart = 0x8001D800;
	u_long stackEnd = memoryEnd - __stacksize;
	u_long ramStart = memoryStart + g_NumNonHeapRamWords * 4;
	InitHeap3((void*)ramStart, stackEnd - ramStart);

	// This byte tells us whether we should use PAL (1) or NTSC (0)
	if (*(char*)0xbfc7ff52 == 'E') SetVideoMode(1); else SetVideoMode(0);
	
	ResetGraph(0);
	InitGeom();
	SetGeomScreen(SCREEN_CENTER_X);
	SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	SetDispMask(1);

	FrameBuffer_Construct(g_FrameBuffers, 0, 0);
	OrderingTable_ConstructStatic(&g_FrameBuffers[0].ot, g_FrameBufOtEntries1, OT_LENGTH);
	FrameBuffer_Construct(g_FrameBuffers + 1, 0, SCREEN_HEIGHT);
	OrderingTable_ConstructStatic(&g_FrameBuffers[1].ot, g_FrameBufOtEntries1, OT_LENGTH);

	VSync(0);
	SwapBuffers();

	Mesh_Construct(&mesh);
	Mesh_AllocateBuffers(&mesh, (sizeof(MeshTriGour) * 2) / sizeof(u_long),
		(sizeof(POLY_G3) * 2) / sizeof(u_long),
		1);

	for (i = 0; i < 2; i++)
	{
		baseIdx = i * 3;
		polys[i].xyz0 = meshVerts[baseIdx];
		polys[i].xyz1 = meshVerts[baseIdx + 1];
		polys[i].xyz2 = meshVerts[baseIdx + 2];
		polys[i].rgb0 = meshColors[baseIdx];
		polys[i].rgb1 = meshColors[baseIdx + 1];
		polys[i].rgb2 = meshColors[baseIdx + 2];
	}

	memcpy(mesh.pModelTris, polys, sizeof(MeshTriGour) * 2);
	mesh.pAttrs[0].attrCode = MESHPT_TRI_GOUR;
	mesh.pAttrs[0].nPrims = 2;

	Mesh_InitPrimBufs(&mesh);
}

void Update(void)
{
	xOffset += 10;
	if (xOffset > 4096) xOffset = 0;
	rotAmt.vy = xOffset;
}

void SetupDraw(void)
{
	OrderingTable_Clear(&g_pCurFrameBuf->ot);
}

void BuildDrawCommands(void)
{
	long p, flag;
	long otz;
	MATRIX mtx, rotMtx;
	POLY_G3* pTri;
	SVECTOR* pVerts;

	Mesh_PrepareDrawing(&mesh, g_CurFrameBufIdx);

	MathUtil_IdentityMatrix(&mtx);
	MathUtil_ZeroMatrixTrans(&rotMtx);

	RotMatrix_gte(&rotAmt, &rotMtx);
	rotMtx.t[2] = 300;

	SetRotMatrix(&rotMtx);
	SetTransMatrix(&rotMtx);

	Mesh_Draw(&mesh, g_CurFrameBufIdx, &g_pCurFrameBuf->ot);
}

void IssueDraw(void)
{
	OrderingTable_IssueToGpu(&g_pCurFrameBuf->ot);
}

void SwapBuffers(void)
{
	PutDispEnv(&g_pCurFrameBuf->dispEnv);
	g_CurFrameBufIdx ^= 0x1;
	g_pCurFrameBuf = &g_FrameBuffers[g_CurFrameBufIdx];
	PutDrawEnv(&g_pCurFrameBuf->drawEnv);
	ClearImage(&g_pCurFrameBuf->drawEnv.clip, 25, 25, 25);
}
