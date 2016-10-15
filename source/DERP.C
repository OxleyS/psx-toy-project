#include "Global.h"

#include <libgte.h>
#include <libgpu.h>

#include "MathUtil.h"
#include "DebugUtil.h"
#include "FrameBuffer.h"

u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long __stacksize = 0x00004000; // force 16 kilobytes of stack

const u_char g_Debug = 1;

FrameBuffer g_FrameBuffers[2];
FrameBuffer* g_pCurFrameBuf = g_FrameBuffers;
u_char g_CurFrameBufIdx = 0;

POLY_G3 orig_tri, tri;
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
		// TODO: Move me to after SwapBuffers() once I have separate primitive buffers per frame buffer
		IssueDraw();
		DrawSync(0); // Wait for draw commands to finish
		VSync(0); // Wait for vertical sync
		SwapBuffers(); // Swap the front and back buffers
	}
}

void Initialize(void)
{	
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
	
	SetPolyG3(&tri);
	tri.r0 = 0; tri.g0 = 0; tri.b0 = 255;
	tri.r1 = 255; tri.g1 = 0; tri.b1 = 0;
	tri.r2 = 0; tri.g2 = 255; tri.b2 = 0;
	tri.x0 = 20; tri.x1 = 200; tri.x2 = 20;
	tri.y0 = 20; tri.y1 = 100; tri.y2 = 200;
	memcpy(&orig_tri, &tri, sizeof(POLY_G3));	
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
	SVECTOR v0 = { 0, 0, 0, 0 };
	SVECTOR v1 = { 100, 50, 0, 0 };
	SVECTOR v2 = { 100, -50, 0, 0 };
	long p, flag;
	MATRIX mtx, rotMtx;
	
	MathUtil_IdentityMatrix(&mtx);
	MathUtil_ZeroMatrixTrans(&rotMtx);
	SetPolyG3(&tri);
	
	RotMatrix_gte(&rotAmt, &rotMtx);
	rotMtx.t[2] = 300;
	DebugUtil_PrintMatrix(&rotMtx, NULL);
	
	SetRotMatrix(&rotMtx);
	SetTransMatrix(&rotMtx);
		
	RotTransPers(&v0, (long*)&tri.x0, &p, &flag);
	RotTransPers(&v1, (long*)&tri.x1, &p, &flag);
	RotTransPers(&v2, (long*)&tri.x2, &p, &flag);
	
	tri.r0 = 0; tri.g0 = 0; tri.b0 = 255;
	tri.r1 = 255; tri.g1 = 0; tri.b1 = 0;
	tri.r2 = 0; tri.g2 = 255; tri.b2 = 0;
	
	AddPrim(g_pCurFrameBuf->ot + 1, &tri);	
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