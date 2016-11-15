#include "StdAfx.h"
#include "GPlayerTargetInfoSender.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GCommand.h"
#include "GSystem.h"
#include "GGlobal.h"
#include "GConst.h"
#include "GPlayerObjectManager.h"
#include "GNPCIconSelector.h"
#include "CCommandTable.h"
#include "GAIMonitorPort.h"

GPlayerTargetInfoSender::GPlayerTargetInfoSender(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
	m_rgrTick.SetTickTime(GConst::TARGET_INFO_OLD_SEND_TIME);
	m_rgrTick.Start();
}

GPlayerTargetInfoSender::~GPlayerTargetInfoSender(void)
{
}

void GPlayerTargetInfoSender::Update(float fDelta)
{
	if (true == m_rgrTick.IsReady(fDelta)) 
	{
		ClearInvalidSendTimeRecord();	
	}
}

void GPlayerTargetInfoSender::Send(const vector<UIID>& vecUIID)
{
	vector<TD_TARGET_INFO> vecTDTargetInfo = MakeTDTargetInfo(vecUIID);	

	RecordSendTime(vecTDTargetInfo);
	Route(vecTDTargetInfo);

	if (GConst::AIMON_ENABLE &&
		GConst::AIMON_AUTO_TARGET)
	{
		GField* pField = m_pOwner->GetField();
		if (!vecUIID.empty() &&
			pField)
		{
			GEntityNPC* pNPC = pField->FindNPC(vecUIID.front());
			if (pNPC)
			{
				GAIMonitorPort::Inst().SetNPC(pNPC->GetUID());
			}
		}
	}
}

vector<TD_TARGET_INFO> GPlayerTargetInfoSender::MakeTDTargetInfo(const vector<UIID>& vecUIID)
{
	vector<TD_TARGET_INFO> vecTDTargetInfo;

	for each (UIID nUIID in vecUIID)
	{
		if (true == IsJustNowSend(nUIID)) continue;

		if (true == IsPlayerUIID(nUIID))
		{
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nUIID);
			if (NULL == pPlayer) continue;
			if (SECTOR_SIZE * 3 < m_pOwner->DistanceTo(pPlayer)) continue;

			vecTDTargetInfo.push_back(TD_TARGET_INFO(pPlayer->GetUIID(), pPlayer->GetMaxHP(), pPlayer->GetHPPercent(), pPlayer->GetLevel(), pPlayer->IsNowCombat(), NIT_NONE));
		}
		else if (true == IsNPCUIID(nUIID))
		{
			GEntityNPC* pNPC = m_pOwner->FindNPC(nUIID);
			if (NULL == pNPC) continue;

			if (SECTOR_SIZE * 3 < m_pOwner->DistanceTo(pNPC)) continue;

			GNPCIconSelector npcIconSelector;
			NPC_ICON_TYPE nIConType = npcIconSelector.SingleSelect(m_pOwner, pNPC);

			vecTDTargetInfo.push_back(TD_TARGET_INFO(pNPC->GetUIID(), pNPC->GetMaxHP(), pNPC->GetHPPercent(), pNPC->GetLevel(), pNPC->IsNowCombat(), nIConType));
		}
		else
		{
			continue;
		}
	}

	return vecTDTargetInfo;
}

void GPlayerTargetInfoSender::Route(const vector<TD_TARGET_INFO>& vecTDTargetInfo)
{
	if (true == vecTDTargetInfo.empty()) return;

	MCommand* pNewCommand = MakeNewCommand(MC_CHAR_TARGET_INFO, 1, 
		NEW_BLOB(vecTDTargetInfo));

	m_pOwner->RouteToMe(pNewCommand);
}

bool GPlayerTargetInfoSender::IsJustNowSend(const UIID& nTargetUIID)
{
	map<UIID, uint32>::iterator itor = m_mapSendTime.find(nTargetUIID);
	if (m_mapSendTime.end() == itor) return false;

	uint32 nNowTime = gsys.pSystem->GetNowTime();
	uint32 nSendTime = (*itor).second;

	uint32 nElapsedTime = nNowTime - nSendTime;

	if (TARGET_INFO_REFRESH_TIME <= nElapsedTime) return false;

	return true;
}

void GPlayerTargetInfoSender::RecordSendTime(vector<TD_TARGET_INFO>& vecTDTargetInfo)
{
	uint32 nNowTime =gsys.pSystem->GetNowTime();

	for (vector<TD_TARGET_INFO>::iterator itorTargetInfo = vecTDTargetInfo.begin(); itorTargetInfo != vecTDTargetInfo.end(); itorTargetInfo++)
	{
		map<UIID, uint32>::iterator itorSendTime = m_mapSendTime.find(itorTargetInfo->m_nUIID);
		if (m_mapSendTime.end() == itorSendTime)
		{
			m_mapSendTime.insert(map<UIID, uint32>::value_type(itorTargetInfo->m_nUIID, nNowTime));
		}
		else
		{
			itorSendTime->second = nNowTime;
		}
	}	
}

void GPlayerTargetInfoSender::ClearInvalidSendTimeRecord()
{
	for each (const map<UIID, uint32>::value_type& val in m_mapSendTime)
	{
		UIID nUIID = val.first;
		uint32 nSendTime = val.second;

		uint32 nNowTime = gsys.pSystem->GetNowTime();
		uint32 nElapsedTime = nNowTime - nSendTime;

		if (true == IsPlayerUIID(nUIID))
		{
			GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nUIID);
			if (NULL == pPlayer || GConst::TARGET_INFO_OLD_SEND_TIME < nElapsedTime)
			{
				m_mapSendTime.erase(nUIID);
			}
		}
		else if (true == IsNPCUIID(nUIID))
		{
			GEntityNPC* pNPC = m_pOwner->FindNPC(nUIID);
			if (NULL == pNPC || GConst::TARGET_INFO_OLD_SEND_TIME < nElapsedTime)
			{
				m_mapSendTime.erase(nUIID);
			}
		}

		
	}
}

void GPlayerTargetInfoSender::ClearSendTime()
{
	m_mapSendTime.clear();
}