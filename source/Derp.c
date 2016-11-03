#include "Global.h"

#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

#include "MathUtil.h"
#include "DebugUtil.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Input.h"
#include "Cube.h"
#include "Camera.h"

u_long __ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long __stacksize = 0x00004000; // force 16 kilobytes of stack

const u_char gDebug = 1;

FrameBuffer gFrameBuffers[2];
FrameBuffer* gpCurFrameBuf = gFrameBuffers;
u_char gCurFrameBufIdx = 0;

static u_long gFrameBufOtEntries1[OT_LENGTH];
static u_long gFrameBufOtEntries2[OT_LENGTH];

typedef enum VideoMode
{
	VIDEOMODE_NTSC, VIDEOMODE_PAL
} VideoMode;
static VideoMode gVideoMode;

Mesh mesh;
Camera camera;

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
	// 1 MB of heap space
	InitHeap3((void*)0x800F8000, 0x00100000);

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
	Input_Initialize(0);
	InitGeom();
	SetGeomScreen(SCREEN_CENTER_X);
	SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
	SetDispMask(1);

	FrameBuffer_Construct(gFrameBuffers, 0, 0);
	OT_ConstructStatic(&gFrameBuffers[0].ot, gFrameBufOtEntries1, OT_LENGTH);
	FrameBuffer_Construct(gFrameBuffers + 1, 0, SCREEN_HEIGHT);
	OT_ConstructStatic(&gFrameBuffers[1].ot, gFrameBufOtEntries2, OT_LENGTH);

	OT_Clear(&gFrameBuffers[0].ot);
	OT_Clear(&gFrameBuffers[1].ot);

	Camera_Construct(&camera);

	VSync(0);
	SwapBuffers();

	mesh = *Cube_AllocateMesh();
}

void Update(void)
{
	ControllerType controllerType;

	Input_Update();

	controllerType = Input_GetControllerType(0);
	if (controllerType != CONTROLLER_NONE)
	{
		if (Input_ButtonDown(BUTTON_DLEFT, 0)) camera.yaw -= 10;
		if (Input_ButtonDown(BUTTON_DRIGHT, 0)) camera.yaw += 10;
		if (Input_ButtonDown(BUTTON_DUP, 0)) camera.pitch += 10;
		if (Input_ButtonDown(BUTTON_DDOWN, 0)) camera.pitch -= 10;
	}
	Camera_ClampRotations(&camera);
}

void SetupDraw(void)
{
	OT_Clear(&gpCurFrameBuf->ot);
}

void BuildDrawCommands(void)
{
	MATRIX mtx;
	MATRIX* pCameraMtx = Camera_GetMatrix(&camera);

	Mesh_PrepareDrawing(&mesh, gCurFrameBufIdx);

	M_IdentityMatrix(&mtx);
	mtx.t[2] = 300;
	
	M_MulMatrixTrans(&mtx, pCameraMtx);

	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);

	Mesh_Draw(&mesh, gCurFrameBufIdx, &gpCurFrameBuf->ot);
}

void IssueDraw(void)
{
	int prevBufIdx = gCurFrameBufIdx ^ 0x1;
	OT_IssueToGpu(&gFrameBuffers[prevBufIdx].ot);
}

void SwapBuffers(void)
{
	PutDispEnv(&gpCurFrameBuf->dispEnv);
	gCurFrameBufIdx ^= 0x1;
	gpCurFrameBuf = &gFrameBuffers[gCurFrameBufIdx];
	PutDrawEnv(&gpCurFrameBuf->drawEnv);
	ClearImage(&gpCurFrameBuf->drawEnv.clip, 25, 25, 25);
}
