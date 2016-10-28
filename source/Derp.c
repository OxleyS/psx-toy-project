#include "Global.h"

#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

#include "MathUtil.h"
#include "DebugUtil.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Input.h"

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
	int i;

	MeshTriGour polys[2];
	int baseIdx;

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
	OrderingTable_ConstructStatic(&gFrameBuffers[0].ot, gFrameBufOtEntries1, OT_LENGTH);
	FrameBuffer_Construct(gFrameBuffers + 1, 0, SCREEN_HEIGHT);
	OrderingTable_ConstructStatic(&gFrameBuffers[1].ot, gFrameBufOtEntries2, OT_LENGTH);

	OrderingTable_Clear(&gFrameBuffers[0].ot);
	OrderingTable_Clear(&gFrameBuffers[1].ot);

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
	ControllerType controllerType;

	Input_Update();

	controllerType = Input_GetControllerType(0);
	if (controllerType != CONTROLLER_NONE)
	{
		if (Input_ButtonDown(BUTTON_DUP, 0)) xOffset += 10;
	}
	
	if (xOffset > 4096) xOffset = 0;
	rotAmt.vy = xOffset;
}

void SetupDraw(void)
{
	OrderingTable_Clear(&gpCurFrameBuf->ot);
}

void BuildDrawCommands(void)
{
	MATRIX mtx, rotMtx;

	Mesh_PrepareDrawing(&mesh, gCurFrameBufIdx);

	MathUtil_IdentityMatrix(&mtx);
	mtx.t[2] = 300;

	MathUtil_ZeroMatrixTrans(&rotMtx);
	RotMatrix_gte(&rotAmt, &rotMtx);

	MulMatrix(&mtx, &rotMtx);

	SetRotMatrix(&mtx);
	SetTransMatrix(&mtx);

	Mesh_Draw(&mesh, gCurFrameBufIdx, &gpCurFrameBuf->ot);
}

void IssueDraw(void)
{
	int prevBufIdx = gCurFrameBufIdx ^ 0x1;
	OrderingTable_IssueToGpu(&gFrameBuffers[prevBufIdx].ot);
}

void SwapBuffers(void)
{
	PutDispEnv(&gpCurFrameBuf->dispEnv);
	gCurFrameBufIdx ^= 0x1;
	gpCurFrameBuf = &gFrameBuffers[gCurFrameBufIdx];
	PutDrawEnv(&gpCurFrameBuf->drawEnv);
	ClearImage(&gpCurFrameBuf->drawEnv.clip, 25, 25, 25);
}
