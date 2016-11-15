#include "StdAfx.h"
#include "GNPCBParts.h"
#include "GEntityNPC.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GAIMonitorPort.h"
#include "GConst.h"
#include "GField.h"
#include "GNPCLoot.h"
#include "CSMotionFactorHelper.h"
#include "GGlobal.h"
#include "GLootSystem.h"
#include "GLootModifyHandler.h"

GNPCBParts::GNPCBParts(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GNPCBParts::~GNPCBParts(void)
{
	m_pOwner->DetachObserver(this);
}

void GNPCBParts::Break(uint8 nPartID, MUID uidOpponent)
{
	if (IsBroken(nPartID))
		return;

	m_setBrokenParts.insert(nPartID);
	
	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwner->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("Breakpart: %d", nPartID);
	}

	vector<TD_USABLE_BPART> outUsableBParts;
	vector<GEntityNPC*> outBPartNPCs;
	OnReward(GetRewardInfo(nPartID), uidOpponent, outUsableBParts, outBPartNPCs);

	RouteBreak(uidOpponent, nPartID, outUsableBParts);

	if (!outUsableBParts.empty())
	{
		RouteInsertLoobatleNPC(outBPartNPCs);
	}
}

void GNPCBParts::OnReward(const vector<BPART_REWARD_INFO>& vecRewardInfo, 
						  MUID uidRewarder, 
						  vector<TD_USABLE_BPART>& outUsableBParts, 
						  vector<GEntityNPC*>& outBPartNPCs)
{
	GField* pField = m_pOwner->GetField();
	VALID(pField);

	for each (const BPART_REWARD_INFO& each in vecRewardInfo)
	{
		GPercentDice dice;
		if (!dice.Dice(each.fRate))	
			return; // 실패	

		float fMinSpawnSize = m_pOwner->GetColRadius(); // test code
		float fMaxSpawnSize = m_pOwner->GetColRadius() + 300.0f; // test code

		vec3 vAdjust = vec3::ZERO;
		switch (each.nDir)
		{
		case UBD_FRONT:
			vAdjust.y = GMath::RandomNumber(fMinSpawnSize, fMaxSpawnSize);
			break;
		case UBD_BACK:
			vAdjust.y = GMath::RandomNumber(-fMinSpawnSize, -fMaxSpawnSize);
			break;
		case UBD_RIGHT:
			vAdjust.x = GMath::RandomNumber(fMinSpawnSize, fMaxSpawnSize);
			break;
		case UBD_LEFT:
			vAdjust.x = GMath::RandomNumber(-fMinSpawnSize, -fMaxSpawnSize);
			break;
		}

		vec3 pos = 
			GMath::LocalPosToWorldPos(m_pOwner->GetDir(), m_pOwner->GetPos(), vAdjust);

		SpawnDetailInfo infoDetail;
		infoDetail.lootable.bEnable = true;
		infoDetail.lootable.uidRewarder = uidRewarder;
		infoDetail.lootable.bPartyReward = true;
		infoDetail.bpart.bEnable = true;
		infoDetail.bpart.nNPCID = m_pOwner->GetID();
		infoDetail.bpart.nPartID = each.nID;
		infoDetail.bpart.nFlag = each.nFlag;
		infoDetail.bpart.nLootID = each.nLootID;
		GEntityNPC* pNPC =
			pField->SpawnNPC(m_pOwner->GetID(), NULL, pos, vec3(0,-1,0), 0, &infoDetail);

		if (!pNPC)
			return;		

		outBPartNPCs.push_back(pNPC);

		TD_USABLE_BPART td;
		td.uid = pNPC->GetUID();
		td.uiid = pNPC->GetUIID();
		td.vPos = pNPC->GetPos();
		td.nFlag = each.nFlag;
		outUsableBParts.push_back(td);
	}
}

void GNPCBParts::Recovery()
{
	m_setBrokenParts.clear();
	RouteRecovery();

	GetScriptNPC()->OnBPartRecver(m_pOwner->GetDelegator());

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwner->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("All Breakparts Recovery");
	}
}

void GNPCBParts::Update( float fDelta )
{
	if (!IsAnyPartBroken())
		return; // 파괴된 파츠가 없음

	if (m_pOwner->IsNowCombat())
		return; // 전투중

	if (m_pOwner->GetHP() < m_pOwner->GetMaxHP())
		return; // 피해 회복이 안되었음

	Recovery();
}

bool GNPCBParts::IsBroken( uint8 nPartID ) const
{
	return m_setBrokenParts.find(nPartID) != m_setBrokenParts.end();
}

void GNPCBParts::SetBreak( uint8 nPartID )
{
	m_setBrokenParts.insert(nPartID);
}

bool GNPCBParts::IsAnyPartBroken() const
{
	return !m_setBrokenParts.empty();
}

void GNPCBParts::MakeTD( TD_UPDATE_CACHE_NPC& out ) const
{
	for (set<CHAR>::const_iterator itor = m_setBrokenParts.begin(); itor != m_setBrokenParts.end(); ++itor)
	{
		switch ((*itor))
		{
		case 1: SetBitSet(out.nStatusFlag, UNS_BREAKPART1); break;
		case 2: SetBitSet(out.nStatusFlag, UNS_BREAKPART2); break;
		case 3: SetBitSet(out.nStatusFlag, UNS_BREAKPART3); break;
		case 4: SetBitSet(out.nStatusFlag, UNS_BREAKPART4); break;
		}
	}
}

void GNPCBParts::RouteBreak( MUID uidOpponent, uint8 nPartID, const vector<TD_USABLE_BPART>& vecUsableBParts )
{
	MCommand* pNewCommand = NULL;

	if (vecUsableBParts.empty())
	{
		pNewCommand = MakeNewCommand(MC_ACTION_BREAK_PART, 3, 
			NEW_UID(m_pOwner->GetUID()),
			NEW_UID(uidOpponent), 
			NEW_CHAR(nPartID));
		
	}
	else
	{
		pNewCommand = MakeNewCommand(MC_ACTION_BREAK_PART_EX, 4, 
			NEW_UID(m_pOwner->GetUID()), 
			NEW_UID(uidOpponent), 
			NEW_CHAR(nPartID), 
			NEW_BLOB(vecUsableBParts));
	}
	
	VALID(pNewCommand);
	m_pOwner->RouteToThisCell(pNewCommand);
}

void GNPCBParts::RouteRecovery()
{
	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_BPART_RECOVERY, 1, NEW_UID(m_pOwner->GetUID()));
	m_pOwner->RouteToThisCell(pNewCommand);
}

vector<BPART_REWARD_INFO> GNPCBParts::GetRewardInfo( uint8 nPartID )
{
	BPartRewardMap::iterator it = m_pOwner->GetNPCInfoMaster()->m_mapBPartRewards.find(nPartID);
	if (it == m_pOwner->GetNPCInfoMaster()->m_mapBPartRewards.end())
		return vector<BPART_REWARD_INFO>();

	return it->second;
}

void GNPCBParts::OnDie()
{
	MUID uidKiller = m_pOwner->GetKiller();

	if (CSMotionFactorHelper::IsMovingMotionFactor(m_pOwner->GetCurrentMotionFactor()))
	{
		for (size_t i = 0; i < MAX_BREAKABLE_PARTS; i++)
		{
			if (IsBroken(i))
				continue;

			Break(i, uidKiller);
		}
	}
}

void GNPCBParts::RouteInsertLoobatleNPC( const vector<GEntityNPC*>& outBPartNPCs )
{
	for each (GEntityNPC* each in outBPartNPCs)
	{
		gsys.pLootSystem->GetModifyHandler().RouteInsertLootableNPC(each);
	}
}

