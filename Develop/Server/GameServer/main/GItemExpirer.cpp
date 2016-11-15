#include "StdAfx.h"
#include "GItemExpirer.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GItemHolder.h"
#include "GItemRemover.h"

const float CYCLIC_CHECK_TIME_ONE_MINUTE		= 60.0f;

GItemExpirer::GItemExpirer(GEntityPlayer* pPlayer)
: m_pOwnerPlayer(pPlayer)
{
}

GItemExpirer::~GItemExpirer(void)
{
}

void GItemExpirer::Init()
{
	m_CheckTimeRegulator.SetTickTime(CYCLIC_CHECK_TIME_ONE_MINUTE);
	m_CheckTimeRegulator.Start();
}

void GItemExpirer::Run(float fDelta)
{
	PFC_THISFUNC;
	if (m_CheckTimeRegulator.IsReady(fDelta) == false) return;

	ExpireItems();
}

void GItemExpirer::RunImmediately()
{
	ExpireItems();
}

void GItemExpirer::ExpireItems()
{
	m_vecExpiredItem.clear();

	BuildExpiredItemList();
	ExpireEachItem();	
}

GExprieItemVec& GItemExpirer::GetExpiredItemList()
{
	return m_vecExpiredItem;
}

void GItemExpirer::ExpireEachItem()
{
	if (m_vecExpiredItem.empty())
		return;

	GItem* pItem = NULL;

	for (size_t i = 0; i < m_vecExpiredItem.size(); ++i)
	{
		pItem = m_vecExpiredItem[i];
		gsys.pItemSystem->GetRemover().Remove(m_pOwnerPlayer, pItem->m_nSlotType, pItem->m_nSlotID, pItem->GetAmount());
	}
}

void GItemExpirer::BuildExpiredItemList()
{
	GItemHolder* pItemHolder = m_pOwnerPlayer->GetItemHolder();	
	pItemHolder->CollectExpiringItem(SLOTTYPE_EQUIP, m_pOwnerPlayer->GetPlayerInfo()->nPlayTimeSec, m_vecExpiredItem);	
	pItemHolder->CollectExpiringItem(SLOTTYPE_INVENTORY, m_pOwnerPlayer->GetPlayerInfo()->nPlayTimeSec, m_vecExpiredItem);	
}
