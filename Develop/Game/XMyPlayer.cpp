#include "stdafx.h"
#include "XMyPlayer.h"
#include "XAnim.h"
#include "XConst.h"
#include "XGame.h"
#include "XWorld.h"
#include "XBaseApplication.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XModuleCollision.h"
#include "XModuleAction.h"
#include "XModulePost.h"
#include "XModuleBuff.h"
#include "XModuleUI.h"
#include "XController.h"
#include "XCameraManager.h"
#include "XGameTransformControllerManager.h"
#include "CSItemHelper.h"
#include "XNaming.h"
#include "XColorTable.h"
#include "XItemManager.h"
#include "XPost_Action.h"


MImplementRTTI(XMyPlayer, XPlayer);

///////////////////////////////////////////////////////////////////////////////////////////
/// XMyPlayer
XMyPlayer::XMyPlayer(MUID uid, UIID nUIID) : XPlayer(uid, nUIID)
{
	m_nTypeID = ETID_PLAYER;

	m_pModuleMyControl = new XModuleMyControl(this);
	m_pModulePost = new XModulePost(this);
	
}

XMyPlayer::~XMyPlayer()
{
	SAFE_DELETE(m_pModuleMyControl);
	SAFE_DELETE(m_pModulePost);
}

void XMyPlayer::OnDestroy()
{
	OutMyInfo();
	XPlayer::OnDestroy();
}

void XMyPlayer::RegisterModules()
{
	XPlayer::RegisterModules();

	m_pModuleMyControl->Initialize();
	m_Modules.AddModule(m_pModuleMyControl, true);

	m_pModulePost->Initialize();
	m_Modules.AddModule(m_pModulePost, true);

}

void XMyPlayer::UnregisterModules()
{
	m_Modules.RemoveModule(m_pModulePost);
	m_pModulePost->Finalize();

	m_Modules.RemoveModule(m_pModuleMyControl);
	m_pModuleMyControl->Finalize();

	XPlayer::UnregisterModules();
}

void XMyPlayer::OnUpdate(float fDelta)
{
	PFC_THISFUNC;
	XPlayer::OnUpdate(fDelta);

	gvar.Game.SoundLootItem.Update(fDelta);
}

bool XMyPlayer::ValidateChangeStance(CHAR_STANCE nStanceTo)
{
	// 수영인가?
	if(IsSwim())
	{
		// 수영중이네... 그렇다면 스텐스 변경 금지
		return false;
	}

	if (nStanceTo == CS_NORMAL)
	{
		if ((m_pModuleMotion->GetCurrMotion() != MOTION_NAME_IDLE) &&
			(m_pModuleMotion->GetCurrMotion() != MOTION_NAME_ROTATE) &&
			(m_pModuleMotion->GetCurrMotion() != MOTION_NAME_RUN) ) return false;

		if (!IsCurrStanceBattle()) return false;
	}
	else if (nStanceTo == CS_BATTLE)
	{
		if ((m_pModuleMotion->GetCurrMotion() != MOTION_NAME_IDLE) &&
			(m_pModuleMotion->GetCurrMotion() != MOTION_NAME_ROTATE) &&
			(m_pModuleMotion->GetCurrMotion() != MOTION_NAME_RUN) ) return false;

		if (IsCurrStanceBattle()) return false;
	}

	return true;	
}

WEAPON_TYPE XMyPlayer::GetCurrWeaponType()
{
	return gvar.MyInfo.EquipmentSlot.GetCurrWeaponType();
}

void XMyPlayer::SwitchingWeaponSet( int8 nWeaponSet )
{
	// 인첸트 해제
	XItem* pWRItem = gvar.MyInfo.EquipmentSlot.GetCurrentWeaponSetWeaponRight();
	XItem* pWLItem = gvar.MyInfo.EquipmentSlot.GetCurrentWeaponSetWeaponLeft();
	if(pWRItem != NULL)
		EnchantBuffAllLostForUnEquip(pWRItem, GetCurrentRWeaponItemSlot());
	if(pWLItem != NULL)
		EnchantBuffAllLostForUnEquip(pWLItem, GetCurrentLWeaponItemSlot());

	gvar.MyInfo.EquipmentSlot.SwitchingWeaponSet(nWeaponSet);

	// 인첸트 셋팅
	pWRItem = gvar.MyInfo.EquipmentSlot.GetCurrentWeaponSetWeaponRight();
	pWLItem = gvar.MyInfo.EquipmentSlot.GetCurrentWeaponSetWeaponLeft();
	if(pWRItem != NULL)
		EnchantBuffAllGainForEquip(pWRItem, GetCurrentRWeaponItemSlot());
	if(pWLItem != NULL)
		EnchantBuffAllGainForEquip(pWLItem, GetCurrentLWeaponItemSlot());

	// 외형 모델 변경
	if (m_pModuleEntity) 
	{
		m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);
	}

	// 모션 무기 타입 변경
	if (m_pModuleMotion) m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());

	global.ui->WeaponChange();
	gvar.MyInfo.EquipmentSlot.RefreshUI();

	// 테스트
	//m_pModuleEntity->GetActor()->UpdatePartsNode();
}

int8 XMyPlayer::GetWeaponSet()
{
	return gvar.MyInfo.EquipmentSlot.GetWeaponSet();
}

bool XMyPlayer::IsEquipShield()
{
	XItem* pLWeaponItem = NULL;

	if (gvar.MyInfo.EquipmentSlot.GetWeaponSet() == 0)
	{
		pLWeaponItem = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_LWEAPON);
	}
	else
	{
		pLWeaponItem = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_LWEAPON2);
	}

	if (pLWeaponItem && pLWeaponItem->m_pItemData)
	{
		if (CSItemHelper::IsShieldItemType(pLWeaponItem->m_pItemData))
		{
			return true;
		}
	}

	return false;
}

bool XMyPlayer::EquipItem(SH_ITEM_SLOT_TYPE nSlotTypeFrom, int nSlotIDFrom, SH_ITEM_SLOT nSlotIDTo)
{
	XItem* pItem = NULL;

	if (SLOTTYPE_INVENTORY == nSlotTypeFrom)
	{
		// 인벤토리에서 꺼낸다.
		pItem = gvar.MyInfo.Inventory.GetItem(nSlotIDFrom);
		if (pItem == NULL) return false;

		// 착용할 아이템 인벤토리에서 제거
		gvar.MyInfo.Inventory.PopItem(nSlotIDFrom);
	}
	else if (SLOTTYPE_EQUIP == nSlotTypeFrom)
	{
		// 장비창에서 해제
		pItem = gvar.MyInfo.EquipmentSlot.UnEquipItem(static_cast<SH_ITEM_SLOT>(nSlotIDFrom));
		if (pItem == NULL) return false;

		// 장비 인체트 해치
		EnchantBuffAllLostForUnEquip(pItem, (SH_ITEM_SLOT)nSlotIDFrom);
	}
	else
	{
		return false;
	}

	// 아이템 착용
	return EquipItem(pItem, nSlotIDTo);
}

bool XMyPlayer::EquipItem( XItem* pItem, SH_ITEM_SLOT nSlotIDTo )
{
	// 아이템 착용
	gvar.MyInfo.EquipmentSlot.EquipItem(pItem, nSlotIDTo);

	if (global.ui)
	{
		global.ui->InventoryUIRefresh();
		global.ui->WeaponChange();
	}

	m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}

	// 장비 인체트
	EnchantBuffAllGainForEquip(pItem, nSlotIDTo);

	return true;
}

bool XMyPlayer::UnEquipItem( SH_ITEM_SLOT nSlot, int nInvenSlotID, bool bSwapToInven /*= false*/ )
{
	XItem* pCheckItem = gvar.MyInfo.EquipmentSlot.GetItem(static_cast<SH_ITEM_SLOT>(nSlot));
	if(pCheckItem)
	{
		// 장비 인체트 해치
		EnchantBuffAllLostForUnEquip(pCheckItem, SH_ITEM_SLOT(nSlot));
	}

	XItem* pItem = gvar.MyInfo.EquipmentSlot.UnEquipItem(SH_ITEM_SLOT(nSlot));
	if (pItem)
	{
		pItem->m_nSlotID = nInvenSlotID;
		gvar.MyInfo.Inventory.PushItem(nInvenSlotID, pItem);
	}

	if (global.ui)
	{
		global.ui->InventoryUIRefresh();
//		global.ui->WeaponChange();
	}
	
	m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}

	return true;
}


void XMyPlayer::InMyInfo( XGameState* pState )
{
	if (m_pModuleEntity)
	{
		vec3 vPos;
		vec3 vDir;
		if (global.app->GetMode() == EM_LOCAL_TEST)
		{
			vPos = gvar.Network.vStartPos;vPos.z += 1.0f;
			vDir = gvar.Network.vStartDir;

			if (XCONST::SEX_MALE) gvar.MyInfo.ChrInfo.nSex = SEX_MALE;
			else gvar.MyInfo.ChrInfo.nSex = SEX_FEMALE;
		}
		else
		{
			vPos = gvar.World.GetNextMapStartPos();
			vDir = gvar.World.GetNextMapStartDir();
			if (vDir.Length() <= 0.0001f) vDir = vec3(0,-1,0);

			//m_pModuleEntity->SetDirection(vDir);


			// 카메라 방향을 캐릭터 방향으로 설정
			gg.controller->SetBackViewCameraDir(vDir);
			//XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
			//if (pCameraNode)
			//{
			//	pCameraNode->SetDirection(vDir);
			//}
		}

		// 내정보를 가지고 내 엔티티 모습 설정. 이 함수에서 모든 일을 다 한다.
		m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);		

		/*m_pModuleEntity->SetPosition(vPos);
		m_pModuleEntity->SetDirection(dir);*/

 		if(m_pModuleMyControl)
 		{
			m_pModuleMyControl->InitSensorController(pState);
			m_pModuleMyControl->OnEnterGame(vPos, vDir, this);
		}

		m_pModuleEntity->GetAnimationController()->SetAnimation(wstring(L"none_idle"));		

		if (m_pModulePost)
		{
			m_pModulePost->Init(vPos, vDir);
		}

		if (m_pModuleUI)
		{
			m_pModuleUI->SetCharacterPane(wstring(gvar.MyInfo.ChrInfo.szName), gvar.MyInfo.Guild.m_strName, PLAYER_ID_COLOR);
		}

		// fake beaten 애니메이션 컨트롤러
		m_pModuleEntity->CreateFakeBeatenAnimationController(tstring(XCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME), XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT, XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED);

		// 장비 강화
		for ( int i = ITEMSLOT_HEAD;  i < ITEMSLOT_MAX;  i++)
		{
			XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem(static_cast<SH_ITEM_SLOT>(i));
			if ( pItem == NULL)		continue;

			EnchantBuffAllGainForEquip(pItem, SH_ITEM_SLOT(i));
		}	
	}

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(gvar.MyInfo.EquipmentSlot.GetCurrWeaponType());
		m_pModuleMotion->ChangeMotion(MOTION_NAME_IDLE);
	}

	if (m_pModuleCollision)
	{
		wstring strMeshName = XNaming::GetPlayerModelSimpleName(gvar.MyInfo.ChrInfo.nSex);
		CSMeshInfo* pMeshInfo = info.mesh_info->GetInfo(strMeshName);
		m_pModuleCollision->InitCol(pMeshInfo);
	}

	// 컨트롤 초기화
	if(m_pModuleMyControl)
	{
		m_pModuleMyControl->Init_ForChangeField();
	}

	// 탈것 처리 _by tripleJ
	int nID			= GetRideID();
	SetRide( nID );
}

const wchar_t* XMyPlayer::GetName()
{
	return gvar.MyInfo.ChrInfo.szName;
}

SEX XMyPlayer::GetSex()
{
	return gvar.MyInfo.ChrInfo.nSex;
}

void XMyPlayer::SafeStop()
{
	// 키 버퍼 삭제
	gg.controller->ClearKeyBuffer();


	// 정지 예약
	GetModuleMotion()->PushMotion( MOTION_NAME_IDLE);


	// 달리는 중이면 바로 정지(오토런일 때는 예외)
	if ( (GetModuleAction()->GetCurrentMotionName() == MOTION_NAME_RUN) &&
		 (GetModuleMyControl()->IsAutoRun() == false))
		GetModuleAction()->Stop();
}

ARMOR_TYPE XMyPlayer::GetArmorType()
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_BODY);
	if (pItem == NULL) return ARMOR_NONE;

	return pItem->m_pItemData->m_nArmorType;
}

//필드 입장시 HP가 0이면 Die 로 처리
bool XMyPlayer::CheckDieToHP()
{
	if (0 >= gvar.MyInfo.GetHP())
	{
		OnDie();
		DeadProcEnterField();

		return true;
	}

	return false;
}

void XMyPlayer::OutMyInfo()
{
	// 내 버프 정보 삭제
	if(m_pModuleBuff)
		m_pModuleBuff->MyBuffAllDelete();

	gvar.MyInfo.BuffList.Clear();
}

void XMyPlayer::SetMyTimeInfo( const vector<TD_BUFF_REMAIN_TIME>& vecTDBuffRemainTime, const vector<TD_TALENT_REMAIN_COOLTIME>& vecTDTalentRemainCoolTime, float fAutoRebirthRemainTime )
{
	// 버프 남은 시간
	// 버프를 날리고 새로 입력한다.
	m_pModuleBuff->MyBuffAllDelete();
	for each (TD_BUFF_REMAIN_TIME tdBuffRemainTime in vecTDBuffRemainTime)
	{
		m_pModuleBuff->SetRemainBuffList(tdBuffRemainTime.nBuffID, tdBuffRemainTime.fRemainTime);
	}

	// 탤런트 쿨타임
	gvar.MyInfo.Talent.CoolTime.Clear();
	for each (TD_TALENT_REMAIN_COOLTIME cooltime  in vecTDTalentRemainCoolTime)
	{
		gvar.MyInfo.Talent.CoolTime.SetRemained(cooltime.nTalentID, cooltime.fRemainCoolTime);
	}

	gvar.MyInfo.fAutoRebirthRemainCoolTime = fAutoRebirthRemainTime;
}

void XMyPlayer::ChangeUIID( UIID nNewUIID )
{
	m_nUIID = nNewUIID;
}

void XMyPlayer::GetWeaponParentName( wstring& strRWeapon, wstring& strLWeapon )
{
	XItem* pRWeaponItem = NULL;

	if (gvar.MyInfo.EquipmentSlot.GetWeaponSet() == 0)
	{
		pRWeaponItem = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_RWEAPON);
	}
	else
	{
		pRWeaponItem = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_RWEAPON2);
	}

	if(pRWeaponItem)
	{
		strRWeapon = pRWeaponItem->m_pItemData->m_strParentName;
		strLWeapon = pRWeaponItem->m_pItemData->m_strParentNameLeft;
	}
}

void XMyPlayer::EnchantDone( bool bEquipSlot, int nItemSlot, int nEnchantSlot, int nEnchantItemID )
{
	XItem* pItem = NULL;
	if(bEquipSlot)
	{
		pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)nItemSlot);
	}
	else
	{
		pItem = gvar.MyInfo.Inventory.GetItem(nItemSlot);
	}

	if(pItem == NULL)
		return;

	// 귀속처리
	pItem->m_bClaimed = true;

	XEvent evt;
	evt.m_nID = XEVTL_ENCHANT_RESULT;
	evt.m_pData = &(pItem->m_pItemData->m_nID);
	evt.m_pResult = &nEnchantItemID;
	Event(evt);

	//////////////////////////////////////////////////////////////////////////
	// 강화
	if(bEquipSlot)
		EnchantBuffLost(pItem, (SH_ITEM_SLOT)nItemSlot, nEnchantSlot);

	pItem->m_nEnchants[nEnchantSlot] = nEnchantItemID;

	if(bEquipSlot)
	{
		EnchantBuffGain(pItem, (SH_ITEM_SLOT)nItemSlot, nEnchantSlot);
		gvar.MyInfo.EquipmentSlot.RefreshUI();
	}
}

void XMyPlayer::EnchantBuffGain( XItem* pItem, SH_ITEM_SLOT nItemSlot, int nEnchantSlot )
{
	if(m_pModuleBuff && CheckEnchantBuffGinEnable(nItemSlot) && pItem->m_nEnchants[nEnchantSlot] != ENCHANT_UNUSED_SLOT)
	{
		bool bShowEffect = CheckEnchantBuffEffectShow(pItem, nItemSlot);
		int nPartsSlotType = XPartsSlotType::Slot2LookSlot(nItemSlot);
		m_pModuleBuff->BuffEnchantGain(info.item->GetEnchantItemBuffID(pItem, nEnchantSlot), bShowEffect, nPartsSlotType);
	}
}

void XMyPlayer::EnchantBuffLost( XItem* pItem, SH_ITEM_SLOT nItemSlot, int nEnchantSlot )
{
	if(m_pModuleBuff && CheckEnchantBuffGinEnable(nItemSlot) && pItem->m_nEnchants[nEnchantSlot] != ENCHANT_UNUSED_SLOT)
	{
		bool bHideEffect = CheckEnchantBuffEffectShow(pItem, nItemSlot);
		int nPartsSlotType = XPartsSlotType::Slot2LookSlot(nItemSlot);
		m_pModuleBuff->BuffEnchantLost(info.item->GetEnchantItemBuffID(pItem, nEnchantSlot), bHideEffect, nPartsSlotType);
	}
}

void XMyPlayer::EnchantBuffAllGainForEquip( XItem* pItem, SH_ITEM_SLOT nItemSlot )
{
	if(m_pModuleBuff && CheckEnchantBuffGinEnable(nItemSlot))
	{
		for(int iEnchant = 0; iEnchant < ENCHANT_MAX_COUNT; ++iEnchant)
		{
			if(pItem->m_nEnchants[iEnchant] != ENCHANT_UNUSED_SLOT)
			{
				bool bShowEffect = CheckEnchantBuffEffectShow(pItem, nItemSlot);
				int nPartsSlotType = XPartsSlotType::Slot2LookSlot(nItemSlot);
				m_pModuleBuff->BuffEnchantGain(info.item->GetEnchantItemBuffID(pItem, iEnchant), bShowEffect, nPartsSlotType);
			}
		}
	}
}

void XMyPlayer::EnchantBuffAllLostForUnEquip( XItem* pItem, SH_ITEM_SLOT nItemSlot )
{
	if(m_pModuleBuff && CheckEnchantBuffGinEnable(nItemSlot))
	{
		for(int iEnchant = 0; iEnchant < ENCHANT_MAX_COUNT; ++iEnchant)
		{
			if(pItem->m_nEnchants[iEnchant] != ENCHANT_UNUSED_SLOT)
			{
				bool bHideEffect = CheckEnchantBuffEffectShow(pItem, nItemSlot);
				int nPartsSlotType = XPartsSlotType::Slot2LookSlot(nItemSlot);
				m_pModuleBuff->BuffEnchantLost(info.item->GetEnchantItemBuffID(pItem, iEnchant), bHideEffect, nPartsSlotType);
			}
		}
	}
}

bool XMyPlayer::CheckEnchantBuffGinEnable( SH_ITEM_SLOT nItemSlot )
{
	SH_ITEM_SLOT eRWeaponSet = ITEMSLOT_LWEAPON2;
	SH_ITEM_SLOT eLWeaponSet = ITEMSLOT_RWEAPON2;

	if(gvar.MyInfo.EquipmentSlot.GetWeaponSet() == 1)
	{
		eRWeaponSet = ITEMSLOT_RWEAPON;
		eLWeaponSet = ITEMSLOT_LWEAPON;
	}
	
	if(nItemSlot <= ITEMSLOT_DEFAULT_MAX)
	{
		if(nItemSlot == eRWeaponSet ||
			nItemSlot == eLWeaponSet)
			return false;

		return true;
	}

	return false;
}

SH_ITEM_SLOT XMyPlayer::GetCurrentRWeaponItemSlot()
{
	if(gvar.MyInfo.EquipmentSlot.GetWeaponSet() == 0)
		return ITEMSLOT_RWEAPON;

	return ITEMSLOT_RWEAPON2;
}

SH_ITEM_SLOT XMyPlayer::GetCurrentLWeaponItemSlot()
{
	if(gvar.MyInfo.EquipmentSlot.GetWeaponSet() == 0)
		return ITEMSLOT_LWEAPON;

	return ITEMSLOT_LWEAPON2;
}

bool XMyPlayer::CheckEnchantBuffEffectShow( XItem* pItem, SH_ITEM_SLOT nSlotID )
{
	if(nSlotID == ITEMSLOT_RWEAPON ||
		nSlotID == ITEMSLOT_RWEAPON2 ||
		nSlotID == ITEMSLOT_LWEAPON ||
		nSlotID == ITEMSLOT_LWEAPON2)
	{
		// 이펙트는 프록만 나온다.
		if(pItem->m_pItemData->m_vecEquipEnchantColor.size() > 0 &&
			pItem->m_pItemData->m_vecEquipEnchantColor[0] == ENCHANT_NONE)
			return false;

		return true;
	}

	return false;
}

int XMyPlayer::GetVisualEnchantBuffID()
{
	XItem* pWRItem = gvar.MyInfo.EquipmentSlot.GetCurrentWeaponSetWeaponRight();
	if(pWRItem == NULL)
		return 0;

	// 비쥬얼은 0번째이다.
	return info.item->GetEnchantItemBuffID(pWRItem, 0);
}

bool XMyPlayer::OnDyeEquipItem( SH_ITEM_SLOT_TYPE nSlotType, int nSlotID, int nColor )
{
	XItem * pItem = NULL;

	if (SLOTTYPE_INVENTORY == nSlotType)
	{
		pItem = gvar.MyInfo.Inventory.GetItem(nSlotID);
	}
	else if (SLOTTYPE_EQUIP == nSlotType)
	{
		pItem = gvar.MyInfo.EquipmentSlot.GetItem(static_cast<SH_ITEM_SLOT>(nSlotID));
	}

	if(pItem == NULL)	return false;

	pItem->m_nDyedColor = nColor;

	if (SLOTTYPE_EQUIP == nSlotType)
	{
		unsigned int nDyedEquipColor = info.colorTable->GetColor(ITEM_DYED_COLOR_NAME, nColor).GetARGB();
		unsigned int nDyedSkinColor = info.colorTable->GetColor(L"skin", gvar.MyInfo.ChrInfo.nFeatureSkinColor).GetARGB();
		for(vector<tstring>::iterator it = pItem->m_pItemData->m_strNodeNames.begin(); it != pItem->m_pItemData->m_strNodeNames.end(); ++it)
		{
			unsigned int nDyedColor = nDyedEquipColor;
			if (XNaming::IsSkinNodeName(it->c_str()))
				nDyedColor = nDyedSkinColor;

			gvar.Game.pMyPlayer->GetActor()->SetColor(nDyedColor, it->c_str());
		}
	}

	return true;
}

WEAPON_TYPE XMyPlayer::GetEquipWeaponType( bool bLeft )
{
	XItemData* pItemDataR = NULL;
	XItemData* pItemDataL = NULL;

	gvar.MyInfo.EquipmentSlot.GetCurrWeapon(&pItemDataR, & pItemDataL);

	if(bLeft && pItemDataL)
		return pItemDataL->m_nWeaponType;
	else if(pItemDataR)
		return pItemDataR->m_nWeaponType;

	return WEAPON_NONE;
}

void XMyPlayer::SetCharacterPane_Player()
{
	wstring strName = GetCaptionName();
	m_pModuleUI->SetCharacterPane(strName, gvar.MyInfo.Guild.m_strName, PLAYER_ID_COLOR);
}

void XMyPlayer::GetCurrentWeaponSetWeaponItemData( XItemData** pItemDataR, XItemData** pItemDataL )
{
	XItemData* pItemR = NULL;
	XItemData* pItemL = NULL;

	gvar.MyInfo.EquipmentSlot.GetCurrWeapon(&pItemR, &pItemL);

	*pItemDataR = pItemR;
	*pItemDataL = pItemL;
}

XItem* XMyPlayer::RemoveEquipItem( SH_ITEM_SLOT nSlot )
{
	XItem* pCheckItem = gvar.MyInfo.EquipmentSlot.GetItem(static_cast<SH_ITEM_SLOT>(nSlot));
	if(pCheckItem == NULL) return NULL;

	// 장비 인체트 해치
	EnchantBuffAllLostForUnEquip(pCheckItem, SH_ITEM_SLOT(nSlot));

	gvar.MyInfo.EquipmentSlot.UnEquipItem(SH_ITEM_SLOT(nSlot));

	m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}

	return pCheckItem;
}

void XMyPlayer::CheckDie()
{
	if (CheckDieToHP())
	{
		GetModuleMyControl()->OnDie();
	}
}

void XMyPlayer::EquipOverlapped( int nItemID )
{
	XItem* pNewItem = info.item->MakeNewItem(nItemID);
	if(pNewItem == NULL)
		return;

	gvar.MyInfo.EquipmentSlot.EquipOverlappedItem(pNewItem);

	if (global.ui)
	{
		global.ui->InventoryUIRefresh();
		global.ui->WeaponChange();
	}

	m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}

	// 장비 인체트
	EnchantBuffAllGainForEquip(pNewItem, pNewItem->m_pItemData->m_nItemSlot);

	XPlayer::EquipOverlapped(nItemID);
}

void XMyPlayer::UnEquipOverlapped(int nItemID)
{
	CSItemData* pItemData = info.item->GetItemData(nItemID);
	if(pItemData == NULL)
		return;

	XItem* pOverlappedItem = gvar.MyInfo.EquipmentSlot.UnEquipOverlappedItem(pItemData);
	if(pOverlappedItem)
		info.item->DelItem(pOverlappedItem);

	if (global.ui)
	{
		global.ui->InventoryUIRefresh();
	}

	m_pModuleEntity->SerializeMyPlayer(gvar.MyInfo.ChrInfo, &gvar.MyInfo.EquipmentSlot);

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeWeaponAniType(GetCurrWeaponType());
	}

	XPlayer::UnEquipOverlapped(nItemID);
}