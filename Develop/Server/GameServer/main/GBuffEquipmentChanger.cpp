#include "StdAfx.h"
#include "GBuffEquipmentChanger.h"
#include "GEntityPlayer.h"
#include "GBuffInfo.h"
#include "GGlobal.h"
#include "GItemManager.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"
#include "GOverlappedEquipmentSlot.h"
#include "GModuleBuff.h"

GBuffEquipmentChanger::GBuffEquipmentChanger(void)
{
}

GBuffEquipmentChanger::~GBuffEquipmentChanger(void)
{
}

void GBuffEquipmentChanger::OnGain( MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo )
{
	if (!pTarget->IsPlayer())
		return; // 플레이어만 적용 가능

	if (!pBuffInfo->m_bUseChangeEquipmentSlot)
		return; // 장비 변경하지 않음

	GEntityPlayer* pTargetPlayer = ToEntityPlayer(pTarget);
	RemoveAllChangeEquipmentBuffs(pTargetPlayer, pBuffInfo->m_nID);

	VALID(pBuffInfo);
	for (int nSlot=0; nSlot<ITEMSLOT_MAX; ++nSlot)
	{
		int nItemID = pBuffInfo->m_nEquipItemIDs[nSlot];
		if (nItemID == INVALID_ID)
			continue;

		GItem* pItem = gmgr.pItemManager->MakeNewItem(nItemID);
		if (!pItem)
			continue;
				
		pTargetPlayer->GetItemHolder()->GetEquipment().GetOverlapped().SetItem((SH_ITEM_SLOT)nSlot, pItem);
	}
}

void GBuffEquipmentChanger::OnLost( MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute )
{
	if (!pTarget->IsPlayer())
		return; // 플레이어만 적용 가능

	if (!pBuffInfo->m_bUseChangeEquipmentSlot)
		return; // 장비 변경하지 않음

	GEntityPlayer* pTargetPlayer = ToEntityPlayer(pTarget);
	pTargetPlayer->GetItemHolder()->GetEquipment().GetOverlapped().Reset();
}

void GBuffEquipmentChanger::RemoveAllChangeEquipmentBuffs(GEntityPlayer* pTargetPlayer, int nExceptBuff)
{
	GModuleBuff* pmBuff = pTargetPlayer->GetModuleBuff();
	pmBuff->RemoveAllChangeEquipmentBuffs(nExceptBuff);
}
