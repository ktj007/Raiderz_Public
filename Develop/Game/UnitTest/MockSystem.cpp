#include "stdafx.h"
#include "MockSystem.h"

unsigned int MockSystem::GetNowTime()
{
	if (m_nExpecedNowTime != UINT_MAX)
		return m_nExpecedNowTime;

	return m_Timer.GetNowTime();
}