#include "Global.h"

#include <libetc.h>
#include <stdio.h>

#include "MathUtil.h"
#include "DebugUtil.h"
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

const u_char gDebug = 1;

FrameBuffer gFrameBuffers[2];
FrameBuffer* gpCurFrameBuf = gFrameBuffers;
u_char gCurFrameBufIdx = 0;

static u_long gFrameBufOtEntries[2][OT_LENGTH];

enum VideoMode
{
	VIDEOMODE_NTSC, VIDEOMODE_PAL
};
static VideoMode gVideoMode;

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
		if (gVideoMode == VIDEOMODE_PAL && nativeFrameCounter % 5 == 0) Update();
		nativeFrameCounter = (nativeFrameCounter + 1) % (gVideoMode == VIDEOMODE_NTSC ? 60 : 50);
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
		gVideoMode = VIDEOMODE_PAL;
		SetVideoMode(1);
	}
	else
	{
		gVideoMode = VIDEOMODE_NTSC;
		SetVideoMode(0);
	}
	
	ResetGraph(0);
	Input::Initialize(0);
	InitGeom();
	SetGeomScreen(400);
	SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	SetDispMask(1);

	gFrameBuffers[0].Initialize(0, 0);
	gFrameBuffers[1].Initialize(0, SCREEN_HEIGHT);

	for (int i = 0; i < 2; i++)
	{
		gFrameBuffers[i].ot.pEntries = gFrameBufOtEntries[i];
		gFrameBuffers[i].ot.nEntries = OT_LENGTH;
		gFrameBuffers[i].ot.bStatic = true;
		gFrameBuffers[i].ot.Clear();
	}

	pCamera = new Camera;

	Checkerboard_Initialize();
	tpage = LoadTPage(gCheckerboardRgb, 2, 0, 384, 0, gCheckerboardWidth, gCheckerboardHeight);
	pChecker = new Mesh;
	pChecker->AllocateBuffers(sizeof(MeshTriGourTex) / 2, sizeof(POLY_GT3) / 2, 1);
	memcpy(pChecker->pModelTris, plane, sizeof(MeshTriGourTex) * 2);
	pChecker->pAttrs[0].attrCode = MESHPT_TRI_GOUR_TEX;
	pChecker->pAttrs[0].nPrims = 2;
	pChecker->pAttrs[0].tpageId = tpage;
	pChecker->InitPrimBufs();

	DrawSync(0);
	VSync(0);
	SwapBuffers();

	pCube = new Cube;
	pCube->pGCRender->worldMtx.t[2] = 500;
}

void Update(void)
{
	Input::Update();

	Input::ControllerType controllerType = Input::GetControllerType(0);
	if (controllerType != Input::CONTROLLER_NONE)
	{
		if (Input::ButtonDown(Input::BUTTON_DLEFT, 0)) pCamera->yaw -= 10;
		if (Input::ButtonDown(Input::BUTTON_DRIGHT, 0)) pCamera->yaw += 10;
		if (Input::ButtonDown(Input::BUTTON_DUP, 0)) pCamera->pitch += 10;
		if (Input::ButtonDown(Input::BUTTON_DDOWN, 0)) pCamera->pitch -= 10;
	}
	pCamera->ClampRotations();
}

void SetupDraw(void)
{
	gpCurFrameBuf->ot.Clear();
}

void BuildDrawCommands(void)
{
	MATRIX mtx;
	MATRIX* pCameraMtx = pCamera->GetCameraMatrix();

	GORenderData renderData;
	renderData.pCamera = pCamera;
	renderData.frameBufIdx = gCurFrameBufIdx;
	renderData.pFrameBuf = gpCurFrameBuf;

	pCube->Draw(&renderData);

	M_IdentityMatrix(&mtx);
	mtx.t[2] = 500;
	
	M_MulMatrixTrans(&mtx, pCameraMtx);

	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);
	pChecker->Draw(gCurFrameBufIdx, &gpCurFrameBuf->ot);
}

void IssueDraw(void)
{
	int prevBufIdx = gCurFrameBufIdx ^ 0x1;
	gFrameBuffers[prevBufIdx].ot.IssueToGpu();
}

void SwapBuffers(void)
{
	PutDispEnv(&gpCurFrameBuf->dispEnv);
	gCurFrameBufIdx ^= 0x1;
	gpCurFrameBuf = &gFrameBuffers[gCurFrameBufIdx];
	PutDrawEnv(&gpCurFrameBuf->drawEnv);
	ClearImage(&gpCurFrameBuf->drawEnv.clip, 25, 25, 25);
}

void* operator new(std::size_t n) throw(std::bad_alloc)
{
  	return malloc3(n);
}

void operator delete(void* p) throw()
{
  	free(p);
}