#include "stdafx.h"
#include "XSelfIlluminationEffect.h"
#include "XCharacter.h"

XSelfIlluminationEffect::XSelfIlluminationEffect()
:XGlowEffect()
{

}

XSelfIlluminationEffect::~XSelfIlluminationEffect()
{
	// 초기화
	SetSelfIlluminationUpdate(1.0f);
}

void XSelfIlluminationEffect::SetSelfIlluminationUpdate( float fScale )
{
	if(m_uidOwer == MUID::ZERO)
		return;

	XObject* pObject = gg.omgr->FindActor(m_uidOwer);
	if(pObject == NULL)
	{
		// 없어졌다.
		m_bDelMe = true;
		return;
	}

	if(m_strBoneName.empty())
		pObject->GetActor()->SetIlluminationScale(fScale);
	else
		pObject->GetActor()->SetIlluminationScale(MLocale::ConvUTF16ToAnsi(m_strBoneName.c_str()), fScale);
}

bool XSelfIlluminationEffect::IsEffectLive()
{
	XObject* pObject = gg.omgr->FindActor(m_uidOwer);
	if(pObject == NULL)
	{
		// 없어졌다.
		m_bDelMe = true;
		return false;
	}

	if(m_strBoneName.empty() == false && pObject->GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(m_strBoneName.c_str()).c_str()) == NULL)
		return false;

	return true;
}

void XSelfIlluminationEffect::CheckOwerChangeMotion( MUID uidOwner )
{
	if(m_bDelMe)
		return;

	if(uidOwner != m_uidOwer)
		return;

	if(m_bCancelEnabled)
		m_bDelMe = true;
}