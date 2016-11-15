#include "stdafx.h"
#include "GBirdMonitor.h"
#include "GField.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GJob.h"
#include "GConst.h"
#include "GCommandTable.h"
#include "GNPCDebugger.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GCommand.h"
#include "GServer.h"
#include "GBehaviorStateMachine.h"
#include "GPlayerObjectManager.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GHateTable.h"
#include "GDropItem.h"
#include "GDropItems.h"
#include "GItemHolder.h"
#include "GEquipmentSlot.h"

GBirdMonitor& GBirdMonitor::GetInstance()
{
	static GBirdMonitor st_Instance;
	return st_Instance;
}


GBirdMonitor::GBirdMonitor() 
: m_bReportToClient(false)
, m_uidReportRequests(0)
, m_nOutputLine(0)
{
}

GBirdMonitor::~GBirdMonitor()
{
}

void GBirdMonitor::Update()
{
	if (!IsRunForTest())
	{
		return;
	}

	for each (Entry each in m_uidReportRequests)
	{
		m_nOutputLine = 4;

		GField* pField = gmgr.pFieldMgr->GetField(each.uidField);
		if (pField)
		{
			GEntityPlayer* pPlayer = pField->FindPlayer(each.nPlayerUIID);
			if (pPlayer)
			{
				OutputMyPlayer(each, pPlayer);
			}

			GEntityNPC* pEntityNPC = pField->FindNPC(each.nTargetNPCUIID);
			if (pEntityNPC)
			{
				OutputNPC(each, pEntityNPC);
			}
			else
			{
				int nCurOutputLine = m_nOutputLine;
				for (int i = nCurOutputLine; i <= 12; i++)
					Output(each, L"");

				FindEnemy(each, pField);
			}
		}
	}
}

void GBirdMonitor::AttachReportToClient( Entry entry )
{
	GField* pField = gmgr.pFieldMgr->GetField(entry.uidField);
	if (pField == NULL) return;

	GEntityNPC* pEntityNPC = pField->FindNPC(entry.nTargetNPCUIID);
	if (pEntityNPC)
	{
		if (GConst::LOG_FSM)
		{
			GBehaviorStateMachine* pFSM = pEntityNPC->GetBehaviorFSM();
			pFSM->SetFSMLog(true);
		}
	}

	m_uidReportRequests.push_back(entry);
	m_bReportToClient = true;
	gsys.pServer->GetServerInfo().bDamageTest = true;
}

void GBirdMonitor::DetachReportToClient( MUID uidRequest )
{
	vector<Entry>::iterator it = m_uidReportRequests.begin();
	for (it; it != m_uidReportRequests.end();)
	{
		if (it->nPlayerUIID == uidRequest)
		{
			it = m_uidReportRequests.erase(it);
		}
		else
		{
			++it;
		}
	}

	if (m_uidReportRequests.empty())
	{
		m_bReportToClient = false;
		gsys.pServer->GetServerInfo().bDamageTest = false;
	}
}

bool GBirdMonitor::IsAttached( MUID uidRequest )
{
	vector<Entry>::iterator it = m_uidReportRequests.begin();
	for (it; it != m_uidReportRequests.end(); ++it)
	{
		if (it->nPlayerUIID == uidRequest)
			return true;
	}

	return false;
}

UIID GBirdMonitor::GetTargetNPC(UIID nPlayerUIID)
{
	vector<Entry>::iterator it = m_uidReportRequests.begin();
	for (it; it != m_uidReportRequests.end(); ++it)
	{
		if (it->nPlayerUIID == nPlayerUIID)
			return it->nTargetNPCUIID;
	}

	return UIID_INVALID;
}

void GBirdMonitor::SetTargetNPC( UIID nPlayerUIID, UIID nTargetNPCUIID )
{
	vector<Entry>::iterator it = m_uidReportRequests.begin();
	for (it; it != m_uidReportRequests.end(); ++it)
	{
		if (it->nPlayerUIID == nPlayerUIID)
		{
			it->nTargetNPCUIID = nTargetNPCUIID;
			return;
		}
	}
}


void GBirdMonitor::ReportToClient( Entry entry, int nParam, const wchar_t* szText )
{
	if (!IsRunForTest())
	{
		return;
	}

	GField* pField = gmgr.pFieldMgr->GetField(entry.uidField);
	if (pField == NULL) return;

	GEntityPlayer* pEntityPlayer = pField->FindPlayer(entry.nPlayerUIID);
	if (pEntityPlayer == NULL) return;

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, NEW_WSTR(L"monitor"), NEW_INT(nParam), NEW_WSTR(szText), NEW_BLOB());
	pEntityPlayer->RouteToMe(pNewCommand);
}

void GBirdMonitor::Output( Entry entry, const wchar_t* pFormat, ... )
{
	if (!IsRunForTest())
	{
		return;
	}

	int nLine = m_nOutputLine++;

	wchar_t temp[1024] = L"";

	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	if (m_pListener)
	{
		m_pListener->OnOutput(nLine, temp);
	}

	if (m_bReportToClient)
	{
		ReportToClient(entry, nLine, temp);
	}
}

void GBirdMonitor::OutputMyPlayer( Entry entry, GEntityPlayer* pEntityPlayer )
{
	PLAYER_INFO* pi = pEntityPlayer->GetPlayerInfo();
	GItemHolder* pItemHolder = pEntityPlayer->GetItemHolder();

	int nWeaponDamageMin = 0, nWeaponDamageMax = 0;

	const GItem* pWeaponItem = pItemHolder->GetEquipment().GetRightWeapon();

	if (NULL != pWeaponItem)
	{
		GItemData* pItemData = pWeaponItem->m_pItemData;
		if (NULL != pItemData)
		{
			if (0 < pItemData->m_nMaxDurability && 0 >= pWeaponItem->m_nDurability) 
			{
				nWeaponDamageMin = 1;
				nWeaponDamageMax = 1;
			}
			else
			{
				nWeaponDamageMin = pItemData->m_nMinDamage;
				nWeaponDamageMax = pItemData->m_nMaxDamage;
			}
		}
	}

	wstring strMF = L"none";
	{
		MF_STATE nMFState = pEntityPlayer->GetCurrentMotionFactor();

		if (MF_NONE != nMFState)
			strMF = MF_STATE_NAME[nMFState];
	}

	Output(entry, L"Player(%I64d) - pos(%.1f %.1f %.1f), dir(%.1f %.1f %.1f)", 
		pEntityPlayer->GetUID(), 
		pEntityPlayer->GetPos().x, pEntityPlayer->GetPos().y, pEntityPlayer->GetPos().z,
		pEntityPlayer->GetDir().x, pEntityPlayer->GetDir().y, pEntityPlayer->GetDir().z);

	Output(entry, L" name(%s) , level(%d) , str(%d) , dex(%d) , int(%d) , AP(%d) , speed(%.1f) , 무기데미지(%d ~ %d)",
		pi->szName, 
		pEntityPlayer->GetLevel(), 
		pEntityPlayer->GetSTR(),
		pEntityPlayer->GetDEX(),
		pEntityPlayer->GetINT(),
		pEntityPlayer->GetAP(),
		pEntityPlayer->GetWalkSpeed(),
		nWeaponDamageMin, nWeaponDamageMax);

	Output(entry, L"MF=%s, %s", strMF.c_str(), pEntityPlayer->GetMotionFactorString().c_str());
}

void GBirdMonitor::OutputNPC( Entry entry, GEntityNPC* pEntityNPC )
{
	GNPCDebugger* pNPCDebugger = pEntityNPC->GetDebugger();

	Output(entry, L"NPC(%I64d) - pos(%.1f %.1f %.1f), dir(%.1f %.1f %.1f)", 
		pEntityNPC->GetUID(), 
		pEntityNPC->GetPos().x, pEntityNPC->GetPos().y, pEntityNPC->GetPos().z,
		pEntityNPC->GetDir().x, pEntityNPC->GetDir().y, pEntityNPC->GetDir().z);

	Output(entry, L" name(%s) , hp(%d) , speed(%.1f)", 
		pEntityNPC->GetNPCInfo()->strName.c_str(), 
		pEntityNPC->GetHP(),
		pEntityNPC->GetWalkSpeed()
		);

	Output(entry, L" grade(%d) , ap(%d) , mode(%d)", 
		pEntityNPC->GetGrade(), pEntityNPC->GetAP(), pEntityNPC->GetMode());

	Output(entry, L" Job = %s", pNPCDebugger->GetCurrentJobName().c_str());
	Output(entry, L" Task = %s", pNPCDebugger->GetCurrentTaskListName().c_str());
	Output(entry, L" MF = %s", pNPCDebugger->GetCurrentMotionfactorName().c_str());
	Output(entry, L" FSM = %s", pNPCDebugger->GetCurrentFSM().c_str());
	Output(entry, L" MF = %s", pNPCDebugger->GetMotionFactorString().c_str());
	Output(entry, L" TalentPhase = %s", pNPCDebugger->GetCurTalentPhaseString().c_str());
	Output(entry, L" LodLevel = %d", pNPCDebugger->GetLodLevel());
	Output(entry, L" Stress = %d", pNPCDebugger->GetStress());
	Output(entry, L" Rage = %d", pNPCDebugger->GetRage());
	Output(entry, L" SightRange = %d", pNPCDebugger->GetSightRange());
	Output(entry, L" AutoAssist = %s, range = %.2f", pNPCDebugger->IsAutoAssist()?L"true":L"false", pNPCDebugger->GetAutoAssistRange());
	Output(entry, L"HateTable ===");
	vector<pair<MUID, HatePoint> > vecHateTables;
	pEntityNPC->GetHateTable().GetHighestTargets(pEntityNPC->GetHateTable().GetQty(), vecHateTables);
	for each (pair<MUID, HatePoint> each in vecHateTables)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(each.first);
		wstring strName = L"unknown";
		if (pPlayer)
			strName = pPlayer->GetPlayerInfo()->szName;
		Output(entry, L"    %8s | %6d (%.0f/%.0f)", strName.c_str(), each.second, pEntityNPC->GetHateTable().GetTimeoutTime(each.first), pEntityNPC->GetHateTable().GetExpiredTime());
	}
	Output(entry, L"ItemDropList ===");

	const map<MUID, GDropItem*>& mapDropItem = pEntityNPC->GetNPCLoot().GetDropList().GetDropItems().GetContainer();
	for each(map<MUID, GDropItem*>::value_type val in mapDropItem)
	{
		GDropItem* pDropItem = val.second;
		if (NULL == pDropItem) continue;

		Output(entry, L"%d | %d", pDropItem->GetID(), pDropItem->GetAmount());
	}
	

	Output(entry, L"==================");
}

void GBirdMonitor::FindEnemy( Entry entry, GField* pField )
{
	if (UIID_INVALID != entry.nPlayerUIID)
		return;

	GEntityPlayer* pPlayer = pField->FindPlayer(entry.nPlayerUIID);
	if (!pPlayer)
		return;

	GVectorMUID vecNPCs;
	pPlayer->GetNeighborSectorNPC(vecNPCs.Vector());

	for each (MUID each in vecNPCs.Vector())
	{
		GEntityNPC* pNPC = pField->FindNPC(each);
		if (!pNPC)
			continue;

		if (!pNPC->IsNowCombat())
			continue;

		entry.nTargetNPCUIID = pNPC->GetUIID();
		break;
	}
}

GBirdMonitor::Entry* GBirdMonitor::FindEntry( MUID uidRequest )
{
	vector<Entry>::iterator it = m_uidReportRequests.begin();
	for (it; it != m_uidReportRequests.end(); ++it)
	{
		if (it->nPlayerUIID == uidRequest)
		{
			return &(*it);
		}
	}

	return NULL;
}
