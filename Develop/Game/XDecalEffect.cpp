#include "stdafx.h"
#include "XDecalEffect.h"
#include "RDecalProjectionSceneNode.h"
#include "XCharacter.h"
#include "XCollisionTree.h"
#include "XGameState.h"

XDecalEffect::XDecalEffect()
: XEffectBase()
, m_eType(DET_NONE)
, m_uidOwer(MUID::ZERO)
{
	m_fSizeX = m_fSizeY = m_fSizeZ = 0.0f;
	m_strBoneName.clear();
}

XDecalEffect::~XDecalEffect()
{

}

bool XDecalEffect::Create( MUID& uid, const wstring& str, BYTE byDecalEffectType, const vec3& vPos, const float fSizeX, const float fSizeY, const float fSizeZ )
{
	m_UID = uid;
	m_strEffectFileName = str;
	m_eType = (DECALEFFECT_TYPE)byDecalEffectType;

	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_fSizeZ = fSizeZ;

	if(m_eType == DET_PROJECTION)
	{
		rs3::RCollisionTree* pCollisionTree = static_cast<rs3::RCollisionTree*>(gg.currentgamestate->GetWorld()->GetCollisionTree());
		m_pEffect = new RDecalProjectionSceneNode;
		return static_cast<RDecalProjectionSceneNode*>(m_pEffect)->Create(pCollisionTree, MLocale::ConvUTF16ToAnsi(str.c_str()), vPos, fSizeX, fSizeY, fSizeZ);
	}

	return false;
}

void XDecalEffect::UpdateXEffect( float fDelta )
{
	if(m_pEffect == NULL)
		return;

	UpdateFollow();
	UpdateDurationTime(fDelta);
	UpdateEffectFader(fDelta);
}

void XDecalEffect::SetDecalEffectData( const vec3& vPos, const float fSizeX, const float fSizeY, const float fSizeZ )
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_fSizeZ = fSizeZ;

	if(m_eType == DET_PROJECTION)
	{
		if(m_pEffect == NULL)
			return;
		GetEffect()->Update(vPos, fSizeX, fSizeY, fSizeZ);
	}
}

void XDecalEffect::SetDecalEffectColor( DWORD dwColor )
{
	if(m_pEffect == NULL)
		return;

	GetEffect()->SetColor(dwColor);
}

void XDecalEffect::StopXEffect()
{
	if(m_pEffect)
	{
		m_pEffect->RemoveFromParent();
		SAFE_DELETE(m_pEffect);
	}
}

bool XDecalEffect::IsEffectLive()
{
	if(m_pEffect)
		return true;

	return false;
}

void XDecalEffect::UpdateFollow()
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

		SetDecalEffectData(vWorldPosition, m_fSizeX, m_fSizeY, m_fSizeZ);
	}
}

void XDecalEffect::CheckOwerChangeMotion( MUID uidOwner )
{
	if(m_bDelMe)
		return;

	if(uidOwner != m_uidOwer)
		return;

	if(m_bCancelEnabled)
		m_bDelMe = true;
}