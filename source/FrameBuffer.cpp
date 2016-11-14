#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() : ot(0)
{

}

void FrameBuffer::Initialize(int xOffset, int yOffset)
{
	SetDefDrawEnv(&drawEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);

	DISPENV* pDispEnv = &dispEnv;
	SetDefDispEnv(pDispEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);
	pDispEnv->screen.w = SCREEN_WIDTH;
	pDispEnv->screen.h = SCREEN_HEIGHT;
}
