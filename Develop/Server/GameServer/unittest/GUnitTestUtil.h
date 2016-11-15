#pragma once

#include "SUnitTestUtil.h"
#include "GSystem.h"

class GUnitTestUtil: public SUnitTestUtil
{
public:
	class FakeSystem : public GSystem
	{
		uint32 nFakeTime;
	public:
		FakeSystem(): nFakeTime(0) {}

		virtual uint32		GetNowTime()			{ return nFakeTime; }
		virtual void		Sleep(float fSeconds)	{ nFakeTime += (int)(fSeconds*1000.0f); }
	};
};
