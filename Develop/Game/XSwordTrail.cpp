#include "stdafx.h"
#include "XSwordTrail.h"
#include "XSwordTrailEffectController.h"
#include "XSwordTrailSamplingData.h"
#include "XCharacter.h"
#include "XSystem.h"
#include "XModuleTalent.h"
#include "XModuleBuff.h"
#include "XModuleEntity.h"
#include "XGameTransformControllerManager.h"

//////////////////////////////////////////////////////////////////////////
XSwordTrail::XSwordTrail()
{
	m_vecSwordTrailEffect.clear();
}

XSwordTrail::~XSwordTrail()
{

}

void XSwordTrail::Play(XObject * pObject)
{
	if(pObject == NULL)
		return;

	if(global.system && gg.omgr)
	{
		// 이벤트 정보 가져오기
		vector<SWORDTRAILEVENT_DATA> vecData;
		SWORDTRAIL_DATA* pSwordData = GetSwordTrailData(pObject);
		if(pSwordData == NULL)
			return ;

		wstring strSwordTrailTexture = GetSwordTrailTexture(pObject);
		for(vector<SWORDTRAILSAMPLING_DATA>::iterator it = pSwordData->vecSamplingData.begin(); it != pSwordData->vecSamplingData.end(); it++)
		{
			MUID uid = global.system->GenerateLocalUID();

			XSwordTrailEffectController * pSwordTrailEffect = new XSwordTrailEffectController(uid);
			pSwordTrailEffect->CreateSwordTrail(pObject->GetUID(), &(*it), strSwordTrailTexture);
			m_vecSwordTrailEffect.push_back(pSwordTrailEffect);

			gg.omgr->Add(pSwordTrailEffect);
		}
	}
}

void XSwordTrail::Destroy()
{
	for(vector<XSwordTrailEffectController *>::iterator it = m_vecSwordTrailEffect.begin(); it != m_vecSwordTrailEffect.end(); it++)
	{
		(*it)->DeleteMe();
	}

	m_vecSwordTrailEffect.clear();
}

void XSwordTrail::Done()
{
	vector<XSwordTrailEffectController *>::iterator it = m_vecSwordTrailEffect.begin();
	while(it != m_vecSwordTrailEffect.end())
	{
		if((*it)->IsDone() ||
			(*it)->IsStart() == false)			// 시작도 안했다면...
		{
			(*it)->DoneSwordTrail();
			it = m_vecSwordTrailEffect.erase(it);
			continue;
		}

		it++;
	}
}

SWORDTRAIL_DATA* XSwordTrail::GetSwordTrailData( XObject * pObject )
{
	XModuleEntity* pModuleEntity = pObject->GetModuleEntity();
	if(pModuleEntity == NULL ||
		pModuleEntity->GetAnimationController() == NULL ||
		info.swordtrail_sampling == NULL)
		return NULL;

	wstring n = MLocale::ConvAnsiToUTF16(pObject->GetActor()->GetMeshName().c_str());
	wstring nn = n.substr(0, n.rfind(L"."));

	wstring strAnimationName = pModuleEntity->GetAnimationController()->GetAnimationName();
	return info.swordtrail_sampling->Get(nn, strAnimationName);
}	

wstring XSwordTrail::GetSwordTrailTexture( XObject * pObject )
{
	wstring strItem;
	wstring strTalent;
	wstring strFocus;
	wstring strEnchantBuff;

	int nEnchantBuffID = 0;

	// 아이템
	if(pObject->GetEntityType() == ETID_PLAYER)
	{
		if(pObject->GetUID() == XGetMyUID())
		{
			// MyPlayer
			XItem* pItem = gvar.MyInfo.EquipmentSlot.GetCurrentWeaponSetWeaponRight();
			if(pItem)
			{
				strItem = pItem->m_pItemData->m_strSwordTrailEffect;
			}
		}
		else
		{
			// NetPlayer
			SH_ITEM_SLOT itemslot = ITEMSLOT_RWEAPON;
			XNetPlayer* pNetPlayer = AsNetPlayer(pObject);

			XItemData* pRItemData = NULL;
			XItemData* pLItemData = NULL;
			pNetPlayer->GetCurrWeapon(pRItemData, pLItemData);
			if(pRItemData)
			{
				strItem = pRItemData->m_strSwordTrailEffect;
			}
		}

		XPlayer* pPlayer = pObject->AsPlayer();
		nEnchantBuffID= pPlayer->GetVisualEnchantBuffID();

	}
	else if(pObject->GetEntityType() == ETID_NPC)
	{
		// NPC
	}

	// 탤런트
	XModuleTalent* pModuleTalent = pObject->GetModuleTalent();
	if(pModuleTalent)
	{
		strTalent = pModuleTalent->GetInfo()->m_strSwordTrailEffect;
	}

	// 강화
	if(nEnchantBuffID > 0)
	{
		// 버프 정보 가져오기
		XBuffInfo * pBuffInfo = info.buff->Get(nEnchantBuffID);
		if(pBuffInfo != NULL)
		{
			// 강화 버프 이펙트 정보 가져오기
			XActor* pXActor = pObject->AsActor();
			WEAPON_TYPE eRType = pXActor->GetEquipWeaponType(false);
			WEAPON_TYPE eLType = pXActor->GetEquipWeaponType(true);

			vector<XEnchantBuffEffectInfo *> vecEnchantEffectInfoByType;
			pBuffInfo->m_EnchantBuffEffect.GetEffectInfoList(EBET_TRAIL_EFFECT, eRType, eLType, PLAYER_PARTS_SLOT_RWEAPON, vecEnchantEffectInfoByType);
			if(vecEnchantEffectInfoByType.size() > 0)
				strEnchantBuff = vecEnchantEffectInfoByType[0]->m_strTrailTexName;
		}
	}

	// 결과
	if(strEnchantBuff.empty() == false)
	{
		return strEnchantBuff;
	}
	else if(strFocus.empty() == false)
	{
		return strFocus;
	}
	else if(strTalent.empty() == false)
	{
		return strTalent;
	}
	else if(strItem.empty() == false)
	{
		return strTalent;
	}

	return L"trail_test1.tga";		// 임시적입니다. 
}
