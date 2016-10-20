#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "Global.h"

#include <libgte.h>
#include <libgpu.h>

typedef struct _FrameBuffer
{
	unsigned long ot[OT_LENGTH]; // Ordering table
	DRAWENV drawEnv;
	DISPENV dispEnv;
} FrameBuffer;

void FrameBuffer_Construct(FrameBuffer* pSelf, int xOffset, int yOffset);

#endif