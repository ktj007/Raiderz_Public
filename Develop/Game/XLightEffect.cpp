#include "stdafx.h"
#include "XLightEffect.h"
#include "RPointLightSceneNode.h"
#include "XCharacter.h"

XLightEffect::XLightEffect()
: XEffectBase()
{

}

XLightEffect::~XLightEffect()
{

}

bool XLightEffect::Create( MUID& uid )
{
	m_UID = uid;

	m_pEffect = new RPointLightSceneNode();

	//REngine::GetSceneManager().AddSceneNode(m_pEffect);

	GetEffect()->m_fApplyIntensity = 5.0f;

	return true;
}

void XLightEffect::UpdateXEffect( float fDelta )
{
	//UpdateFollow();
	UpdateDurationTime(fDelta);

	m_pEffect->UpdateTransform();
}

void XLightEffect::UpdateFollow()
{
	if(m_uidOwer != MUID::ZERO)
	{
		XObject* pObject = gg.omgr->FindActor(m_uidOwer);
		if(pObject == NULL)
		{
			// 없어졌다.
			m_bDelMe = true;
			return;
		}

		vec3 vWorldPosition = pObject->GetPosition();

		pObject->GetActor()->GetActorNodePos(m_strBoneName.c_str(), vWorldPosition, E_TS_WORLD);
	}
}

void XLightEffect::SetRadius( float fRadius )
{
	if(m_pEffect == NULL)
		return;

	RPointLightSceneNode* pEffect = static_cast<RPointLightSceneNode*>(m_pEffect);
	pEffect->SetRadius(fRadius);
}

void XLightEffect::SetColorVector( RVector vColor )
{
	if(m_pEffect == NULL)
		return;

	RPointLightSceneNode* pEffect = static_cast<RPointLightSceneNode*>(m_pEffect);
	pEffect->SetColorVector(vColor);
}

void XLightEffect::SetColorRGB( DWORD dwColor )
{
	if(m_pEffect == NULL)
		return;

	RPointLightSceneNode* pEffect = static_cast<RPointLightSceneNode*>(m_pEffect);
	pEffect->SetColorRGB(dwColor);
}

void XLightEffect::SetWorldMatrix( RMatrix& mat )
{
	if(m_pEffect == NULL)
		return;

	RPointLightSceneNode* pEffect = static_cast<RPointLightSceneNode*>(m_pEffect);
	pEffect->SetTransform(mat);

}

void XLightEffect::SetAttenuationStart( float fStartDist )
{
	if(m_pEffect == NULL)
		return;

	RPointLightSceneNode* pEffect = static_cast<RPointLightSceneNode*>(m_pEffect);
	pEffect->m_fAttenuationStart = fStartDist;
}

void XLightEffect::SetIntensity( float fIntensity )
{
	if(m_pEffect == NULL)
		return;

	RPointLightSceneNode* pEffect = static_cast<RPointLightSceneNode*>(m_pEffect);
	pEffect->m_fApplyIntensity = fIntensity;
}