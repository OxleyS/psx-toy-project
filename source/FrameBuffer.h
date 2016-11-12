#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "Global.h"
#include "OrderingTable.h"

struct FrameBuffer
{
	OrderingTable ot;
	DRAWENV drawEnv;
	DISPENV dispEnv;
};

void FrameBuffer_Construct(FrameBuffer* pSelf, int xOffset, int yOffset);

#endif
