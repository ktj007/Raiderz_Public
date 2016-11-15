#include "stdafx.h"
#include "XEffectBase.h"

XEffectBase::XEffectBase()
: m_pEffect(NULL)
{
	m_fDurationTime			= 0.0f;
	m_fEndVisibilityTime	= 0.0f;

	m_bUseDurationTime		= false;
	m_bDelMe				= false;
	m_bUseVisibility		= false;
	m_bCancelEnabled		= false;

	m_matLocal.MakeIdentity();
}

XEffectBase::~XEffectBase()
{
	if(m_pEffect && m_pEffect->GetParent())
		m_pEffect->RemoveFromParent();
	SAFE_DELETE(m_pEffect);
}

void XEffectBase::Stop()
{
	if(m_bUseVisibility &&
		m_fEndVisibilityTime > 0.0f)
	{
		m_Fader.SetFaderState(EFS_END);
		m_Fader.Start(m_pEffect->GetVisibility(), 0.0f, m_fEndVisibilityTime);
	}
	else
	{
		StopXEffect();
	}
}

void XEffectBase::SetDurationTime( float fDurationTime )
{
	// 지속시간이 0초보다 커야 사용하는 걸로 한다.
	if(fDurationTime > 0.0f)
		m_bUseDurationTime = true;

	m_fDurationTime = fDurationTime;
}

bool XEffectBase::IsEffectLiveTime()
{
	return !m_bDelMe;
}

void XEffectBase::SetBoneName( tstring& strName )
{
	m_strBoneName = strName;
}

void XEffectBase::SetUseVisibility( float fStartTime, float fEndTime )
{
	if(fStartTime > 0.0f ||
		fEndTime > 0.0f)
	{
		m_bUseVisibility		= true;
	}

	if(fStartTime > 0.0f)
	{
		m_Fader.SetFaderState(EFS_START);
		m_pEffect->SetVisibility(0.0f);
		m_Fader.Start(0.0f, 1.0f, fStartTime);
	}

	m_fEndVisibilityTime	= fEndTime;
}

void XEffectBase::UpdateDurationTime( float fDelta )
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

void XEffectBase::UpdateEffectFader( float fDelta )
{
	m_Fader.Update(fDelta);

	if(m_Fader.IsDoing())
	{
		m_pEffect->SetVisibility(m_Fader.GetCurrAlpha());
	}

	if(m_bUseVisibility &&
		m_Fader.GetFaderState() == EFS_END &&
		m_Fader.IsDoing() == false)
	{
		StopXEffect();
	}
}