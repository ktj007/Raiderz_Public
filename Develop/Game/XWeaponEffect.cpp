#include "stdafx.h"
#include "XWeaponEffect.h"
#include "REffectInstance.h"
#include "RSceneManager.h"
#include "XResourceManager.h"
#include "XCharacter.h"

XWeaponEffect::XWeaponEffect()
: XModelEffect()
{
	m_nPartsSlotType = PLAYER_PARTS_SLOT_RWEAPON;
}

XWeaponEffect::~XWeaponEffect()
{

}

bool XWeaponEffect::Create( MUID& uid, const wstring& str )
{
	m_UID = uid;

	if (!global.res)
		return false;

	REffectInstanceSceneNode* pEffectInstance = global.res->CreateEffectWithBGCache(str);
	if (NULL == pEffectInstance)
		return false;

	if (global.res->IsUsingEffectCacheLog())
		global.res->LogEffectCache(str);

	m_pEffect = pEffectInstance;
	m_strEffectFileName = str;

	return true;
}

void XWeaponEffect::UpdateXEffect( float fDelta )
{
	UpdateDurationTime(fDelta);
	UpdateWeaponEffect();
}

void XWeaponEffect::UpdateWeaponEffect()
{
	XObject* pObject = gg.omgr->FindActor(m_uidOwer);

	if(pObject == NULL || m_pEffect == NULL || m_strBoneName.empty())
	{
		m_bDelMe = true;
		return;
	}

	XCharacter* pCharacter = pObject->GetActor();
	//////////////////////////////////////////////////////////////////////////
	// 붙여진 무기 붙이기...
	// 무기 부모 위치 가져오기
	vector<RActorNode*> weaponActorNode;
	vector<tstring> weaponActorNodeName;
	//m_pCharacter->GetPartsActorNode(PLAYER_PARTS_SLOT_LWEAPON, weaponActorNode, weaponActorNodeName);
	pCharacter->GetPartsActorNode(m_nPartsSlotType, weaponActorNode, weaponActorNodeName);

	//RMatrix matLocal;
	//RActorNode* pActorNode = pCharacter->GetActorNodeTransform(m_strBoneName.c_str(), matLocal, E_TS_LOCAL);
	//if(pActorNode == NULL)
	//{
	//	m_bDelMe = true;
	//	return;
	//}

	// 지금은 무기 하나에만 이펙트를 붙인다. 나중에 2개일경우를 생각해서 하자.
	RActorNode* pWeaponParentNode = NULL;
	RActorNode* pWeaponNode = NULL;
	for(vector<RActorNode*>::iterator it = weaponActorNode.begin(); it != weaponActorNode.end(); ++it)
	{
		if((*it) && (*it)->GetNodeName() == MLocale::ConvUTF16ToAnsi(m_strBoneName.c_str()).c_str() && (*it)->GetParent())
		{
			pWeaponNode = *it;
		}

		if((*it) && (*it)->GetParentActorNode() && (*it)->GetParentActorNode()->GetVisibility() > 0.0f)
		{
			pWeaponParentNode = (*it)->GetParentActorNode();
		}
	}

	if(pWeaponNode && pWeaponParentNode)
	{
		RMatrix matLocal = pWeaponNode->GetLocalTransform();
		RMatrix matWeaponWorld = pWeaponParentNode->GetWorldTransform();
		RMatrix matResult = matLocal * matWeaponWorld;

		m_pEffect->SetTransform(matResult);
		m_pEffect->SetScale(pWeaponNode->GetScale());
		m_pEffect->SetVisibility(1.0f);
	}
	//else if(pWeaponNode == NULL)
	//{
	//	m_bDelMe = true;
	//}
	else if(pWeaponNode == NULL || pWeaponParentNode == NULL)
	{
		m_pEffect->SetVisibility(0.0f);
	}
}