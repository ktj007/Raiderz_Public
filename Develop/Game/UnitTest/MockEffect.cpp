#include "stdafx.h"
#include "MockEffect.h"

MockEffect::MockEffect()
{
	m_bLoadingComplete = false;
}

MockEffect::~MockEffect()
{

}

bool MockEffect::Create( MUID& uid, const wstring& str )
{
	m_UID = uid;
	m_strEffectFileName = str;
	return true;
}

void MockEffect::Stop()
{
	m_bDelMe = true;
}

void MockEffect::UpdateXEffect( float fDelta )
{
	// 이펙트가 없어져야 하는 시간인가?
	m_fDurationTime -= fDelta;
	if(m_bUseDurationTime &&
		m_fDurationTime <= 0.0f)
	{
		m_bUseDurationTime = false;
		m_bDelMe = true;
	}
	else if(m_bUseDurationTime == false)
	{
		m_fDurationTime = 0.0f;
	}
}

bool MockEffect::IsEffectLive()
{
	return (!m_bDelMe);
}

bool MockEffect::IsEffectLoadingComplete()
{
	return m_bLoadingComplete;
}