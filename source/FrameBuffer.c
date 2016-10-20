#include "FrameBuffer.h"

void FrameBuffer_Construct(FrameBuffer* pSelf, int xOffset, int yOffset)
{
	DISPENV* pDispEnv;
	
	ClearOTagR(pSelf->ot, OT_LENGTH);
	
	SetDefDrawEnv(&pSelf->drawEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	pDispEnv = &pSelf->dispEnv;
	SetDefDispEnv(pDispEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);
	pDispEnv->screen.w = SCREEN_WIDTH;
	pDispEnv->screen.h = SCREEN_HEIGHT;
}
