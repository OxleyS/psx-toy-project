#include "Global.h"

#include <libetc.h>
#include <stdio.h>

#include "Math.h"
#include "Debug.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Input.h"
#include "Cube.h"
#include "Checkerboard.h"
#include "Camera.h"
#include "GameObject.h"
#include "GCRender.h"

u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long __stacksize = 0x00004000; // force 16 kilobytes of stack

const u_char g_Debug = 1;

FrameBuffer g_FrameBuffers[2];
FrameBuffer* g_pCurFrameBuf = g_FrameBuffers;
u_char g_CurFrameBufIdx = 0;

static u_long g_FrameBufOtEntries[2][OT_LENGTH];

enum VideoMode
{
	VIDEOMODE_NTSC, VIDEOMODE_PAL
};
static VideoMode g_VideoMode;

Mesh* pChecker;
Camera* pCamera;
Cube* pCube;

MeshTriGourTex plane[2] =
{
	{ 
		{ -150, 64, -150, 0 }, { 128, 128, 128 }, 0, 64, 0,
		{ -150, 64, 150, 0 }, { 128, 128, 128 }, 0, 0, 0,
		{ 150, 64, -150, 0 }, { 128, 128, 128 }, 64, 64, 0
	},
	{ 
		{ -150, 64, 150, 0 }, { 128, 128, 128 }, 0, 0, 0,
		{ 150, 64, 150, 0 }, { 128, 128, 128 }, 64, 0, 0,
		{ 150, 64, -150, 0 }, { 128, 128, 128 }, 64, 64, 0
	}
};

void Initialize(void);
void Update(void);
void SetupDraw(void);
void BuildDrawCommands(void);
void SwapBuffers(void);
void IssueDraw();

int main(void)
{
	int nativeFrameCounter = 0;

	Initialize();

	while (1)
	{
		Update();
		if (g_VideoMode == VIDEOMODE_PAL && nativeFrameCounter % 5 == 0) Update();
		nativeFrameCounter = (nativeFrameCounter + 1) % (g_VideoMode == VIDEOMODE_NTSC ? 60 : 50);
		SetupDraw();
		BuildDrawCommands();
		DrawSync(0); // Wait for draw commands to finish
		VSync(0); // Wait for vertical sync
		SwapBuffers(); // Swap the front and back buffers
		IssueDraw();
	}

	return 0;
}

void Initialize(void)
{
	u_short tpage;

	// 1 MB of heap space
	InitHeap3((unsigned long*)0x800F8000, 0x00100000);

	// This byte tells us whether we should use PAL (1) or NTSC (0)
	if (*(char*)0xbfc7ff52 == 'E')
	{
		g_VideoMode = VIDEOMODE_PAL;
		SetVideoMode(1);
	}
	else
	{
		g_VideoMode = VIDEOMODE_NTSC;
		SetVideoMode(0);
	}
	
	ResetGraph(0);
	Input::Initialize(0);
	InitGeom();
	SetGeomScreen(400);
	SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	SetDispMask(1);

	g_FrameBuffers[0].Initialize(0, 0);
	g_FrameBuffers[1].Initialize(0, SCREEN_HEIGHT);

	for (int i = 0; i < 2; i++)
	{
		g_FrameBuffers[i].m_Ot.m_pEntries = g_FrameBufOtEntries[i];
		g_FrameBuffers[i].m_Ot.m_nEntries = OT_LENGTH;
		g_FrameBuffers[i].m_Ot.m_bStatic = true;
		g_FrameBuffers[i].m_Ot.Clear();
	}

	pCamera = new Camera;

	Checkerboard_Initialize();
	tpage = LoadTPage(g_CheckerboardRgb, 2, 0, 384, 0, g_CheckerboardWidth, g_CheckerboardHeight);
	pChecker = new Mesh;
	pChecker->AllocateBuffers(sizeof(MeshTriGourTex) / 2, sizeof(POLY_GT3) / 2, 1);
	memcpy(pChecker->m_pModelTris, plane, sizeof(MeshTriGourTex) * 2);
	pChecker->m_pAttrs[0].attrCode = MESHPT_TRI_GOUR_TEX;
	pChecker->m_pAttrs[0].nPrims = 2;
	pChecker->m_pAttrs[0].tpageId = tpage;
	pChecker->InitPrimBufs();

	DrawSync(0);
	VSync(0);
	SwapBuffers();

	pCube = new Cube;
	pCube->m_pGCRender->m_WorldMtx.t[2] = 500;
}

void Update(void)
{
	Input::Update();

	Input::ControllerType controllerType = Input::GetControllerType(0);
	if (controllerType != Input::CONTROLLER_NONE)
	{
		if (Input::ButtonDown(Input::BUTTON_DLEFT, 0)) pCamera->m_Yaw -= 10;
		if (Input::ButtonDown(Input::BUTTON_DRIGHT, 0)) pCamera->m_Yaw += 10;
		if (Input::ButtonDown(Input::BUTTON_DUP, 0)) pCamera->m_Pitch += 10;
		if (Input::ButtonDown(Input::BUTTON_DDOWN, 0)) pCamera->m_Pitch -= 10;
	}
	pCamera->ClampRotations();
}

void SetupDraw(void)
{
	g_pCurFrameBuf->m_Ot.Clear();
}

void BuildDrawCommands(void)
{
	MATRIX mtx;
	MATRIX* pCameraMtx = pCamera->GetCameraMatrix();

	GORenderData renderData;
	renderData.pCamera = pCamera;
	renderData.frameBufIdx = g_CurFrameBufIdx;
	renderData.pFrameBuf = g_pCurFrameBuf;

	pCube->Draw(&renderData);

	Math::IdentityMatrix(&mtx);
	mtx.t[2] = 500;
	
	Math::MulMatrixTrans(&mtx, pCameraMtx);

	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);
	pChecker->Draw(g_CurFrameBufIdx, &g_pCurFrameBuf->m_Ot);
}

void IssueDraw(void)
{
	int prevBufIdx = g_CurFrameBufIdx ^ 0x1;
	g_FrameBuffers[prevBufIdx].m_Ot.IssueToGpu();
}

void SwapBuffers(void)
{
	PutDispEnv(&g_pCurFrameBuf->m_DispEnv);
	g_CurFrameBufIdx ^= 0x1;
	g_pCurFrameBuf = &g_FrameBuffers[g_CurFrameBufIdx];
	PutDrawEnv(&g_pCurFrameBuf->m_DrawEnv);
	ClearImage(&g_pCurFrameBuf->m_DrawEnv.clip, 25, 25, 25);
}

void* operator new(std::size_t n) throw(std::bad_alloc)
{
  	return malloc3(n);
}

void operator delete(void* p) throw()
{
  	free(p);
}