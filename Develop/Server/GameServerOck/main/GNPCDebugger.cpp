#include "stdafx.h"
#include "GNPCDebugger.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GModuleCombat.h"
#include "GNPCPathFinder.h"
#include "GTalent.h"
#include "GField.h"
#include "GStrings.h"
#include "GJob.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GBehaviorState.h"
#include "GNPCAILod.h"
#include "GHateTable.h"
#include "GNPCRage.h"
#include "GNPCStress.h"

void GNPCDebugger::Logf( const wchar_t* szText, ... )
{
	wchar_t szBuff[1024];

	va_list args;
	va_start(args,szText);
	vswprintf_s(szBuff,szText,args);
	va_end(args);

	Log(szBuff);
}

void GNPCDebugger::Log( const wchar_t* szText )
{
	GField* pField = m_pEntityNPC->m_pField;
	const SPAWN_INFO* pSpawnInfo = m_pEntityNPC->m_pSpawnInfo;

	if (!pField ||
		!pField->GetInfo() ||
		!pSpawnInfo)
	{
		mlog(szText);
		return;
	}

	wchar_t szBuff[1024];
	swprintf_s(szBuff, L"FieldID(%d), SpawnID(%d) -> %s", 
		pField->GetInfo()->m_nFieldID, 
		pSpawnInfo->nID, 
		szText);

	mlog(szBuff);
}

wstring GNPCDebugger::GetCurrentMotionfactorName()
{
	wstring strMF = L"none";

	MF_STATE nMFState = m_pEntityNPC->GetCurrentMotionFactor();

	if (MF_NONE != nMFState)
		strMF = MF_STATE_NAME[nMFState];

	return strMF;
}

wstring GNPCDebugger::GetCurrentFSM()
{
	GModuleAI* pmAI = m_pEntityNPC->GetModuleAI();
	if (!pmAI)	return L"no moduleAI";

	wstring strState;
	wstring strInnerState;

	GBehaviorStateMachine* pFSM = pmAI->GetBehaviorFSM();

	strState = wstring(L"Behavior = ") + GStrings::BehaviorStateToString(pFSM->GetCurrentStateID());

	if (pFSM->GetCurrentStateID() == AI_BEHAVIOR_COMBAT)
	{
		strInnerState = GStrings::CombatStateToString(pFSM->GetCurrentCombatStateID());
	}

	if (strInnerState.length() > 0)
	{
		strState += L" / Combat = ";
		strState += strInnerState;
	}

	return strState;
}

wstring GNPCDebugger::GetCurrentJobName()
{
	int nPushTaskCounter = -1;
	
	GJobMgr* pJobMgr = m_pEntityNPC->GetJobMgr();
	GJob* pCurrJob = pJobMgr->GetCurrJob();
	if (!pCurrJob)	return L"none";

	wstring strJobName = pCurrJob->GetJobName();


	wchar_t buff[512];
	swprintf_s(buff, L"%s", strJobName.c_str());

	return buff;
}

wstring GNPCDebugger::GetCurrentTaskListName()
{
	GJobMgr* pJobMgr = m_pEntityNPC->GetJobMgr();
	GJob* pCurrJob = pJobMgr->GetCurrJob();
	if (!pCurrJob)	return L"none";

	//list<GTask*> Tasks = m_pEntityNPC->GetAllTasks();

	//for (list<GTask*>::iterator iter = Tasks.begin();
	//	iter != Tasks.end();
	//	++iter)
	//{
	//	GTask* pTask = *iter;
	//	if (pTask)
	//	{
	//		strTaskName += L"[";
	//		strTaskName += pTask->GetTaskName();
	//		strTaskName += L"] ";
	//	}
	//}

	return pCurrJob->GetTaskName();
}

wstring GNPCDebugger::GetPathFindingString()
{
	GField* pField = m_pEntityNPC->GetField();
	if (!pField)		return L"invalid field";
	const GFieldInfo* pFieldInfo = pField->GetInfo();
	if (!pFieldInfo)		return L"invalid field info";

	wstring strText;

	wchar_t buff[512];

	vector<vec3> outvPathList;
	m_pEntityNPC->GetPathFinder()->PathTo3DPoints(outvPathList);

	if (outvPathList.empty())	return L"         <Empty PathList>";

	for (size_t i=0; i<outvPathList.size(); ++i)
	{
		swprintf_s(buff, L"[%3d] %5.1f %5.1f %5.1f\n", i+1, 
			outvPathList[i].x,
			outvPathList[i].y,
			outvPathList[i].z
			); 
		strText += buff;
	}

	return strText;
}

wstring GNPCDebugger::GetMotionFactorString()
{
	return m_pEntityNPC->m_MotionFactor.GetDebugString();
}

wstring GNPCDebugger::GetCurTalentPhaseString()
{
	if (!m_pEntityNPC->GetModuleCombat())		return L"Combat Module NULL";

	GTalent* pCurTalent = m_pEntityNPC->GetModuleCombat()->GetTalent();
	if (!pCurTalent)		return L"none";

	switch (pCurTalent->GetPhase())
	{
	case TALENT_PHASE_NONE: return L"none";
	case TALENT_PHASE_PREPARE: return L"prepare";
	case TALENT_PHASE_ACT: return L"act";
	case TALENT_PHASE_FINISH: return L"finish";
	case TALENT_PHASE_CANCELED: return L"canceled";
	case TALENT_PHASE_EXTRA: return L"extra";
	case TALENT_PHASE_EXTRA2: return L"extra2";
	case TALENT_PHASE_EXTRA3: return L"extra3";
	}

	return L"unknown";
}

wstring GNPCDebugger::GetAAString()
{
	switch (m_pEntityNPC->GetAAType())
	{
	case NAAT_NONE: return L"none";
	case NAAT_ALWAYS: return L"always";
	case NAAT_FACTION: return L"faction";
	}

	return L"unknown";
}
wstring GNPCDebugger::GetAttackableString()
{
	switch (m_pEntityNPC->GetNPCStatus().nAttackable)
	{
	case NAT_NONE: return L"none";
	case NAT_ALWAYS: return L"always";
	case NAT_FACTION: return L"faction";
	}

	return L"unknown";
}

int GNPCDebugger::GetStress()
{
	return m_pEntityNPC->GetNPCStress().GetStress();
}

int GNPCDebugger::GetRage()
{
	return m_pEntityNPC->GetNPCRage().GetRage();
}

int GNPCDebugger::GetLodLevel( void ) const
{
	if (!m_pEntityNPC->GetModuleAI())		return AI_LOD_INVALID_LEVEL;

	return m_pEntityNPC->GetModuleAI()->GetLodLevel();
}

int GNPCDebugger::GetSightRange() const
{
	return m_pEntityNPC->GetNPCInfo()->nSightRange;
}

bool GNPCDebugger::IsAutoAssist() const
{
	return m_pEntityNPC->GetNPCInfo()->m_bAutoAssist;
}

float GNPCDebugger::GetAutoAssistRange() const
{
	return m_pEntityNPC->GetNPCInfo()->m_fAutoAssistRange;
}

vector<string> GNPCDebugger::GetTimerStrings()
{
	if (!m_pEntityNPC->m_pScriptTimer)
		return vector<string>();

	GScriptTimer::MAP_REGULATORS& mapRgrs =
		m_pEntityNPC->m_pScriptTimer->m_mapRegulators;
	
	vector<string> ret;
	for (GScriptTimer::MAP_REGULATORS::iterator iter = mapRgrs.begin(); iter != mapRgrs.end(); ++iter)
	{
		const int nTimerID			= iter->first;
		MRegulator& Regulator		= iter->second;
		bool bRepeat				= (m_pEntityNPC->m_pScriptTimer->m_setRepeatedIDs.find(nTimerID) != m_pEntityNPC->m_pScriptTimer->m_setRepeatedIDs.end());
		
		char buff[512];
		sprintf(buff, "%d = %.2f / %.2f%s", nTimerID, Regulator.GetElapsedTime(), Regulator.GetTickTime(), bRepeat?" (Repeat)":"");
		ret.push_back(buff);
	}

	return ret;
}
