#include "stdafx.h"
#include "TSelfIlluminationEffect.h"

TSelfIlluminationEffect::TSelfIlluminationEffect()
:XGlowEffect()
{

}

TSelfIlluminationEffect::~TSelfIlluminationEffect()
{
	// ÃÊ±âÈ­
	SetSelfIlluminationUpdate(1.0f);
}

void TSelfIlluminationEffect::SetSelfIlluminationUpdate( float fScale )
{
	if(g_pMainApp->GetActor() == NULL)
		return;

	if(m_strBoneName.empty())
		g_pMainApp->GetActor()->SetIlluminationScale(fScale);
	else
		g_pMainApp->GetActor()->SetIlluminationScale(m_strBoneName, fScale);
}

bool TSelfIlluminationEffect::IsEffectLive()
{
	if(g_pMainApp->GetActor() == NULL || (m_strBoneName.empty() == false && g_pMainApp->GetActor()->GetActorNode(m_strBoneName.c_str()) == NULL))
		return false;

	return true;
}

bool TSelfIlluminationEffect::Create( float fStartTime, float fStartDurationTime, float fEndTime, float fEndDurationTime, float fMin, float fMax )
{
	MUID uidEmpty = MUID::ZERO;
	return XGlowEffect::Create(uidEmpty, fStartTime, fStartDurationTime, fEndTime, fEndDurationTime, fMin, fMax);
}