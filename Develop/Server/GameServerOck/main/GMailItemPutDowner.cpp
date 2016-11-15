#include "stdafx.h"
#include "GMailItemPutDowner.h"
#include "CTransData.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "GPlayerMailReserver.h"
#include "GInteractionSystem.h"
#include "GGlobal.h"
#include "GNPCInteractor.h"
#include "GNPCInteractionInfo.h"

GMailItemPutDowner::GMailItemPutDowner()
: m_nLastFailure(CR_FAIL)
{
	// do nothing
}

GMailItemPutDowner::~GMailItemPutDowner()
{
	// do nothing
}

bool GMailItemPutDowner::Do(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pReserveItem)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pReserveItem != NULL, false);


	// 아이템 확인
	int nSlotID = pReserveItem->nInventorySlotID;
	int nAmount = pReserveItem->nItemAmount;

	GItemHolder* pItemHolder =  pPlayer->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nSlotID);

	if (pItem == NULL)
	{		
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_NO_INVENTORY;
		return false;
	}


	// 첨부된 아이템인지 확인한다.
	if (!Check(pPlayer, pItem, nAmount))
	{
		return false;
	}


	// 첨부 목록에 제거한다.
	GPlayerMailReserver& mailReserver = pPlayer->GetMailReserver();		
	bool nResult = mailReserver.PutDownItem(nSlotID, nAmount);

	return nResult;
}

bool GMailItemPutDowner::Check(GEntityPlayer* pPlayer, const GItem* pItem, int nSubAmount)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pItem != NULL, false);


	// 우편 인터랙션 상태를 재확인한다.
	if (!CheckMailInteraction(pPlayer))
	{
		return false;
	}

	// 내릴 수 있는 수량이 남아 있는지 확인한다.
	if (!CheckItemAmount(pPlayer, pItem, nSubAmount))
	{
		return false;
	}


	return true;
}

bool GMailItemPutDowner::CheckMailInteraction(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer != NULL, false);

	// 우체통과 인터렉션 중이어야 한다.
	GNPCInteractor& interactor = gsys.pInteractionSystem->GetNPCInteractor();

	if (!interactor.CheckIProgress(pPlayer, IT_MAILBOX))
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_NOT_INTERATION;
		return false;
	}

	return true;
}

bool GMailItemPutDowner::CheckItemAmount(GEntityPlayer* pPlayer, const GItem* pItem, int nSubAmount)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pItem != NULL, false);
	VALID_RET(0 < nSubAmount, false);

	// 아이템의 첨부된 수량만큼만 내릴 수 있다.
	int nSlotID = pItem->m_nSlotID;	

	GPlayerMailReserver& reserver = pPlayer->GetMailReserver();
	int nReservedAmount = reserver.GetAmount(nSlotID);

	if (nReservedAmount - nSubAmount < 0)
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_NOT_ENOUGH_AMOUNT_DOWN;
		return false;
	}

	return true;
}

CCommandResultTable GMailItemPutDowner::GetLastFailure(void)
{
	return m_nLastFailure;
}
