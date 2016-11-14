#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "Global.h"
#include "OrderingTable.h"

class FrameBuffer
{
	public:

	FrameBuffer();

	void Initialize(int xOffset, int yOffset);

	OrderingTable m_Ot;
	DRAWENV m_DrawEnv;
	DISPENV m_DispEnv;
};

#endif
