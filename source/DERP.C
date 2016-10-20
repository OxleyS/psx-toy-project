#include "Global.h"

#include <libgte.h>
#include <libgpu.h>

#include "MathUtil.h"
#include "DebugUtil.h"
#include "FrameBuffer.h"
#include "Mesh.h"

u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long __stacksize = 0x00004000; // force 16 kilobytes of stack
u_long g_NumNonHeapRamWords = 0;

const u_char g_Debug = 1;

FrameBuffer g_FrameBuffers[2];
FrameBuffer* g_pCurFrameBuf = g_FrameBuffers;
u_char g_CurFrameBufIdx = 0;

Mesh mesh;
SVECTOR meshVerts[] =
{
	{ 0, 0, 0, 0 },
	{ 100, 50, 0, 0 },
	{ 100, -50, 0, 0 },
	{ 0, 0, -10, 0 },
	{ 100, 50, -10, 0 },
	{ 100, -50, -10, 0 },
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
	FrameBuffer_Construct(g_FrameBuffers + 1, 0, SCREEN_HEIGHT);
	
	VSync(0);
	SwapBuffers();
	
	Mesh_Construct(&mesh);
	Mesh_AllocateBuffers(&mesh, 6, sizeof(POLY_G3) * 6, 3);
	memcpy(mesh.pVerts, meshVerts, sizeof(SVECTOR) * 6);
}

void Update(void)
{
	xOffset += 10;
	if (xOffset > 4096) xOffset = 0;
	rotAmt.vy = xOffset;
}

void SetupDraw(void)
{	
	ClearOTagR(g_pCurFrameBuf->ot, OT_LENGTH);	
}

void BuildDrawCommands(void)
{
	long p, flag;
	long otz;
	MATRIX mtx, rotMtx;
	POLY_G3* pTri;
	
	Mesh_PrepareDrawing(&mesh, g_CurFrameBufIdx);

	pTri = (POLY_G3*)mesh.pPrimBufs[g_CurFrameBufIdx];
	SetPolyG3(pTri);
	
	MathUtil_IdentityMatrix(&mtx);
	MathUtil_ZeroMatrixTrans(&rotMtx);
	
	RotMatrix_gte(&rotAmt, &rotMtx);
	rotMtx.t[2] = 300;
	
	SetRotMatrix(&rotMtx);
	SetTransMatrix(&rotMtx);
	
	// TODO: So this returns a value we can use in an OT?
	otz = RotTransPers3(&mesh.pVerts[0], &mesh.pVerts[1], &mesh.pVerts[2],
		(long*)&pTri->x0, (long*)&pTri->x1, (long*)&pTri->x2, &p, &flag);
	pTri->r0 = 0; pTri->g0 = 0; pTri->b0 = 255;
	pTri->r1 = 255; pTri->g1 = 0; pTri->b1 = 0;
	pTri->r2 = 0; pTri->g2 = 255; pTri->b2 = 0;
	AddPrim(mesh.pOrderTbls[g_CurFrameBufIdx] + 1, pTri);
	//AddPrim(g_pCurFrameBuf->ot + 3, pTri);
		
	pTri++;
	SetPolyG3(pTri);
	otz = RotTransPers3(&mesh.pVerts[3], &mesh.pVerts[4], &mesh.pVerts[5],
		(long*)&pTri->x0, (long*)&pTri->x1, (long*)&pTri->x2, &p, &flag);
	pTri->r0 = 255; pTri->g0 = 0; pTri->b0 = 255;
	pTri->r1 = 255; pTri->g1 = 255; pTri->b1 = 0;
	pTri->r2 = 0; pTri->g2 = 255; pTri->b2 = 255;
	AddPrim(mesh.pOrderTbls[g_CurFrameBufIdx] + 2, pTri);
	//AddPrim(g_pCurFrameBuf->ot + 1, pTri);

	
	/*if (otz >= 0 && otz < OT_LENGTH)*/ 
	Mesh_InsertOrderTbl(&mesh, g_CurFrameBufIdx, g_pCurFrameBuf->ot + 1);
	DumpOTag(g_pCurFrameBuf->ot + (OT_LENGTH - 1));
}

void IssueDraw(void)
{
	DrawOTag(g_pCurFrameBuf->ot + (OT_LENGTH - 1));
}

void SwapBuffers(void)
{
	PutDispEnv(&g_pCurFrameBuf->dispEnv);
	g_CurFrameBufIdx ^= 0x1;
	g_pCurFrameBuf = &g_FrameBuffers[g_CurFrameBufIdx];
	PutDrawEnv(&g_pCurFrameBuf->drawEnv);
	ClearImage(&g_pCurFrameBuf->drawEnv.clip, 25, 25, 25);
}