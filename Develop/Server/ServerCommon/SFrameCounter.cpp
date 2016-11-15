#include "stdafx.h"
#include "SFrameCounter.h"
#include "SSystem.h"
#include <MMSystem.h>


SFrameCounter::SFrameCounter(SSystem* pSystem) : m_pSystem(pSystem), m_nTime(0), m_nFrameCounter(0), m_nCurrFrame(0)
{
	m_nLastTime = GetNowTime();
}

int SFrameCounter::Update()
{
	++m_nFrameCounter;
	MTimeValue nNowTime = GetNowTime();

	m_nTime += (nNowTime - m_nLastTime);
	m_nLastTime = nNowTime;

	if (m_nTime >= 1000)
	{
		m_nCurrFrame = m_nFrameCounter;
		m_nFrameCounter = 0;
		m_nTime = 0;
	}

	return m_nCurrFrame.GetValue();
}

uint32 SFrameCounter::GetNowTime()
{
	if (m_pSystem)
	{
		return m_pSystem->GetNowTime();
	}

	return timeGetTime();
}

void SFrameCounter::Clear()
{
	m_nTime = 0;
	m_nFrameCounter = 0;
	m_nCurrFrame = 0;
	m_nLastTime = 0;
}