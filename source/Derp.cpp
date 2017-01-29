#include "Global.h"

#include <libetc.h>
#include <libds.h>
#include <stdio.h>
#include <libapi.h>

#include "Math.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Input.h"
#include "Cube.h"
#include "TestObject.h"
#include "Checkerboard.h"
#include "Camera.h"
#include "GameObject.h"
#include "GCRender.h"
#include "World.h"
#include "Player.h"

u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long __stacksize = 0x00007FF0; // force (almost) 32 kilobytes of stack

const u_char g_Debug = 1;

FrameBuffer g_FrameBuffers[2];
FrameBuffer* g_pCurFrameBuf = g_FrameBuffers;

static u_long g_FrameBufOtEntries[2][OT_LENGTH];

enum VideoMode
{
	VIDEOMODE_NTSC, VIDEOMODE_PAL
};
static VideoMode g_VideoMode;

static Camera* g_pCamera;
static World g_World;

struct MeshTriGourTex
{
	MeshPoly::Color c0, c1, c2;
	MeshPoly::Position p0, p1, p2;
	MeshPoly::UVCoords uv0, uv1, uv2;
	u_char pad0, pad1;
};

MeshTriGourTex plane[2] =
{
	{
		{ 128, 128, 128 }, { 128, 128, 128 }, { 128, 128, 128 },
		{ -150, 64, -150, 0 }, { -150, 64, 150, 0 }, { 150, 64, -150, 0 },
		{ 0, 64 }, { 0, 0 }, { 64, 64 }, 0, 0
	},
	{ 
		{ 128, 128, 128 }, { 128, 128, 128 }, { 128, 128, 128 },
		{ -150, 64, 150, 0 }, { 150, 64, 150, 0 }, { 150, 64, -150, 0 },
		{ 0, 0 }, { 64, 0 }, { 64, 64} , 0, 0
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

	DsInit();
	
	ResetGraph(0);
	Input::Initialize(0);
	InitGeom();
	SetGeomScreen(400);
	SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	SetDispMask(1);

	g_FrameBuffers[0].Initialize(0, 0, 0);
	g_FrameBuffers[1].Initialize(1, 0, 256);

	for (int i = 0; i < 2; i++)
	{
		g_FrameBuffers[i].m_Ot.m_pEntries = g_FrameBufOtEntries[i];
		g_FrameBuffers[i].m_Ot.m_nEntries = OT_LENGTH;
		g_FrameBuffers[i].m_Ot.m_bStatic = true;
		g_FrameBuffers[i].m_Ot.Clear();
	}

	DrawSync(0);
	VSync(0);
	SwapBuffers();

	bool bAdded;

	g_pCamera = new Camera;
	bAdded = g_World.AddObject(g_pCamera);
	g_pCamera->m_Position = Vec3Long(0, -300, -5500);
	g_pCamera->m_bDirty = true;
	assert(bAdded);

	Player* pPlayer = new Player;
	RotMatrixY(500, &pPlayer->m_pGCRender->m_WorldMtx);
	pPlayer->m_pGCRender->m_WorldMtx.SetTrans(Vec3Long(0, 0, 500));
	
	bAdded = g_World.AddObject(pPlayer);
	assert(bAdded);
	pPlayer->SetFollowCamera(g_pCamera);

	GameObject* pLevelObj = new TestObject("TESTLVL.OXM");
	RotMatrixY(100, &pLevelObj->m_pGCRender->m_WorldMtx);
	pLevelObj->m_pGCRender->m_WorldMtx.SetTrans(Vec3Long(0, 50, 500));
	bAdded = g_World.AddObject(pLevelObj);
	assert(bAdded);
}

void Update(void)
{
	Input::Update();

	Input::ControllerType controllerType = Input::GetControllerType(0);
	if (controllerType != Input::CONTROLLER_NONE)
	{
		if (Input::ButtonDown(Input::BUTTON_SQ, 0)) g_pCamera->m_Yaw -= 10;
		if (Input::ButtonDown(Input::BUTTON_O, 0)) g_pCamera->m_Yaw += 10;
		if (Input::ButtonDown(Input::BUTTON_TRI, 0)) g_pCamera->m_Pitch += 10;
		if (Input::ButtonDown(Input::BUTTON_X, 0)) g_pCamera->m_Pitch -= 10;
		g_pCamera->m_bDirty = true;

		if (Input::ButtonDown(Input::BUTTON_SELECT, 0)) g_pCamera->SetDebugMode(!g_pCamera->IsDebugMode());
	}
	g_pCamera->ClampRotations();

	g_World.Update();
}

void SetupDraw(void)
{
	g_pCurFrameBuf->m_Ot.Clear();
}

void BuildDrawCommands(void)
{
	g_World.Draw(g_pCamera, g_pCurFrameBuf);
}

void IssueDraw(void)
{
	int prevBufIdx = g_pCurFrameBuf->m_Index ^ 0x1;
	g_FrameBuffers[prevBufIdx].m_Ot.IssueToGpu();
}

void SwapBuffers(void)
{
	PutDispEnv(&g_pCurFrameBuf->m_DispEnv);
	int newFrameIdx = g_pCurFrameBuf->m_Index ^ 0x1;
	g_pCurFrameBuf = &g_FrameBuffers[newFrameIdx];
	PutDrawEnv(&g_pCurFrameBuf->m_DrawEnv);
	ClearImage(&g_pCurFrameBuf->m_DrawEnv.clip, 25, 25, 25);
}

void* operator new(std::size_t n)
{
	void* pMem = malloc3(n);
	if (!pMem)
	{
		printf("FATAL: OUT OF MEMORY (%u)\n", n);
		SystemError('m', 69);
	}
  	return pMem;
}

void operator delete(void* p) throw()
{
  	free(p);
}
