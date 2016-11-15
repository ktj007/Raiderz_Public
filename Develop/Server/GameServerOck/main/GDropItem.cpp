#include "StdAfx.h"
#include "GDropItem.h"
#include "CTransData.h"
#include "GConst.h"
#include "GEntityNPC.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GSTLUtil.h"
#include "GEntityPlayer.h"
#include "GParty.h"
#include "GPartySystem.h"

GDropItem::GDropItem(const MUID& nDropItemUID, int nItemID, int nItemAmount)
: m_nDropItemUID(nDropItemUID)
, m_nItemID(nItemID)
, m_nItemAmount(nItemAmount)
, m_bFreeAuthority(false)
, m_nType(DIT_NONE)
, m_bVisible(true)
{
}

GDropItem::~GDropItem()
{
}

void GDropItem::InsertAuthorizedCID( int nCID )
{
	m_setAuthorizedCID.insert(nCID);
}

void GDropItem::DecraseAmount(int nDecreaseAmount)
{
	m_nItemAmount -= nDecreaseAmount;
}

void GDropItem::SetFreeAuthority()
{
	m_bFreeAuthority = true;
}

void GDropItem::SetGettable()
{
	m_nType = DIT_GETTABLE;
}

void GDropItem::SetMasterLootable()
{
	m_nType = DIT_MASTERLOOTABLE;
}

void GDropItem::SetRollable()
{
	m_nType = DIT_ROLLABLE;
}

void GDropItem::SetType(DROP_ITEM_TYPE nDIT)
{
	m_nType = nDIT;
}

void GDropItem::Visible()
{
	m_bVisible = true;
}

void GDropItem::Unvisible()
{
	m_bVisible = false;
}

void GDropItem::GetViewableCID(set<int>& outsetViewableCID) const
{	
	if (false == m_bVisible) return;

	for each (int nAuthorizedCID in m_setAuthorizedCID)
	{
		outsetViewableCID.insert(nAuthorizedCID);
	}	
}

int GDropItem::GetAmount() const
{
	return m_nItemAmount;
}

const MUID& GDropItem::GetUID() const
{
	return m_nDropItemUID;
}

int GDropItem::GetID() const
{
	return m_nItemID;
}

bool GDropItem::IsViewableCID(int nCID) const
{
	if (false == m_bVisible) return false;
	if (false == IsAuthorizedCID(nCID)) return false;

	return true;
}

bool GDropItem::IsAuthorizedCID( int nCID ) const
{
	if (true == m_bFreeAuthority) return true;
	if (m_setAuthorizedCID.end() == m_setAuthorizedCID.find(nCID)) return false;

	return true;
}

bool GDropItem::IsGettable() const
{
	return DIT_GETTABLE == m_nType;
}

bool GDropItem::IsMasterLootable() const
{
	return DIT_MASTERLOOTABLE == m_nType;
}

bool GDropItem::IsRollable() const
{
	return DIT_ROLLABLE == m_nType;
}

TD_ITEM_DROP GDropItem::MakeTDDropItem() const
{
	return TD_ITEM_DROP(m_nDropItemUID, m_nItemID, m_nItemAmount, m_nType);
}

TD_LOOT_MASTERLOOT_UI GDropItem::MakeTDMasterLootUI(GEntityNPC* pOwner) const
{
	TD_LOOT_MASTERLOOT_UI tdMasterLootUI = TD_LOOT_MASTERLOOT_UI(m_nDropItemUID, m_nItemID, m_nItemAmount);

	const set<int>& setBeneficiaryCID = pOwner->GetNPCLoot().GetDropList().GetBeneficiaryCID();

	int i = 0;
	for each (int nBeneficiaryCID in setBeneficiaryCID)
	{
		GEntityPlayer* pBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL != pBeneficiary)
		{
			tdMasterLootUI.m_Beneficiarys[i] = pBeneficiary->GetUID();
		}
		else
		{
			GParty* pParty = gsys.pPartySystem->FindParty(pOwner->GetNPCLoot().GetDropList().GetPartyUID());
			if (NULL != pParty)
			{
				 tdMasterLootUI.m_Beneficiarys[i] = pParty->FindMemberUID(nBeneficiaryCID);
			}
			else
			{
				tdMasterLootUI.m_Beneficiarys[i] = MUID::ZERO;
			}			
		}

		i++;
	}

	return tdMasterLootUI;
}

TD_LOOT_MASTERLOOT_NOTIFY GDropItem::MakeTDMasterLootNotify() const
{
	return TD_LOOT_MASTERLOOT_NOTIFY(m_nItemID, m_nItemAmount);
}

bool GDropItem::IsVisible() const
{
	return m_bVisible;
}

DROP_ITEM_TYPE GDropItem::GetType() const
{
	return m_nType;
}

void GDropItem::ChangeGettableItem(int nCID)
{
	SetGettable();
	m_setAuthorizedCID.clear();
	m_setAuthorizedCID.insert(nCID);
}