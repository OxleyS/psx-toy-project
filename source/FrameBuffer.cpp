#include "FrameBuffer.h"

FrameBuffer::FrameBuffer() : m_Ot(0)
{
}

void FrameBuffer::Initialize(int index, int xOffset, int yOffset)
{
	m_Index = index;

	SetDefDrawEnv(&m_DrawEnv, xOffset, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);

	DISPENV& dispEnv = m_DispEnv;
	SetDefDispEnv(&dispEnv, -1, yOffset, SCREEN_WIDTH, SCREEN_HEIGHT);
	dispEnv.screen.w = SCREEN_WIDTH;
	dispEnv.screen.h = SCREEN_HEIGHT;
}
