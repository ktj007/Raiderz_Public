#include "stdafx.h"
#include "XNetPlayer.h"
#include "XAnim.h"
#include "XConst.h"
#include "XGame.h"
#include "XWorld.h"
#include "XModuleMovable.h"
#include "XModuleNetControl.h"
#include "XModuleEntity.h"
#include "XModuleAction.h"
#include "XModuleEffect.h"
#include "XModuleCollision.h"
#include "XItemManager.h"
#include "CSItemData.h"
#include "CSItemHelper.h"
#include "XModuleUI.h"
#include "XModuleBuff.h"
#include "XPlayerPVP.h"
#include "XNaming.h"

MImplementRTTI(XNetPlayer, XPlayer);

XNetPlayer::XNetPlayer(MUID uid, UIID nUIID) : XPlayer(uid, nUIID)
{
	m_nTypeID = ETID_PLAYER;
	m_bPartyLeader = false;

	m_pModuleNetControl		= new XModuleNetControl(this);
}

XNetPlayer::~XNetPlayer()
{
	SAFE_DELETE(m_pModuleNetControl);
	//SAFE_DELETE(m_pModuleUI);
}

void XNetPlayer::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	XPlayer::OnUpdate(fDelta);
}


void XNetPlayer::RegisterModules()
{
	XPlayer::RegisterModules();

	m_pModuleNetControl->Initialize();
	m_Modules.AddModule(m_pModuleNetControl, true);


}

void XNetPlayer::UnregisterModules()
{
	m_Modules.RemoveModule(m_pModuleNetControl);
	m_pModuleNetControl->Finalize();

	XPlayer::UnregisterModules();
}

//void XNetPlayer::SetCharacterPane_AllyNetPlayer()
//{
//	wstring strName = GetCaptionName();
//	m_pModuleUI->SetCharacterPane(strName, wstring(m_Feature.szGuildName), ALLY_PLAYER_ID_COLOR);
//}

void XNetPlayer::SetCharacterPane_VSNetPlayer()
{
	wstring strName = GetCaptionName();
	m_pModuleUI->SetCharacterPane(strName, wstring(m_Feature.szGuildName), VS_PLAYER_ID_COLOR);
}

void XNetPlayer::SetCharacterPane_Player()
{
	wstring strName = GetCaptionName();
	m_pModuleUI->SetCharacterPane(strName, wstring(m_Feature.szGuildName), PLAYER_ID_COLOR);
}

void XNetPlayer::SetFeature(const wchar_t* szID, XPlayerInfoFeature& feature)
{
	m_strName = szID;
	m_Feature = feature;

	UpdateFeature();

	SetCharacterPane_Player();

	// 무기 강화
	if(m_pModuleBuff)
	{
		for(int i = 0; i < FEATURE_ITEMSLOT_MAX; ++i)
		{
			if(CheckEnchantBuffGinEnable((SH_FEATURE_ITEMSLOT)i))
			{
				bool bShowEffect = CheckEnchantBuffEffectShow(m_Feature.nEquipedItemID[i], (SH_FEATURE_ITEMSLOT)i);
				int nPartsSlotType = XPartsSlotType::NetSlot2LookSlot((SH_FEATURE_ITEMSLOT)i);
				m_pModuleBuff->BuffEnchantGain(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[i]), bShowEffect, nPartsSlotType);
			}
		}
	}
}

void XNetPlayer::UpdateFeature()
{
	// 외형 모델 변경
	if (m_pModuleEntity) m_pModuleEntity->SerializeNetPlayer(m_bLoadingAsync);

	// 모션 무기 타입 변경
	if (m_pModuleMotion) m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
}

void XNetPlayer::SwitchingWeaponSet( int8 nWeaponSet )
{
	if(m_pModuleBuff)
	{
		int nPartsSlotType = XPartsSlotType::NetSlot2LookSlot(GetRWeaponCurrentSlot());
		m_pModuleBuff->BuffEnchantLost(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[GetRWeaponCurrentSlot()]), true, nPartsSlotType);
		nPartsSlotType = XPartsSlotType::NetSlot2LookSlot(GetLWeaponCurrentSlot());
		m_pModuleBuff->BuffEnchantLost(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[GetLWeaponCurrentSlot()]), true, nPartsSlotType);
	}

	GetFeature().nWeaponSet = nWeaponSet;
	
	UpdateFeature();

	if(m_pModuleBuff)
	{
		int nPartsSlotType = XPartsSlotType::NetSlot2LookSlot(GetRWeaponCurrentSlot());
		m_pModuleBuff->BuffEnchantGain(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[GetRWeaponCurrentSlot()]), true, nPartsSlotType);
		nPartsSlotType = XPartsSlotType::NetSlot2LookSlot(GetLWeaponCurrentSlot());
		m_pModuleBuff->BuffEnchantGain(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[GetLWeaponCurrentSlot()]), true, nPartsSlotType);
	}
}

WEAPON_TYPE XNetPlayer::GetCurrWeaponType()
{
	CSItemData* pItemDataR = NULL;
	CSItemData* pItemDataL = NULL;

	switch (GetFeature().nWeaponSet)
	{
	case 0: // 주무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] != 0) 
				pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON] != 0) 
				pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON]);
		}break;
	case 1: // 보조무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2] != 0) 
				pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2] != 0) 
				pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2]);
		}break;
	default:
		_ASSERT(0);
	}
	

	return CSItemHelper::GetCurrentWeaponType(pItemDataR, pItemDataL);
}

bool XNetPlayer::IsEquipShield()
{
	CSItemData* pItemDataL = NULL;
	int nFeatureLWeapon = FEATURE_ITEMSLOT_LWEAPON;

	if(m_Feature.nWeaponSet != 0)
	{
		nFeatureLWeapon = FEATURE_ITEMSLOT_LWEAPON2;
	}

	if (m_Feature.nEquipedItemID[nFeatureLWeapon] != 0) 
	{
		pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[nFeatureLWeapon]);
		if (pItemDataL)
		{
			if (CSItemHelper::IsShieldItemType(pItemDataL))
			{
				return true;
			}
		}
	}
	return false;
}

CSItemData* XNetPlayer::GetEquipedItemData(SH_FEATURE_ITEMSLOT nFeatureItemSlot)
{
	return info.item->GetItemData(m_Feature.nEquipedItemID[nFeatureItemSlot]);
}

void XNetPlayer::EquipItem( SH_FEATURE_ITEMSLOT nFeatureItemSlot, int nItemID, SH_FEATURE_ITEMSLOT nUnEquipFeatureItemSlot, int nColor, int nEnchantItemID )
{
	if(nUnEquipFeatureItemSlot != FEATURE_ITEMSLOT_NONE)
	{
		EnchantBuffLost(nUnEquipFeatureItemSlot);

		m_Feature.nEquipedItemID[nUnEquipFeatureItemSlot] = 0;
	}

	// 버프 이펙트 삭제
	EnchantBuffLost(nFeatureItemSlot);

	m_Feature.nEquipedItemID[nFeatureItemSlot] = nItemID;
	m_Feature.nEquipedItemColor[nFeatureItemSlot] = nColor;
	m_Feature.nEquipedItemEnchantBuff[nFeatureItemSlot] = nEnchantItemID;
	m_pModuleEntity->SerializeNetPlayer();

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}

	if(m_pModuleBuff && CheckEnchantBuffGinEnable(nFeatureItemSlot))
	{
		bool bShowEffect = CheckEnchantBuffEffectShow(m_Feature.nEquipedItemID[nFeatureItemSlot], nFeatureItemSlot);
		int nPartsSlotType = XPartsSlotType::NetSlot2LookSlot(nFeatureItemSlot);
		m_pModuleBuff->BuffEnchantGain(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[nFeatureItemSlot]), bShowEffect, nPartsSlotType);
	}
}

void XNetPlayer::UnEquipItem(SH_FEATURE_ITEMSLOT nFeatureItemSlot)
{
	EnchantBuffLost(nFeatureItemSlot);

	m_Feature.nEquipedItemID[nFeatureItemSlot] = 0;
	m_pModuleEntity->SerializeNetPlayer();

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}
}

void XNetPlayer::OnRender()
{
	XPlayer::OnRender();
	_RenderDebug();
}


void XNetPlayer::_RenderDebug()
{
#ifdef _PUBLISH
	return;
#endif // _PUBLISH

	// NPC 이름(디버그용)
	//vec3 vTextPos = GetPosition();
	//vTextPos.z += 150.0f+35.0f;

	//XRenderHelper::DrawText(vTextPos, MCOLOR(0xEE0000FF), m_strName.c_str());

}

XModuleActorControl* XNetPlayer::GetModuleActorControl()	
{ 
	return m_pModuleNetControl; 
}

void XNetPlayer::InPlayer( TD_UPDATE_CACHE_PLAYER* pPlayerInfo, TD_PLAYER_FEATURE_TATTOO* pTattooInfo, bool bAppearEffect/*=true*/, bool bLoadingAsync /*= true*/ )
{
	m_nUIID = pPlayerInfo->nUIID;

	// 외모 정보
	m_bLoadingAsync = bLoadingAsync;

	XPlayerInfoFeature feature(pPlayerInfo->Feature);
	if ( pTattooInfo != NULL)
		feature.SetTattooInfo( *pTattooInfo);

	SetFeature(pPlayerInfo->szName, feature);
	SetStance(CHAR_STANCE(pPlayerInfo->nStance));


	// 장비
	//TODO: 여기서 장비 바꿔줘야함

	if(m_pModuleNetControl)
	{
		vec3 vPlayerDir;
		vPlayerDir = pPlayerInfo->svDir;
		m_pModuleNetControl->OnEnterGame(pPlayerInfo->vPos, vPlayerDir, this);
	}

	if (m_pModuleMotion)
	{
		if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_DEAD))
		{
			m_bDead = true;
			DeadProcEnterField();

			if(m_pModuleMovable)
				m_pModuleMovable->TriggerDie(true);
		}
		else if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_SWIMMING))
		{
			SetSwim(true);

			if(m_pModuleMovable)
				m_pModuleMovable->TriggerSwim(true);
		}
		else if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_LOOTING))
		{
			m_pModuleMotion->ChangeMotion(MOTION_NAME_LOOTING_ITEM, MOTION_NAME_IDLE);
		}
		else if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_SITTING))
		{
			SetSitting(true);
			m_pModuleNetControl->DoActionSitDown();
			m_pModuleMotion->ChangeMotion(MOTION_NAME_SIT_IDLE);
		}
		else if(CheckBitSet(pPlayerInfo->nStatusFlag, UPS_CUTSCENING))
		{
			SetCutScene(true);
			m_pModuleMotion->ChangeMotion(MOTION_NAME_IDLE);
		}
		else
		{
			m_pModuleMotion->ChangeMotion(MOTION_NAME_IDLE);
		}
	}

	if(CheckBitSet(pPlayerInfo->nStatusFlag, UPS_AFK))
	{
		SetAFK(true);
		m_pModuleNetControl->DoActionSitDown();
		//m_pModuleMotion->ChangeMotion(MOTION_NAME_SIT_IDLE);

		if(IsDuel())	SetCharacterPane_VSNetPlayer();
		else			SetCharacterPane_Player();
	}

	if(CheckBitSet(pPlayerInfo->nStatusFlag, UPS_PARTYLEADER))
	{
		SetPartyLeader(true);
	}

	// pvp 지역
	if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_FIELDPVP_TEAM1))
	{
		m_pPlayerPVP->SetPvPAreaFaction(true, false);
	}
	else if (CheckBitSet(pPlayerInfo->nStatusFlag, UPS_FIELDPVP_TEAM2))
	{
		m_pPlayerPVP->SetPvPAreaFaction(false, true);
	}

	if (m_pModuleCollision)
	{
		wstring strMeshName = XNaming::GetPlayerModelSimpleName(pPlayerInfo->Feature.nSex);
		CSMeshInfo* pMeshInfo = info.mesh_info->GetInfo(strMeshName);
		m_pModuleCollision->InitCol(pMeshInfo);
	}

	// 캐릭터를 서서히 나타나게 한다.
	if (bAppearEffect)
	{
		m_pModuleNetControl->StartAppear();
	}

	// 버프
	SetBuff(pPlayerInfo);

	// 모션 팩터
	if(pPlayerInfo->nMF == MF_SWALLOWED)
	{
		// 먹기라면...
		m_pModuleNetControl->WaitSwallowedSynch(pPlayerInfo->nMFWeight);
	}

	if (m_pModuleEntity)
	{
		// fake beaten 애니메이션 컨트롤러
		m_pModuleEntity->CreateFakeBeatenAnimationController(tstring(XCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME), XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT, XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED);
	}
}

void XNetPlayer::SetCharExtFeature( const XCharExtFeature* pExtFeature)
{
	m_Feature.nMakeUp = pExtFeature->nMakeUp;
	m_Feature.nTattoo = pExtFeature->nTattoo;
	m_Feature.fTattooScale = pExtFeature->fTattooScale;
	m_Feature.vTattooPos = pExtFeature->vTattooPos;

	m_pModuleEntity->SetCharExtFeature( pExtFeature);
}

ARMOR_TYPE XNetPlayer::GetArmorType()
{
	CSItemData* pItemData = GetEquipedItemData(FEATURE_ITEMSLOT_BODY);
	if (pItemData == NULL) return ARMOR_NONE;

	return pItemData->m_nArmorType;
}

void XNetPlayer::ChangeGuildName( wstring strName )
{
	wcscpy_s(m_Feature.szGuildName, strName.c_str());
}

void XNetPlayer::ChangeNameForPvP( wstring strName )
{
	if ( m_KeepingDataForPvp.bChanged == false)
	{
		m_KeepingDataForPvp.strName = m_strName;
		m_KeepingDataForPvp.strGuild = m_Feature.szGuildName;
	}

	m_KeepingDataForPvp.bChanged = true;

	m_strName = strName;
	wcscpy_s(m_Feature.szGuildName, L"");
}

void XNetPlayer::RestoreNameForPvP()
{
	if ( m_KeepingDataForPvp.bChanged == false)
		return;

	m_strName = m_KeepingDataForPvp.strName;
	wcscpy_s( m_Feature.szGuildName, m_KeepingDataForPvp.strGuild.c_str());
}

const wchar_t* XNetPlayer::GetRealNameInPvP()
{
	if ( m_KeepingDataForPvp.bChanged == true)
	{
		if ( !m_KeepingDataForPvp.strName.empty())
			return m_KeepingDataForPvp.strName.c_str();
	}

	return GetName();
}

void XNetPlayer::SetBuff( TD_UPDATE_CACHE_PLAYER* pPlayerInfo )
{
	if(m_pModuleBuff)
	{
		// 게임 처음 진입시 버프 입력
		for(int iBuff = 0; iBuff < MAX_OWN_BUFF_NUMBER; iBuff++)
		{
			int nBuffID = pPlayerInfo->Buffs[iBuff];
			if (nBuffID == 0) continue;

			if (m_pModuleBuff->BuffExist(nBuffID) == false)
			{
				m_pModuleBuff->SetRemainBuffList(nBuffID, 0.0f);
			}
		}
	}
}

void XNetPlayer::GetCurrWeapon( XItemData* pItemDataR, XItemData* pItemDataL )
{
	switch (GetFeature().nWeaponSet)
	{
	case 0: // 주무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] != 0) 
				pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON] != 0) 
				pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON]);
		}break;
	case 1: // 보조무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2] != 0) 
				pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2] != 0) 
				pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2]);
		}break;
	default:
		_ASSERT(0);
	}
}

SH_FEATURE_ITEMSLOT XNetPlayer::GetRWeaponCurrentSlot()
{
	if(GetFeature().nWeaponSet == 0)
	{
		return FEATURE_ITEMSLOT_RWEAPON;
	}

	return FEATURE_ITEMSLOT_RWEAPON2;
}

SH_FEATURE_ITEMSLOT XNetPlayer::GetLWeaponCurrentSlot()
{
	if(GetFeature().nWeaponSet == 0)
	{
		return FEATURE_ITEMSLOT_LWEAPON;
	}

	return FEATURE_ITEMSLOT_LWEAPON2;
}

bool XNetPlayer::CheckEnchantBuffGinEnable( SH_FEATURE_ITEMSLOT nFeatureItemSlot )
{
	SH_FEATURE_ITEMSLOT nNotCurRWeaponSet;
	SH_FEATURE_ITEMSLOT nNotCurLWeaponSet;

	if(GetFeature().nWeaponSet == 0)
	{
		nNotCurRWeaponSet = FEATURE_ITEMSLOT_RWEAPON2;
		nNotCurLWeaponSet = FEATURE_ITEMSLOT_LWEAPON2;
	}
	else if(GetFeature().nWeaponSet == 1)
	{
		nNotCurRWeaponSet = FEATURE_ITEMSLOT_RWEAPON;
		nNotCurLWeaponSet = FEATURE_ITEMSLOT_LWEAPON;
	}

	if(nNotCurLWeaponSet == nFeatureItemSlot ||
		nNotCurRWeaponSet == nFeatureItemSlot)
		return false;

	return true;
}

bool XNetPlayer::CheckEnchantBuffEffectShow( int nItemID, SH_FEATURE_ITEMSLOT nFeatureItemSlot )
{
	if(nFeatureItemSlot == FEATURE_ITEMSLOT_RWEAPON ||
		nFeatureItemSlot == FEATURE_ITEMSLOT_RWEAPON2 ||
		nFeatureItemSlot == FEATURE_ITEMSLOT_LWEAPON ||
		nFeatureItemSlot == FEATURE_ITEMSLOT_LWEAPON2)
	{
		// 이펙트는 프록만 나온다.
		XItemData* pItemData = info.item->GetItemData(nItemID);
		if(pItemData == NULL || 
			(pItemData->m_vecEquipEnchantColor.size() > 0 && pItemData->m_vecEquipEnchantColor[0] == ENCHANT_NONE))
			return false;

		return true;
	}

	return false;
}

int XNetPlayer::GetVisualEnchantBuffID()
{
	return m_Feature.nEquipedItemEnchantBuff[GetRWeaponCurrentSlot()];
}

int XNetPlayer::GetEnchantBuffID( int nID )
{
	XItemData* pItemData = info.item->GetItemData(nID);
	if(pItemData == NULL)
		return 0;

	return pItemData->m_EnchantStone.nLinkedBuffID;
}

void XNetPlayer::DelVisualEnchantBuffEffect()
{
	for(int i = 0; i < FEATURE_ITEMSLOT_MAX; ++i)
	{
		if(CheckEnchantBuffGinEnable((SH_FEATURE_ITEMSLOT)i))
		{
			bool bShowEffect = CheckEnchantBuffEffectShow(m_Feature.nEquipedItemID[i], (SH_FEATURE_ITEMSLOT)i);
			int nPartsSlotType = XPartsSlotType::NetSlot2LookSlot((SH_FEATURE_ITEMSLOT)i);
			m_pModuleBuff->BuffEnchantLost(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[i]), bShowEffect, nPartsSlotType);
		}
	}
}

void XNetPlayer::EnchantBuffLost( SH_FEATURE_ITEMSLOT& nFeatureItemSlot )
{
	// 버프 이펙트 삭제
	if(m_pModuleBuff && m_Feature.nEquipedItemEnchantBuff[nFeatureItemSlot] > 0)
	{
		bool bHideEffect = CheckEnchantBuffEffectShow(m_Feature.nEquipedItemID[nFeatureItemSlot], nFeatureItemSlot);
		int nPartsSlotType = XPartsSlotType::NetSlot2LookSlot(nFeatureItemSlot);
		m_pModuleBuff->BuffEnchantLost(GetEnchantBuffID(m_Feature.nEquipedItemEnchantBuff[nFeatureItemSlot]), bHideEffect, nPartsSlotType);
	}
}

WEAPON_TYPE XNetPlayer::GetEquipWeaponType( bool bLeft )
{
	CSItemData* pItemDataR = NULL;
	CSItemData* pItemDataL = NULL;

	switch (GetFeature().nWeaponSet)
	{
	case 0: // 주무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] != 0) 
				pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON] != 0) 
				pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON]);
		}break;
	case 1: // 보조무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2] != 0) 
				pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2] != 0) 
				pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2]);
		}break;
	default:
		_ASSERT(0);
	}

	if(bLeft && pItemDataL)
		return pItemDataL->m_nWeaponType;
	else if(pItemDataR)
		return pItemDataR->m_nWeaponType;

	return WEAPON_NONE;
}

void XNetPlayer::GetCurrentWeaponSetWeaponItemData( XItemData** pItemDataR, XItemData** pItemDataL )
{
	switch (GetFeature().nWeaponSet)
	{
	case 0: // 주무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON] != 0) 
				*pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON] != 0) 
				*pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON]);
		}break;
	case 1: // 보조무기
		{
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2] != 0) 
				*pItemDataR = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_RWEAPON2]);
			if (m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2] != 0) 
				*pItemDataL = info.item->GetItemData(m_Feature.nEquipedItemID[FEATURE_ITEMSLOT_LWEAPON2]);
		}break;
	default:
		_ASSERT(0);
	}
}

void XNetPlayer::EquipOverlapped( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL)
		return;

	SH_FEATURE_ITEMSLOT ItemSlot = XPlayerInfoFeature::TransItemSlotToNetSlot(pItemData->m_nItemSlot);
	SH_FEATURE_ITEMSLOT ItemSubSlot = XPlayerInfoFeature::TransItemSlotToNetSlot(pItemData->m_nSubSlot);;

	if(GetFeature().nWeaponSet == 1)
	{
		if(ItemSlot == FEATURE_ITEMSLOT_RWEAPON)
			ItemSlot = FEATURE_ITEMSLOT_RWEAPON2;
		else if(ItemSlot == FEATURE_ITEMSLOT_LWEAPON)
			ItemSlot = FEATURE_ITEMSLOT_LWEAPON2;
		if(ItemSubSlot == FEATURE_ITEMSLOT_LWEAPON)
			ItemSubSlot = FEATURE_ITEMSLOT_LWEAPON2;
	}

	m_OverlappedFeature = m_Feature;

	EquipItem(ItemSlot, nItemID, ItemSubSlot, pItemData->m_nTexColor, 0);

	XPlayer::EquipOverlapped(nItemID);
}


void XNetPlayer::UnEquipOverlapped( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL)
		return;

	for(int i = 0; i < FEATURE_ITEMSLOT_MAX; ++i)
	{
		if(m_Feature.nEquipedItemID[i] != m_OverlappedFeature.nEquipedItemID[i])
		{
			if(m_OverlappedFeature.nEquipedItemID[i] == 0)
				UnEquipItem(SH_FEATURE_ITEMSLOT(i));
			else
				EquipItem(SH_FEATURE_ITEMSLOT(i), m_OverlappedFeature.nEquipedItemID[i], FEATURE_ITEMSLOT_NONE, m_OverlappedFeature.nEquipedItemColor[i], m_OverlappedFeature.nEquipedItemEnchantBuff[i]);
		}
	}

	m_OverlappedFeature.Init();

	XPlayer::UnEquipOverlapped(nItemID);
}
