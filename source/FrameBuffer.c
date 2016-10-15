#include "FrameBuffer.h"

void FrameBuffer_Construct(FrameBuffer* pFrameBuf, int xOffset, int yOffset)
{
	DISPENV* pDispEnv;
	
	ClearOTagR(pFrameBuf->ot, OT_LENGTH);
	
	SetDefDrawEnv(&pFrameBuf->drawEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	pDispEnv = &pFrameBuf->dispEnv;
	SetDefDispEnv(pDispEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);
	pDispEnv->screen.w = SCREEN_WIDTH;
	pDispEnv->screen.h = SCREEN_HEIGHT;
}
