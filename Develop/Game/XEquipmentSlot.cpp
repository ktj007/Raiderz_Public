#include "stdafx.h"
#include "XEquipmentSlot.h"
#include "XMyPlayer.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XGameFrameManager.h"
#include "CSItemHelper.h"

XMyEquipmentSlot::XMyEquipmentSlot()
{
}

XItem* XMyEquipmentSlot::GetItem(SH_ITEM_SLOT nSlot)
{
	return m_EquipmentSlot.GetItem(nSlot);
}

int XMyEquipmentSlot::GetEquipSlotID(const wchar_t* slot)
{
	if (slot == NULL) return ITEMSLOT_NONE;

	for (int i=0; i<(int)ITEMSLOT_MAX; i++)
	{
		if (_wcsicmp(slot, CSItemSlotStr[i]) == 0)
		{
			return i;
		}
	}

	_ASSERT(0 && "슬롯이름이 잘못되었음.");
	return ITEMSLOT_NONE;
}

XItem* XMyEquipmentSlot::GetItem(const wchar_t* slot)
{
	int nEquipSlotID = GetEquipSlotID(slot);
	return GetItem((SH_ITEM_SLOT)nEquipSlotID);
}

const wchar_t* XMyEquipmentSlot::GetItemName(const wchar_t* slot)
{
	int nSlotID = GetEquipSlotID(slot);
	return GetItemName(nSlotID);
}

const wchar_t* XMyEquipmentSlot::GetItemName( int nSlot )
{
	XItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(nSlot));
	if( pItem == NULL) return NULL;

	return pItem->m_pItemData->GetName();
}

const wchar_t* XMyEquipmentSlot::GetItemIconName(const wchar_t* slot)
{
	int nSlotID = GetEquipSlotID(slot);
	return GetItemIconName(nSlotID);
}

const wchar_t* XMyEquipmentSlot::GetItemIconName(int nSlot)
{
	XItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(nSlot));
	if( pItem == NULL) return NULL;

	XItemData* pItemData = pItem->m_pItemData;
	if ( pItemData == NULL)		return L"";

	return pItemData->m_strIconName.c_str();
}

void XMyEquipmentSlot::RefreshUI()
{
	if ((global.gfmgr == NULL) || (global.gfmgr->GetCurrFrameID() != PLAY_FRAME_ID)) return;

	if (global.script)
		global.script->GetGlueGameEvent().OnGameEvent( "CHARINFO", "EQUIPPEDITEM");
}

WEAPON_TYPE XMyEquipmentSlot::GetCurrWeaponType()
{
	CSItemData* pItemDataR = NULL;
	CSItemData* pItemDataL = NULL;

	switch (m_EquipmentSlot.GetWeaponSet())
	{
	case 0: // 주무기
		{
			XItem* pItemR = GetItem(ITEMSLOT_RWEAPON);
			XItem* pItemL = GetItem(ITEMSLOT_LWEAPON);
			if (pItemR) pItemDataR = pItemR->m_pItemData;
			if (pItemL) pItemDataL = pItemL->m_pItemData;
		}break;
	case 1: // 보조무기
		{
			XItem* pItemR = GetItem(ITEMSLOT_RWEAPON2);
			XItem* pItemL = GetItem(ITEMSLOT_LWEAPON2);
			if (pItemR) pItemDataR = pItemR->m_pItemData;
			if (pItemL) pItemDataL = pItemL->m_pItemData;
		}break;
	default:
		_ASSERT(0);
	}

	return CSItemHelper::GetCurrentWeaponType(pItemDataR, pItemDataL);
}

void XMyEquipmentSlot::GetCurrWeapon( XItemData** pItemDataR, XItemData** pItemDataL )
{
	switch (m_EquipmentSlot.GetWeaponSet())
	{
	case 0: // 주무기
		{
			XItem* pItemR = GetItem(ITEMSLOT_RWEAPON);
			XItem* pItemL = GetItem(ITEMSLOT_LWEAPON);
			if (pItemR) *pItemDataR = pItemR->m_pItemData;
			if (pItemL) *pItemDataL = pItemL->m_pItemData;
		}break;
	case 1: // 보조무기
		{
			XItem* pItemR = GetItem(ITEMSLOT_RWEAPON2);
			XItem* pItemL = GetItem(ITEMSLOT_LWEAPON2);
			if (pItemR) *pItemDataR = pItemR->m_pItemData;
			if (pItemL) *pItemDataL = pItemL->m_pItemData;
		}break;
	default:
		_ASSERT(0);
	}
}

void XMyEquipmentSlot::ChangeItemAttr(XItem* pItem)
{
	pItem->m_bNewItem = false;

	ClaimRequire(pItem);
}

// 착귀아이템이 장착된것이면 귀속으로 설정.
void XMyEquipmentSlot::ClaimRequire(XItem* pItem)
{
	if( pItem != NULL)
	{
		CSItemData* pItemData = pItem->m_pItemData;
		if( pItemData != NULL)
		{
			if (pItemData->m_bClaimRequired &&
				(pItem->m_bClaimed == false) )
				pItem->m_bClaimed = true;
		}
	}
}

void XMyEquipmentSlot::EquipItem( XItem* pItem, SH_ITEM_SLOT nSlot )
{
	assert(pItem && pItem->m_pItemData);

	m_EquipmentSlot.EquipItem(pItem, nSlot);

	ChangeItemAttr(pItem);
}

XItem* XMyEquipmentSlot::UnEquipItem(SH_ITEM_SLOT nSlot)
{
	XItem* pItem = static_cast<XItem*>(m_EquipmentSlot.RemoveItem(nSlot));
	return pItem;
}

bool XMyEquipmentSlot::IsEquipItem(int nItemID)
{
	for(int i = 0; i < (int)ITEMSLOT_MAX; i++)
	{
		XItem* pItem = GetItem((SH_ITEM_SLOT)i);
		if (pItem && (pItem->m_pItemData->m_nID == nItemID))
			return true;
	}

	return false;
}

bool XMyEquipmentSlot::IsEquipShield()
{
	XItem* pItemL = GetItem(ITEMSLOT_LWEAPON);
	CSItemData* pItemDataL = NULL;
	if (pItemL) pItemDataL = pItemL->m_pItemData;

	return (pItemDataL && pItemDataL->m_ItemType == ITEMTYPE_ARMOR);
}

int XMyEquipmentSlot::GetItemID( SH_ITEM_SLOT nSlot )
{
	XItem* pItem = GetItem(nSlot);
	if (pItem && pItem->m_pItemData)
	{
		return pItem->m_pItemData->m_nID;
	}
	return 0;
}

MUID XMyEquipmentSlot::GetItemUID( SH_ITEM_SLOT nSlot )
{
	XItem* pItem = GetItem(nSlot);
	if (pItem && pItem->m_pItemData)
	{
		return pItem->m_nAmount;
	}
	return MUID::ZERO;
}

vector<XItem*> XMyEquipmentSlot::GetAllItem()
{
	vector<XItem*> vecItem;
	for ( int i = ITEMSLOT_HEAD;  i < ITEMSLOT_MAX;  i++)
	{
		XItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if ( pItem == NULL)		continue;

		vecItem.push_back(pItem);
	}	

	return vecItem;
}

float XMyEquipmentSlot::GetSumOfCraftOptionRate()
{
	float fSum = 0;
	for (int i = ITEMSLOT_HEAD; ITEMSLOT_MAX > i; i++)
	{
		XItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if (NULL == pItem) continue;

		fSum += pItem->m_pItemData->m_fCraftOptionRate;
	}

	return fSum;
}

void XMyEquipmentSlot::Clear()
{
	m_EquipmentSlot.Clear();	
}

XItem* XMyEquipmentSlot::GetCurrentWeaponSetWeaponRight()
{
	if (GetWeaponSet() == 0)
	{
		return GetItem(ITEMSLOT_RWEAPON);
	}
	return GetItem(ITEMSLOT_RWEAPON2);
}

XItem* XMyEquipmentSlot::GetCurrentWeaponSetWeaponLeft()
{
	if (GetWeaponSet() == 0)
	{
		return GetItem(ITEMSLOT_LWEAPON);
	}
	return GetItem(ITEMSLOT_LWEAPON2);
}

bool XMyEquipmentSlot::IsEquiped( SH_ITEM_SLOT nSlot )
{
	if (NULL == GetItem(nSlot)) return false;

	return true;
}

int XMyEquipmentSlot::GetAP()
{
	int _ap = 0;
	for ( int i = ITEMSLOT_HEAD;  i < ITEMSLOT_MAX;  i++)
	{
		XItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if (NULL == pItem)		continue;
		XItemData* pItemData = pItem->m_pItemData;
		if (NULL == pItemData) continue;

		if (0 < pItemData->m_nMaxDurability && 0 >= pItem->m_nDurability) continue;

		if ((ITEMSLOT_LWEAPON == i || ITEMSLOT_RWEAPON == i) && 
			ITEMSWITCH_PRIMARY != m_EquipmentSlot.GetWeaponSet()) continue;

		if ((ITEMSLOT_LWEAPON2 == i || ITEMSLOT_RWEAPON2 == i) && 
			ITEMSWITCH_SECONDARY != m_EquipmentSlot.GetWeaponSet()) continue;

		if (!m_EquipmentSlot.CheckHasPassiveTalent(pItem)) continue;

		_ap += pItem->m_pItemData->m_nAP;
	}
	return _ap;
}

XItem* XMyEquipmentSlot::GetRandomItem()
{
	vector<XItem*> vecItem = GetAllItem();
	if (vecItem.empty()) return NULL;

	int nIndex = RandomNumber(0, vecItem.size()-1);
	XItem* pItem = vecItem[nIndex];

	return pItem;
}

void XMyEquipmentSlot::SwitchingWeaponSet( int8 nWeaponSet )
{
	m_EquipmentSlot.SwitchingWeaponSet(nWeaponSet);
}

int XMyEquipmentSlot::GetItemQuantityByID(int nID)
{
	int nCount = 0;
	for(int i = ITEMSLOT_HEAD; i < (int)ITEMSLOT_MAX; i++)
	{
		XItem* pItem = GetItem((SH_ITEM_SLOT)i);
		if ( pItem == NULL)		continue;
		if ( pItem->m_pItemData->m_nID != nID) continue;
		nCount++;
	}

	return nCount;
}

XItem* XMyEquipmentSlot::GetItemByID( int nID )
{
	for(int i = ITEMSLOT_HEAD; i < (int)ITEMSLOT_MAX; i++)
	{
		XItem* pItem = GetItem((SH_ITEM_SLOT)i);
		if (pItem == NULL)						continue;
		if (pItem->m_pItemData->m_nID != nID)	continue;

		return pItem;
	}

	return NULL;
}

void XMyEquipmentSlot::EquipOverlappedItem( XItem* pNewItem )
{
	SH_ITEM_SLOT nSlot = pNewItem->m_pItemData->m_nItemSlot;
	SH_ITEM_SLOT nSubSlot = pNewItem->m_pItemData->m_nSubSlot;

	if(GetWeaponSet() == 1)
	{
		if(nSlot == ITEMSLOT_RWEAPON)
			nSlot = ITEMSLOT_RWEAPON2;
		else if(nSlot == ITEMSLOT_LWEAPON)
			nSlot = ITEMSLOT_LWEAPON2;
		if(nSubSlot == ITEMSLOT_LWEAPON)
			nSubSlot = ITEMSLOT_LWEAPON2;
	}

	if(nSubSlot != ITEMSLOT_NONE)
	{
		// 서브 슬롯이 있다면... 그 슬롯도 저장한다.

		XItem* pSubItem = GetItem(nSubSlot);
		if(pSubItem)
		{
			m_OverlappedEquipmentSlot.EquipItem(pSubItem, nSubSlot);
		}
	}

	XItem* pItem = GetItem(nSlot);
	if(pItem)
	{
		m_OverlappedEquipmentSlot.EquipItem(pItem, nSlot);
	}

	EquipItem(pNewItem, nSlot);
}

XItem* XMyEquipmentSlot::UnEquipOverlappedItem( CSItemData* pTempItem )
{
	// 오버랩꺼 삭제하고...
	SH_ITEM_SLOT nSlot = pTempItem->m_nItemSlot;
	SH_ITEM_SLOT nSubSlot = pTempItem->m_nSubSlot;

	if(GetWeaponSet() == 1)
	{
		if(nSlot == ITEMSLOT_RWEAPON)
			nSlot = ITEMSLOT_RWEAPON2;
		else if(nSlot == ITEMSLOT_LWEAPON)
			nSlot = ITEMSLOT_LWEAPON2;
		if(nSubSlot == ITEMSLOT_LWEAPON)
			nSubSlot = ITEMSLOT_LWEAPON2;
	}

	XItem* pItem = GetItem(nSlot);

	XItem* pOverlappedItem = m_OverlappedEquipmentSlot.RemoveItem(nSlot);
	XItem* pOverlappedSubItem = m_OverlappedEquipmentSlot.RemoveItem(nSubSlot);

	// 이전거 다시 입력
	if(pOverlappedItem)				EquipItem(pOverlappedItem, nSlot);
	else							UnEquipItem(nSlot);

	if(pOverlappedSubItem)			EquipItem(pOverlappedSubItem, nSubSlot);
	else							UnEquipItem(nSubSlot);

	return pItem;
}

bool XMyEquipmentSlot::IsOverlappedItemActive()
{
	return m_OverlappedEquipmentSlot.GetActive();
}

bool XMyEquipmentSlot::IsEquipOverlappedItem( int nItemID )
{
	for(int i = 0; i < (int)ITEMSLOT_MAX; i++)
	{
		XItem* pItem = m_OverlappedEquipmentSlot.GetItem((SH_ITEM_SLOT)i);
		if (pItem && (pItem->m_pItemData->m_nID == nItemID))
			return true;
	}

	return false;
}

void XMyEquipmentSlot::AddModifierByTrain( TALENT_EXTRA_PASSIVE_TYPE nTrainTEPT )
{
	m_EquipmentSlot.AddModifierByTrain(nTrainTEPT);
}

void XMyEquipmentSlot::EraseModifierByUntrain()
{
	m_EquipmentSlot.EraseModifierByUntrainAll();
}

//////////////////////////////////////////////////////////////////////////



void XEquipmentSlot::EquipItem( XItem* pItem, SH_ITEM_SLOT nSlot )
{
	_ASSERT(pItem);

	if (GetItem(nSlot))			RemoveItem(nSlot);
	SetItem(nSlot, pItem);

	if ((CSItemHelper::IsPrimaryWeaponSlot(nSlot) && ITEMSWITCH_PRIMARY != m_nWeaponSet) ||
		(CSItemHelper::IsSecondaryWeaponSlot(nSlot) && ITEMSWITCH_SECONDARY != m_nWeaponSet))
	{
		return;
	}
	
	// 아이템 보정 적용
	AddModifierByEquip(pItem);
}

XItem* XEquipmentSlot::RemoveItem( SH_ITEM_SLOT nSlot )
{
	if (0 > nSlot)				return NULL;
	if (ITEMSLOT_MAX <= nSlot)	return NULL;

	XItem* pItem = m_pItem[nSlot];
	if (!pItem) return NULL;
	SetItem(nSlot, NULL);

	if ((CSItemHelper::IsPrimaryWeaponSlot(nSlot) && ITEMSWITCH_PRIMARY != m_nWeaponSet) ||
		(CSItemHelper::IsSecondaryWeaponSlot(nSlot) && ITEMSWITCH_SECONDARY != m_nWeaponSet))
	{
		return pItem;
	}
	
	// 아이템 보정 해제	
	EraseModifierByUnequip(pItem);	

	return pItem;
}

XItem* XEquipmentSlot::GetItem( SH_ITEM_SLOT nSlot ) const
{
	if (0 > nSlot)				return NULL;
	if (ITEMSLOT_MAX <= nSlot)	return NULL;

	return m_pItem[nSlot];
}

void XEquipmentSlot::Clear()
{
	for (int i=0; i<ITEMSLOT_MAX; i++)
		SetItem(SH_ITEM_SLOT(i),  NULL);

	m_nWeaponSet = 0;
}

void XEquipmentSlot::SetItem( SH_ITEM_SLOT nSlot, XItem* pItem )
{
	if (nSlot < 0 || nSlot >= ITEMSLOT_MAX)				return;

	m_pItem[nSlot] = pItem;

	if (pItem != NULL)
	{
		pItem->m_nSlotID = nSlot;
	}
}

XEquipmentSlot::XEquipmentSlot()
: m_nWeaponSet(0)
{
	Clear();
}

void XEquipmentSlot::EraseModifierByUnequip( XItem* pItem )
{
	if (NULL == pItem) return;
	if (!CheckHasPassiveTalent(pItem)) return;

	EraseModifier(pItem);

}

void XEquipmentSlot::AddModifierByEquip( XItem* pItem )
{
	if (NULL == pItem) return;
	if (!CheckHasPassiveTalent(pItem)) return;

	AddModifier(pItem);
}

void XEquipmentSlot::SwitchingWeaponSet(int8 nWeaponSet)
{
	if (nWeaponSet == m_nWeaponSet) return;

	if (ITEMSWITCH_PRIMARY == nWeaponSet)
	{
		XItem* pRWeapon1 = GetItem(ITEMSLOT_RWEAPON);
		XItem* pLWeapon1 = GetItem(ITEMSLOT_LWEAPON);
		XItem* pRWeapon2 = GetItem(ITEMSLOT_RWEAPON2);
		XItem* pLWeapon2 = GetItem(ITEMSLOT_LWEAPON2);

		AddModifierByEquip(pRWeapon1);
		AddModifierByEquip(pLWeapon1);
		EraseModifierByUnequip(pRWeapon2);
		EraseModifierByUnequip(pLWeapon2);
	}
	else
	{
		XItem* pRWeapon1 = GetItem(ITEMSLOT_RWEAPON);
		XItem* pLWeapon1 = GetItem(ITEMSLOT_LWEAPON);
		XItem* pRWeapon2 = GetItem(ITEMSLOT_RWEAPON2);
		XItem* pLWeapon2 = GetItem(ITEMSLOT_LWEAPON2);

		EraseModifierByUnequip(pRWeapon1);
		EraseModifierByUnequip(pLWeapon1);
		AddModifierByEquip(pRWeapon2);
		AddModifierByEquip(pLWeapon2);
	}

	SetWeaponSet(nWeaponSet);
}

bool XEquipmentSlot::CheckHasPassiveTalent( XItem* pItem )
{
	if (NULL == pItem) return false;

	TALENT_EXTRA_PASSIVE_TYPE nNeedTEPT = CSItemHelper::GetTEPTForEquip(pItem->m_pItemData);
	if (nNeedTEPT == TEPT_NONE)
		return true;

	if (!CSItemHelper::IsMustCheckPassiveType(nNeedTEPT))
		return true;

	if (!gvar.MyInfo.Talent.HasExtraPassiveTalent(nNeedTEPT))
		return false;

	return true;
}

void XEquipmentSlot::AddModifierByTrain( TALENT_EXTRA_PASSIVE_TYPE nTrainTEPT )
{
	if (TEPT_NONE == nTrainTEPT) return;

	for (int i=0; i<ITEMSLOT_MAX; i++)
	{
		SH_ITEM_SLOT nSlot = static_cast<SH_ITEM_SLOT>(i);
		XItem* pItem = GetItem(nSlot);
		if (pItem == NULL)
			continue;
		if (nTrainTEPT != CSItemHelper::GetTEPTForEquip(pItem->m_pItemData))
			continue;

		AddModifier(pItem);
	}
}

void XEquipmentSlot::EraseModifierByUntrainAll()
{
	for (int i=0; i<ITEMSLOT_MAX; i++)
	{
		SH_ITEM_SLOT nSlot = static_cast<SH_ITEM_SLOT>(i);
		XItem* pItem = GetItem(nSlot);
		if (pItem == NULL)
			continue;
		if (CheckHasPassiveTalent(pItem))
			continue;

		EraseModifier(pItem);
	}
}

void XEquipmentSlot::AddModifier( XItem* pItem )
{
	m_ModEffector.AddActorModifier(gvar.MyInfo.ChrStatus.ActorModifier, pItem->m_pItemData->m_ActorModifier);
	m_ModEffector.AddActorModifier(gvar.MyInfo.ActorModifier_ExceptTalent, pItem->m_pItemData->m_ActorModifier);
	m_ModEffector.AddPlayerModifier(gvar.MyInfo.PlayerModifier, pItem->m_pItemData->m_PlayerModifier);
	m_ModEffector.AddPlayerModifier(gvar.MyInfo.PlayerModifier_ExceptTalent, pItem->m_pItemData->m_PlayerModifier);
	m_ModEffector.RefreshStatus();
}

void XEquipmentSlot::EraseModifier( XItem* pItem )
{
	m_ModEffector.EraseActorModifier(gvar.MyInfo.ChrStatus.ActorModifier, pItem->m_pItemData->m_ActorModifier);
	m_ModEffector.EraseActorModifier(gvar.MyInfo.ActorModifier_ExceptTalent, pItem->m_pItemData->m_ActorModifier);
	m_ModEffector.ErasePlayerModifier(gvar.MyInfo.PlayerModifier, pItem->m_pItemData->m_PlayerModifier);
	m_ModEffector.ErasePlayerModifier(gvar.MyInfo.PlayerModifier_ExceptTalent, pItem->m_pItemData->m_PlayerModifier);
	m_ModEffector.RefreshStatus();
}

//////////////////////////////////////////////////////////////////////////
XOverlappedEquipmentSlot::XOverlappedEquipmentSlot()
: m_bActive(false)
{

}

void XOverlappedEquipmentSlot::EquipItem( XItem* pItem, SH_ITEM_SLOT nSlot )
{
	m_bActive = true;

	XEquipmentSlot::EquipItem(pItem, nSlot);
}

XItem* XOverlappedEquipmentSlot::RemoveItem( SH_ITEM_SLOT nSlot )
{
	XItem* pItem = XEquipmentSlot::RemoveItem(nSlot);

	// 아직 아이템이 존재하는지... 체크
	for (int i=0; i<ITEMSLOT_MAX; i++)
	{
		if(GetItem(SH_ITEM_SLOT(i)) != NULL)
			return pItem;
	}

	// 아이템이 없으면... 자동으로 끈다.
	m_bActive = false;

	return pItem;
}
