#pragma once

#include "SUnitTestUtil.h"
#include "LSystem.h"

class LUnitTestUtil: public SUnitTestUtil
{
public:
	class FakeSystem : public LSystem
	{
		uint32 nFakeTime;
	public:
		FakeSystem(): nFakeTime(0) {}
		
		virtual uint32		GetNowTime()			{ return nFakeTime; }
		virtual void		Sleep(float fSeconds)	{ nFakeTime += (int)(fSeconds*1000.0f); }
		void	SetNowTime(uint32 nTime)			{ nFakeTime = nTime;	}
	};

};
