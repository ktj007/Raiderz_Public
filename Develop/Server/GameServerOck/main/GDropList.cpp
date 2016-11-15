#include "StdAfx.h"
#include "GDropList.h"
#include "GEntityNPC.h"
#include "GLootInfo.h"
#include "GEntityPlayer.h"
#include "GDropitem.h"
#include "GDropSilver.h"
#include "GDropSoul.h"
#include "GItemManager.h"
#include "GSTLUtil.h"
#include "GDropItems.h"
#include "GNPCBParts.h"
#include "GParty.h"
#include "GPartySystem.h"
#include "GGlobal.h"
#include "GDropSouls.h"
#include "GLootInfoMgr.h"


GDropList::GDropList(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_nDropCause(DC_NONE)
, m_nMasterCID(0)
, m_nPartyUID(MUID::ZERO)
{	
	m_pDropItems = new GDropItems(pOwner);
	m_pDropMoney = new GDropMoney();
	m_pDropSouls = new GDropSouls(pOwner);
}

GDropList::~GDropList(void)
{
	SAFE_DELETE(m_pDropMoney);
	SAFE_DELETE(m_pDropItems);
	SAFE_DELETE(m_pDropSouls)
}

void GDropList::Clear()
{
	m_pDropItems->Clear();
	m_pDropMoney->Clear();
	m_pDropSouls->Clear();		
	m_setBeneficiaryCID.clear();

	m_nDropCause = DC_NONE;
	m_nMasterCID = 0;
	m_nPartyUID = MUID::ZERO;
}

bool GDropList::DropByKill(int nBeneficiaryCID)
{
	vector<int> vecBeneficiaryCID;
	vecBeneficiaryCID.push_back(nBeneficiaryCID);

	return DropByKill(vecBeneficiaryCID, MUID::ZERO);
}

bool GDropList::DropByKill(const vector<int>& vecBeneficiaryCID, MUID nPartyUID)
{
	if (vecBeneficiaryCID.empty()) return false;

	if (DC_NONE != m_nDropCause) return false;

	GLootInfo* pLootInfo = m_pOwner->GetLootInfo();		
	if (NULL == pLootInfo) return false;

	m_setBeneficiaryCID = VectorToSet(vecBeneficiaryCID);
	m_nPartyUID = nPartyUID;

	LOOTING_RULE_DATA lootingRuleData(LRC_FREE_FOR_ALL, LRR_FREE_FOR_ALL, LRRF_RARE_OR_HIGHER);
	GParty* pParty = gsys.pPartySystem->FindParty(nPartyUID);
	if (NULL != pParty)
	{		
		lootingRuleData = pParty->GetLootingRuleData();
		m_nMasterCID = pParty->GetLeaderCID();		
	}
	
	m_pDropItems->DropByKill(pLootInfo, vecBeneficiaryCID, nPartyUID, lootingRuleData);
	m_pDropMoney->Drop(vecBeneficiaryCID, pLootInfo);	
	m_pDropSouls->Drop(vecBeneficiaryCID, pLootInfo);

	m_nDropCause = DC_KILL;	

	return true;
}

bool GDropList::DropByGather(int nGatherCID, int nLootID)
{
	if (DC_NONE != m_nDropCause) return false;
	
	GLootInfo* pLootInfo = gmgr.pLootInfoMgr->Get(nLootID);
	if (NULL == pLootInfo) return false;

	m_setBeneficiaryCID.insert(nGatherCID);

	LOOTING_RULE_DATA lootingRuleData(LRC_FREE_FOR_ALL, LRR_FREE_FOR_ALL, LRRF_RARE_OR_HIGHER);
	m_pDropItems->DropByKill(pLootInfo, nGatherCID, MUID::ZERO, lootingRuleData);
	
	m_nDropCause = DC_GATHER;

	return true;
}

bool GDropList::DropByInteract_Mortal(int nBeneficiaryCID,int nLootID)
{
	return DropByInteract(nBeneficiaryCID, nLootID, false);
}

bool GDropList::DropByInteract_Immortal(int nBeneficiaryCID,int nLootID)
{
	return DropByInteract(nBeneficiaryCID, nLootID, true);
}

bool GDropList::DropByInteract(int nBeneficiaryCID, int nLootID, bool bImmortal)
{
	if (true == bImmortal)
	{
		if (DC_INTERACT_MORTAL == m_nDropCause) return false;
	}

	if (DC_KILL == m_nDropCause && false == IsEmpty()) return false;	
	if (DC_GATHER == m_nDropCause) return false;

	GLootInfo* pLootInfo = gmgr.pLootInfoMgr->Get(nLootID);
	if (NULL == pLootInfo) return false;

	m_setBeneficiaryCID.insert(nBeneficiaryCID);

	m_pDropItems->DropByInteract(nBeneficiaryCID, pLootInfo);

	if (true == bImmortal)
	{
		m_nDropCause = DC_INTERACT_IMMORTAL;
	}
	else
	{
		m_nDropCause = DC_INTERACT_MORTAL;
	}

	return true;
}

bool GDropList::IsEmpty()
{
	if (false == m_pDropItems->IsEmpty() || 
		false == m_pDropMoney->IsEmpty()) return false;

	return true;
}

bool GDropList::IsAuthorizedCID( int nCID )
{
	if (true == m_pDropMoney->IsAuthorizedCID(nCID)) return true;
	if (true == m_pDropItems->IsAuthorizedCID(nCID)) return true;

	if (m_pOwner->IsBPart() &&
		IsBeneficiaryCID(nCID))
	{
		return true;
	}

	return false;
}

bool GDropList::IsViewableCID( int nCID )
{
	if (true == m_pDropMoney->IsAuthorizedCID(nCID)) return true;
	if (true == m_pDropItems->IsViewableCID(nCID)) return true;

	if (m_pOwner->IsBPart() &&
		IsBeneficiaryCID(nCID))
	{
		return true;
	}

	return false;
}

bool GDropList::IsDropByKill()
{
	return DC_KILL == m_nDropCause;
}

bool GDropList::IsDropByGather()
{
	return DC_GATHER == m_nDropCause;
}

bool GDropList::IsDropByInteract_Mortal()
{
	return DC_INTERACT_MORTAL == m_nDropCause;
}

bool GDropList::IsDropByInteract_Immortal()
{
	return DC_INTERACT_IMMORTAL == m_nDropCause;
}

bool GDropList::IsBeneficiaryCID( int nCID )
{
	if (m_setBeneficiaryCID.end() == m_setBeneficiaryCID.find(nCID)) return false;

	return true;
}


void GDropList::GetViewableCID(set<int>& outsetViewableCID)
{
	m_pDropItems->GetViewableCID(outsetViewableCID);
	m_pDropMoney->GetViewableCID(outsetViewableCID);

	for each (int each in m_setBeneficiaryCID)
	{
		if (m_pOwner->IsBPart() &&
			IsBeneficiaryCID(each))
		{
			outsetViewableCID.insert(each);
		}
	}
	
}

GDropItems& GDropList::GetDropItems()
{
	return (*m_pDropItems);
}

GDropMoney& GDropList::GetDropMoney()
{
	return (*m_pDropMoney);
}

GDropSouls& GDropList::GetDropSouls()
{
	return (*m_pDropSouls);
}

const set<int>& GDropList::GetBeneficiaryCID()
{
	return m_setBeneficiaryCID;
}

void GDropList::SetDropCause(DROP_CAULE nDropCause)
{
	m_nDropCause = nDropCause;
}

int GDropList::GetMasterCID()
{
	return m_nMasterCID;
}

void GDropList::InsertBeneficiaryCID(int nCID)
{
	m_setBeneficiaryCID.insert(nCID);
}

MUID GDropList::GetPartyUID()
{
	return m_nPartyUID;
}
