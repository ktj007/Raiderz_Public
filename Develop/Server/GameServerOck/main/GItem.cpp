#include "stdafx.h"
#include "GItem.h"
#include "CSItemManager.h"
#include "CTransData.h"
#include "GCalculator.h"
#include "GBuffInfo.h"
#include "STransData_M2G.h"

const int8 FAILED = -1;
const int8 ACTIVE_SLOT = 0;

GItem::GItem() 
: m_uidID(MUID::Invalid())
, m_pItemData(NULL)
, m_nAmount(1)						// 갯수는 기본으로 1개.
, m_nDurability(0)
, m_nIUID(0)
, m_bEquiped(false)
, m_nDyedColor(0)
, m_bBind(false)
, m_nSoulCount(0)
, m_nSlotID(0)
, m_nSlotType(SLOTTYPE_NONE)
, m_strExpiDt(L"NULL")
{
	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		m_nEnchants[i] = ENCHANT_UNUSED_SLOT;
	}
}

GItem::~GItem()
{
}

void GItem::Set(const MUID& id, GItemData* dat)
{
	m_bEquiped = false;
	m_uidID = id;
	m_pItemData = dat;
	m_nDurability = m_pItemData->m_nMaxDurability;
}

TD_ITEM GItem::ToTD_ITEM()
{
	TD_ITEM tdItem;
	tdItem.m_nDurability = this->m_nDurability;	
	tdItem.m_nQuantity = this->m_nAmount;	
	tdItem.m_bBind = this->m_bBind;
	tdItem.m_nDyedColor = this->m_nDyedColor;
	tdItem.m_nSoulQuantity = this->m_nSoulCount;

	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		tdItem.m_nEnchants[i] = m_nEnchants[i];
	}
	
	if (NULL != this->m_pItemData)
	{
		tdItem.m_nItemID = this->m_pItemData->m_nID;
	}

	tdItem.m_nSlotID = this->m_nSlotID;

	return tdItem;
}

TD_PLAYER_GAME_DATA_ITEM_INSTANCE GItem::ToTD_PLAYER_GAME_DATA_ITEM_INSTANCE()
{
	TD_PLAYER_GAME_DATA_ITEM_INSTANCE tdItem;	

	tdItem.nItemID = m_pItemData->m_nID;
	tdItem.nQuantity = GetAmount();
	tdItem.nDurability = m_nDurability;
	tdItem.nDyedColor = m_nDyedColor;
	tdItem.nIUID = m_nIUID;
	tdItem.bBind = m_bBind;
	tdItem.nSoulQuantity = m_nSoulCount;

	tdItem.bUseUsagePeriod = m_UsagePeriod.IsPeriod();
	tdItem.bStartCalculatingUsagePeriod = m_UsagePeriod.IsStartedCalculatingUsagePeriod();
	tdItem.nUsagePeriodSec = m_UsagePeriod.GetPeriodSec();		
	tdItem.nCharPlayTimeSec = m_UsagePeriod.GetUsageStartCharPlayTimeSec();

	tdItem.bUseExpirationTime = m_ExpirationPeriod.IsPeriod();
	tdItem.nExpirationTimeSec = m_ExpirationPeriod.GetPeriodSec();		

	wcsncpy_s(tdItem.szExpiDt, m_strExpiDt.c_str(), _TRUNCATE);

	tdItem.nUsageState;	// 설정 안함

	tdItem.nSlotType = m_nSlotType;
	tdItem.nSlotID = m_nSlotID;

	// 강화 정보
	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		tdItem.nEnchants[i] = m_nEnchants[i];
	}

	return tdItem;
}


TD_UPDATE_DURABILITY GItem::ToTD_UPDATE_DURABILITY()
{
	TD_UPDATE_DURABILITY tdUpdateDurability;
	tdUpdateDurability.m_nSlotID = this->m_nSlotID;
	tdUpdateDurability.m_nSlotType = this->m_nSlotType;
	tdUpdateDurability.m_nDurability = this->m_nDurability;		

	return tdUpdateDurability;
}

TD_ITEM_INSERT GItem::ToTD_ITEM_INSERT()
{
	return TD_ITEM_INSERT(m_pItemData->m_nID, m_nSlotID, m_nAmount, m_nDurability, m_nDyedColor, m_nSoulCount);
}

int GItem::GetLostedDurability()
{
	return m_pItemData->m_nMaxDurability - m_nDurability;
}

bool GItem::operator==(const GItem* pItem)
{
	if (pItem->m_nDurability == this->m_nDurability &&
		pItem->m_nIUID == this->m_nIUID &&
		pItem->GetAmount() == this->m_nAmount &&
		pItem->m_uidID == this->m_uidID &&
		pItem->m_nDyedColor == this->m_nDyedColor)
	{
		return true;
	}

	return false;
}

bool GItem::operator!=(const GItem* pItem)
{
	if (pItem == this) return false;

	return true;
}


bool GItem::IsInteractableType()
{
	if (ITEMTYPE_USABLE != m_pItemData->m_ItemType) return false;

	if (USABLE_INTERACTION == m_pItemData->m_nUsableType ||
		USABLE_INTERACTION_DEAD == m_pItemData->m_nUsableType)
	{
		return true;
	}

	return false;
}

bool GItem::IsExpired( const int nCharPlayTimeMin )
{
	if (!m_UsagePeriod.IsExpired(nCharPlayTimeMin) && !m_ExpirationPeriod.IsExpired(nCharPlayTimeMin))
		return false;
	
	return true;
}

bool GItem::IsFineDuration() const
{
	if (m_pItemData->m_nMaxDurability <= 0)
		return true;	// 내구력을 고려하지 않음

	if (m_nDurability > 0)
		return true;	// 내구력이 충분함

	return false;
}

bool GItem::IsPeriod() const
{
	return m_UsagePeriod.IsPeriod() || m_ExpirationPeriod.IsPeriod();
}

void GItem::GetExpiDtForDB( wstring& strExpiDt )
{
	if (L"NULL" == m_strExpiDt)
		strExpiDt = m_strExpiDt;
	else
		strExpiDt = wstring(L"'") + m_strExpiDt + wstring(L"'");
}

bool GItem::IsEnchantable(GBuffInfo* pBuffInfo) const
{
	VALID_RET(pBuffInfo, false);
	VALID_RET(pBuffInfo->IsEnchant(), false);

	if (!IsEnchantableActiveBuff(pBuffInfo))
		return false; // 액티브 강화 불가능

	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		int nSlot = m_nEnchants[i];
		if (nSlot == ENCHANT_UNUSED_SLOT)
			return true;
	}

	return false;
}

int8 GItem::AddEnchantSlot(bool bIsActiveSlot, int nBuffID, ENCHANT_COLOR nEnchantColor)
{
	int nEmptyEnchantSlotIdx = GetEmptyEnchantSlotIdx(bIsActiveSlot, nEnchantColor);
	if (nEmptyEnchantSlotIdx == FAILED)
		return FAILED;
	
	m_nEnchants[nEmptyEnchantSlotIdx] = nBuffID;

	return nEmptyEnchantSlotIdx;
}

int8 GItem::GetEmptyEnchantSlotIdx(bool bIsActiveSlot, ENCHANT_COLOR nColor)
{
	VALID_RET(m_pItemData, FAILED);
	if (m_pItemData->m_vecEquipEnchantColor.empty())
		return FAILED; // 소켓이 없음

	if (bIsActiveSlot)
	{
		int nSlot = m_nEnchants[ACTIVE_SLOT];
		if (nSlot != ENCHANT_UNUSED_SLOT)
			return FAILED; // 이미 액티브 강화가 되어있음

		if (nColor != ENCHANT_NONE &&
			m_pItemData->m_vecEquipEnchantColor[ACTIVE_SLOT] != nColor)
			return FAILED; // 소켓 색이 다름

		return ACTIVE_SLOT;
	}
	else
	{
		for (int8 i=1; i<min(ENCHANT_MAX_COUNT, (int8)m_pItemData->m_vecEquipEnchantColor.size()); ++i)
		{
			int nSlot = m_nEnchants[i];
			if (nSlot == ENCHANT_UNUSED_SLOT)
			{
				if (nColor != ENCHANT_NONE &&
					m_pItemData->m_vecEquipEnchantColor[i] != nColor)
					continue; // 소켓 색이 다름
				
				return i;
			}
		}
	}
	
	return FAILED;
}

int8 GItem::Enchant(GBuffInfo* pBuffInfo, int nItemID, ENCHANT_COLOR nEnchantColor)
{
	VALID_RET(pBuffInfo, FAILED);
	return AddEnchantSlot(pBuffInfo->HasProcEffect(), nItemID, nEnchantColor);
}

int8 GItem::EnchantBreak(GBuffInfo* pBuffInfo)
{
	VALID_RET(pBuffInfo, FAILED);
	return AddEnchantSlot(pBuffInfo->HasProcEffect(), ENCHANT_BROKEN_SLOT);
}

int GItem::GetActiveEnchantBuffID() const
{
	return m_nEnchants[0];
}

int GItem::GetEnchantCount() const
{
	int nCount = 0;
	for (int i=0; i<ENCHANT_MAX_COUNT; ++i)
	{
		int nSlot = m_nEnchants[i];
		if (nSlot == ENCHANT_UNUSED_SLOT)
			continue; // 강화 안한 슬롯

		++nCount;
	}

	return nCount;
}

bool GItem::IsEnchantableActiveBuff( GBuffInfo* pBuffInfo) const
{
	VALID_RET(pBuffInfo, false);
	if (!pBuffInfo->HasProcEffect())
		return true; // ProcEffect가 없으면 무시

	if (HasActiveEnchantBuff())
		return false; // 액티브 강화가 이미 되어있음

	return true;
}

bool GItem::HasActiveEnchantBuff() const
{
	return (GetActiveEnchantBuffID() != ENCHANT_UNUSED_SLOT);
}

int GItem::GetID()
{
	return m_pItemData->m_nID;
}

int GItem::GetAmount() const
{
	return m_nAmount;
}