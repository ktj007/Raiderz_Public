#pragma once

#include "ZSystem.h"

class FakeSystem : public ZSystem
{
public:
	uint32 nFakeTime;
public:
	FakeSystem(): nFakeTime(0) {}

	virtual uint32		GetNowTime()			{ return nFakeTime; }
	virtual void		Sleep(float fSeconds)	{ nFakeTime += (int)(fSeconds*1000.0f); }
};
