#include "stdafx.h"
#include "TWeaponEffect.h"
#include "REffectInstance.h"

TWeaponEffect::TWeaponEffect()
: XModelEffect()
, m_nWeaponEffectIndex(0)
{

}

TWeaponEffect::~TWeaponEffect()
{

}

bool TWeaponEffect::Create( MUID& uid, const string& str, int nIndex )
{
	m_UID = uid;

	REffectInstanceSceneNode* pEffectInstance = static_cast<REffectInstanceSceneNode*>(REngine::GetSceneManagerPtr()->CreateSceneNode(RSID_EFFECTINSTANCE));
	if( !pEffectInstance->Create( str.c_str() ) )
	{
#ifdef _DEBUG
		mlog("CreateEffect 실패(%s)\n", str.c_str());
#endif
		SAFE_DELETE(pEffectInstance);
		return false;
	}

	m_pEffect = pEffectInstance;
	m_strEffectFileName = str;
	m_nWeaponEffectIndex = nIndex;

	char chEffectName[256] = {0,};
	sprintf_s(chEffectName, "%s%02d", WEAPON_EFFECT_DUMMY_NAME_TOOL, m_nWeaponEffectIndex);
	m_strBoneName = chEffectName;

	return true;
}

void TWeaponEffect::UpdateXEffect( float fDelta )
{
	if( m_pEffect )
	{
		RSceneNodeUpdateVisitor v;
		m_pEffect->Traverse( &v );
	}

	UpdateDurationTime(fDelta);
	UpdateEffectFader(fDelta);
	UpdateWeaponEffect();
}

void TWeaponEffect::UpdateWeaponEffect()
{
	if(m_pCharacter == NULL || m_pEffect == NULL)
		return;

	//////////////////////////////////////////////////////////////////////////
	// 붙여진 무기 붙이기...
	// 무기 부모 위치 가져오기
	vector<RActorNode*> weaponActorNode;
	vector<tstring> weaponActorNodeName;
	m_pCharacter->GetPartsActorNode(m_pCharacter->Slot2LookSlot(ITEMSLOT_LWEAPON), weaponActorNode, weaponActorNodeName);
	m_pCharacter->GetPartsActorNode(m_pCharacter->Slot2LookSlot(ITEMSLOT_RWEAPON), weaponActorNode, weaponActorNodeName);

	RMatrix matLocal;
	RActorNode* pActorNode = m_pCharacter->GetActorNodeTransform(m_strBoneName.c_str(), matLocal, E_TS_LOCAL);
	if(pActorNode == NULL)
	{
		m_bDelMe = true;
		return;
	}

	// 지금은 무기 하나에만 이펙트를 붙인다. 나중에 2개일경우를 생각해서 하자.
	RActorNode* pWeaponNode = NULL;
	for(vector<RActorNode*>::iterator it = weaponActorNode.begin(); it != weaponActorNode.end(); ++it)
	{
		if((*it) && (*it)->GetParent() && (*it)->GetParent()->GetVisibility() > 0.0f)
		{
			pWeaponNode = *it;
			break;
		}
	}

	if(pWeaponNode)
	{
		RMatrix matWeaponWorld = pWeaponNode->GetWorldTransform();
		RMatrix matResult = matLocal * matWeaponWorld;

		m_pEffect->SetTransform(matResult);
		m_pEffect->SetScale(pActorNode->GetScale());
	}
}