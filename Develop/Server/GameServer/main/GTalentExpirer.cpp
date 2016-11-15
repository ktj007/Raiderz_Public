#include "StdAfx.h"
#include "GTalentExpirer.h"
#include "GTalentInfo.h"

GTalentExpirer::GTalentExpirer(void)
: m_fExpiredTime(0.0f)
{
}

GTalentExpirer::~GTalentExpirer(void)
{
}

bool GTalentExpirer::IsExpired(float fElapsedTime)
{
	if (m_fExpiredTime == NO_EXPIRE_TALENT)
		return false;

	if (fElapsedTime >= m_fExpiredTime)	
	{
		return true;
	}

	return false;
}

void GTalentExpirer::SetExpiredTime(float fExpiredTime)
{
	m_fExpiredTime = fExpiredTime;
}