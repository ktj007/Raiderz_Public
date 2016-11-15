#include "stdafx.h"
#include "XGlowEffect.h"
#include "XCharacter.h"

XGlowEffect::XGlowEffect()
: XEffectBase()
, m_fStartTime(0.0f)
, m_fStartDurationTime(0.0f)
, m_fEndTime(0.0f)
, m_fEndDurationTime(0.0f)
, m_eState(ILL_READY)
, m_bStop(false)
, m_bLoop(false)
, m_fMin(1.0f)
, m_fMax(1.0f)
{

}

XGlowEffect::~XGlowEffect()
{

}

bool XGlowEffect::Create( MUID& uid, float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax )
{
	m_UID = uid;

	m_fStartTime			= fStartTime;
	m_fStartDurationTime	= fStartDurationTime;
	m_fEndTime				= fEndTime;
	m_fEndDurationTime		= fEndDurationTime;
	m_fMin					= fMin;
	m_fMax					= fMax;

	m_eState = ILL_START;
	m_Fader.Start(m_fMin, m_fMax, m_fStartTime);

	return true;
}

void XGlowEffect::UpdateXEffect( float fDelta )
{
	UpdateDurationTime(fDelta);

	if(m_bDelMe)
		return;

	m_Fader.Update(fDelta);
	SetSelfIlluminationUpdate(m_Fader.GetCurrAlpha());

	if(m_Fader.IsDoing() == false)
	{
		switch (m_eState)
		{
		case::XGlowEffect::ILL_START:
			m_Fader.Start(m_fMax, m_fMax, m_fStartDurationTime);
			break;
		case::XGlowEffect::ILL_START_DURATION:
			m_Fader.Start(m_fMax, m_fMin, m_fEndTime);
			break;
		case::XGlowEffect::ILL_END:
			m_Fader.Start(m_fMin, m_fMin, m_fEndDurationTime);
			break;
		case::XGlowEffect::ILL_END_DURATION:
			{
				if(m_bStop || m_bLoop == false)
					m_bDelMe = true;
				else
					m_Fader.Start(m_fMin, m_fMax, m_fStartTime);
			}
			break;
		}

		m_eState = (GLOW_SATE)((m_eState + 1) % ILL_MAX);
	}
}

void XGlowEffect::StopXEffect()
{
	m_bStop = true;
}

void XGlowEffect::SetOwner( MUID uidOwner )
{
	m_uidOwer = uidOwner;
}

bool XGlowEffect::IsDuplicationEffect( MUID& uidOwner, tstring strActorNodeName )
{
	// 중복 체크는 루프인 경우에만 한다.
	if(m_bLoop &&
		m_uidOwer == uidOwner &&
		m_strBoneName == strActorNodeName)
		return true;

	return false;
}

void XGlowEffect::SetLoop( bool bLoop )
{
	m_bLoop = bLoop;

	// 그러나 지속 시간이 있다면... loop는 자동으로 true이다.
	if(m_bUseDurationTime)
	{
		m_bLoop = true;
	}
}