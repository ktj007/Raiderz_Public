#include "stdafx.h"
#include "TEffect.h"
#include "REffectInstance.h"

TEffect::TEffect()
:XModelEffect()
, m_bFollowWaterPos(false)
, m_pCharacter(NULL)
{
}

TEffect::~TEffect()
{

}

bool TEffect::Create(MUID& uid, const string& str)
{
	m_UID = uid;

	REffectInstanceSceneNode* pEffectInstance = static_cast<REffectInstanceSceneNode*>(REngine::GetSceneManagerPtr()->CreateSceneNode(RSID_EFFECTINSTANCE));
	if( !pEffectInstance->Create( str.c_str() ) )
	{
#ifdef _DEBUG
		mlog("CreateEffect ½ÇÆÐ(%s)\n", str.c_str());
#endif
		SAFE_DELETE(pEffectInstance);
		return false;
	}

	m_pEffect = pEffectInstance;
	m_strEffectFileName = str;

	return true;
}

void TEffect::UpdateXEffect( float fDelta )
{
	if( m_pEffect )
	{
		RSceneNodeUpdateVisitor v;
		m_pEffect->Traverse( &v );
	}

	UpdateWaterPos();
	UpdateDurationTime(fDelta);
	UpdateEffectFader(fDelta);
}

void TEffect::UpdateWaterPos()
{
	if(m_bFollowWaterPos == false)
		return;

	vec3 vPos = m_pCharacter->GetWorldPosition();
	vec3 vDirect = m_pCharacter->GetWorldDirection();
	vec3 vUp = vec3::AXISZ;

	if(m_strBoneName.empty() == false)
	{
		m_pCharacter->GetActorNodePos(m_strBoneName.c_str(), vPos, E_TS_WORLD, true);
	}

	vPos.z = SWIMMING_HEIGHT_VALUE_TEST;

	MMatrix matResult;
	matResult.SetLocalMatrix(vPos, vDirect, vUp);

	m_pEffect->SetTransform(matResult);
}