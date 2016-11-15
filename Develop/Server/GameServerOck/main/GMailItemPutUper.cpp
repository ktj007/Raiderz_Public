#include "stdafx.h"
#include "GMailItemPutUper.h"
#include "CTransData.h"
#include "GItemHolder.h"
#include "GEntityPlayer.h"
#include "GPlayerMailReserver.h"
#include "GGlobal.h"
#include "GItemActableChecker.h"
#include "GNPCInteractor.h"
#include "GInteractionSystem.h"
#include "GNPCInteractionInfo.h"

GMailItemPutUper::GMailItemPutUper()
: m_nLastFailure(CR_FAIL)
{
	// do nothing
}

GMailItemPutUper::~GMailItemPutUper()
{
	// do nothing
}

bool GMailItemPutUper::Do(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pReserveItem)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pReserveItem != NULL, false);


	// 아이템 확인
	int nSlotID = pReserveItem->nInventorySlotID;
	int nAmount = pReserveItem->nItemAmount;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nSlotID);

	if (pItem == NULL)
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_NO_INVENTORY;
		return false;
	}


	// 첨부할 수 있는지 확인한다.
	if (!Check(pPlayer, pItem, nAmount))
	{
		return false;
	}


	// 첨부 목록에 추가한다.
	GPlayerMailReserver& mailReserver = pPlayer->GetMailReserver();		
	bool nResult = mailReserver.PutUpItem(nSlotID, nAmount);

	return nResult;
}

bool GMailItemPutUper::Check(GEntityPlayer* pPlayer, const GItem* pItem, int nAddAmount)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pItem != NULL, false);


	// 우편 인터랙션 상태를 재확인한다.
	if (!CheckMailInteraction(pPlayer))
	{
		return false;
	}

	// 아이템 속성이 첨부가능 아이템인지 확인한다.
	if (!CheckAppendable(pPlayer, pItem))
	{
		return false;
	}

	// 첨부가능한 수량이 남아 있는지 확인한다.
	if (!CheckItemAmount(pPlayer, pItem, nAddAmount))
	{
		return false;
	}


	return true;
}

bool GMailItemPutUper::CheckItemAmount(GEntityPlayer* pPlayer, const GItem* pItem, int nAddAmount)
{
	VALID_RET(pPlayer != NULL, false);
	VALID_RET(pItem != NULL, false);
	VALID_RET(0 < nAddAmount, false);

	// 첨부할 수량이 아이템 현재 수량을 넘을 수 없다.
	int nSlotID = pItem->m_nSlotID;
	int nTotalAmount = pItem->GetAmount();

	GPlayerMailReserver& reserver = pPlayer->GetMailReserver();
	int nReservedAmount = reserver.GetAmount(nSlotID);

	if (nTotalAmount < nReservedAmount + nAddAmount)
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_NOT_ENOUGH_AMOUNT_UP;
		return false;
	}

	return true;
}

bool GMailItemPutUper::CheckAppendable(GEntityPlayer* pPlayer, const GItem* pItem)
{
	// 귀속 아이템은 첨부 못한다.
	if (pItem->m_bBind)
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_BIND;
		return false;
	}

	// 기간제 아이템은 첨부 못한다.
	if (pItem->m_UsagePeriod.IsPeriod() || pItem->m_ExpirationPeriod.IsPeriod())
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_PERIOD;
		return false;
	}

	// 강화 아이템은 첨부 못한다.
	if (0 < pItem->GetEnchantCount())
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_ENCHANT;
		return false;
	}

	// 현재 다른 기능에서 처리중인 아이템은 첨부 못한다.
	if (!gsys.pItemSystem->GetActableChecker().IsMailAppendable(pPlayer, pItem))
	{
		m_nLastFailure = CR_FAIL_MAIL_APPEND_ITEM_BUSY;
		return false;
	}


	return true;
}

bool GMailItemPutUper::CheckMailInteraction(GEntityPlayer* pPlayer)
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

CCommandResultTable GMailItemPutUper::GetLastFailure(void)
{
	return m_nLastFailure;
}
