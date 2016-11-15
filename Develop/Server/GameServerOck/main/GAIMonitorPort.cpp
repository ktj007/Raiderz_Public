#include "StdAfx.h"
#include "GAIMonitorPort.h"
#include "MUtil.h"
#include "GGlobal.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "GNPCMove.h"
#include "GNPCRage.h"
#include "GNPCDebugger.h"
#include "GJob.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GConst.h"
#include "GFieldMgr.h"
#include "GSharedField.h"
#include "GHateTableDef.h"
#include "GHateTable.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GFieldNPCSession.h"
#include "GActorCooltimeChecker.h"

GAIMonitorPort::GAIMonitorPort(void)
{
	if (GConst::AIMON_ENABLE)
	{
		m_rgrTick.SetTickTime(GConst::AIMON_UPDATE_INTERVAL);
		m_rgrTick.Start();
	}
}

GAIMonitorPort::~GAIMonitorPort(void)
{
}

GAIMonitorPort& GAIMonitorPort::Inst()
{
	static GAIMonitorPort inst;
	return inst;
}

void GAIMonitorPort::SetNPC( MUID uidTargetNPC )
{
	if (!GConst::RUN_FOR_TEST)
		return;

	if (uidTargetNPC.IsInvalid())
		return;

	GEntityNPC* pNPCNew = gmgr.pNPCMgr->GetEntityNPC(uidTargetNPC);
	if (!pNPCNew)
		return; 

	MUID uidOld = m_uidTargetNPC;
	m_uidTargetNPC = uidTargetNPC;

	GEntityNPC* pNPCOld = gmgr.pNPCMgr->GetEntityNPC(uidOld);
	if (pNPCOld)
	{
		pNPCOld->DetachObserver(this);
	}

	pNPCNew->AttachObserver(this);	

	LaunchMonitor();
}

class GJobForLog : public GJob, public MTestMemPool<GJobForLog>
{
public:
	GJobForLog(wstring _strTitle, string _strDesc): GJob(NULL), strTitle(_strTitle), strDesc(_strDesc) {}
	virtual bool OnStart() override { return true; }
	GJobID GetID() { return GJOB_NA; }
	virtual const wchar_t* GetJobName() const override { return strTitle.c_str(); }
	virtual string GetParamString() const override { return strDesc; }

	wstring strTitle;
	string strDesc;
};

void GAIMonitorPort::RefreshNPCInfo()
{
	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(m_uidTargetNPC);
	if (!pNPC)
	{
		// 찾을 수 없으면 기본 설정으로...
		GSharedField* pField = gmgr.pFieldMgr->GetSharedField(GConst::AIMON_SET_FIELDID);
		if (!pField)
			return;

		GEntityNPC* pNPC = pField->GetSpawnNPC(GConst::AIMON_SET_SPAWNID);
		if (!pNPC)
			return;

		SetNPC(pNPC->GetUID());
		return;
	}

	Send(ExtractNPCInfoForXML(pNPC));
	Send(ExtractJobQueueForXML("BJobQueue", pNPC->GetJobMgr()->GetJobs()));
	Send(ExtractJobQueueForXML("MJobQueue", pNPC->GetBehaviorFSM()->GetMissionJobs()));
	if (pNPC->HasSession())
	{
		GFieldNPCSession* pSession =
			pNPC->GetSession();
		VALID(pSession);
		
		deque<GJob*> qSessionJobs = pSession->GetScheduledJobs();

		char szName[512];
		sprintf(szName, "_____Session: %s", pSession->GetName().c_str());

		char szDesc[512];
		sprintf(szDesc, "Scene: %s, ConditionQty: %d", pSession->GetSceneName(), pSession->GetScneneConditionCount());
		
		GJobForLog* pJobTitleLog = new GJobForLog(MLocale::ConvAnsiToUTF16(szName).c_str(), szDesc);
		qSessionJobs.push_front(pJobTitleLog);
		
		
		Send(ExtractJobQueueForXML("SJobQueue", qSessionJobs));

		SAFE_DELETE(pJobTitleLog);
	}
	else
	{
		const deque<GJob*> empty;
		Send(ExtractJobQueueForXML("SJobQueue", empty));
	}
}

string GAIMonitorPort::ExtractNPCInfoForXML( GEntityNPC* pNPC )
{
	VALID_RET(pNPC, "");
	VALID_RET(pNPC->GetNPCInfo(), "invalid npcinfo");
	
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNPCAttr = new MXmlElement("NPCAttr");
	root->LinkEndChild(pNPCAttr);

	GNPCDebugger* pNPCDebugger = pNPC->GetDebugger();

	AddXMLElement("Name", MLocale::ConvUTF16ToAnsi(pNPC->GetName()), pNPCAttr);
	AddXMLElement("ID", pNPC->GetID(), pNPCAttr);
	AddXMLElement("SpawnID", (int)pNPC->GetSpawnID(), pNPCAttr);
	AddXMLElement("UIID", pNPC->GetUIID(), pNPCAttr);
	AddXMLElement("Level", pNPC->GetLevel(), pNPCAttr);
	AddXMLElement("Mode", pNPC->GetMode(), pNPCAttr);
	AddXMLElement("WalkSpeed", pNPC->GetWalkSpeed(), pNPCAttr);
	AddXMLElement("RunSpeed", pNPC->GetNPCMove().GetRunSpeed(), pNPCAttr);
	AddXMLElement("Grade", pNPC->GetGrade(), pNPCAttr);
	AddXMLElement("Stress", pNPC->GetStress(), pNPCAttr);
	AddXMLElement("Rage", pNPC->GetNPCRage().GetRage(), pNPCAttr);
	AddXMLElement("FSM", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetCurrentFSM().c_str()), pNPCAttr);
	AddXMLElement("IsNowCombat", pNPC->GetBehaviorFSM()->IsNowCombat()?1:0, pNPCAttr);
	AddXMLElement("SightRange", pNPCDebugger->GetSightRange(), pNPCAttr);
	AddXMLElement("UsingTalentID", pNPC->GetCurrentTalent(), pNPCAttr);
	AddXMLElement("UsingTalentPhase", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetCurTalentPhaseString().c_str()), pNPCAttr);
	AddXMLElement("AutoAssist", pNPCDebugger->IsAutoAssist()?"true":"false", pNPCAttr);
	AddXMLElement("AutoAssistRange", pNPCDebugger->GetAutoAssistRange(), pNPCAttr);
	AddXMLElement("CurrentJob", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetCurrentJobName()), pNPCAttr);
	AddXMLElement("CurrentTask", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetCurrentTaskListName()), pNPCAttr);
	AddXMLElement("Motionfactor", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetCurrentMotionfactorName()), pNPCAttr);
	AddXMLElement("Attackable", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetAttackableString()), pNPCAttr);
	AddXMLElement("CounterAttack", pNPC->GetNPCInfo()->bCounterAttack?"true":"false", pNPCAttr);
	AddXMLElement("AA", MLocale::ConvUTF16ToAnsi(pNPCDebugger->GetAAString()), pNPCAttr);
	AddXMLElement("GroupID", pNPC->GetGroupID(), pNPCAttr);
	AddXMLElement("IsActivateUpdate", pNPC->IsActivateUpdate(), pNPCAttr);

	char buff[512];
	for (size_t i=0; i<GEntityNPC::GetUserdataSize(pNPC->GetID()); ++i)
	{
		sprintf(buff, "Userdata%d", i);
		AddXMLElement(buff, pNPC->GetUserdata(i), pNPCAttr);
	}
	
	const MOTION_FACTOR_GROUP& mf_group = pNPC->GetMotionfactorForTest().GetFactorsForTest();
	for (int i = MF_BEATEN; i<MF_SIZE; i++)
	{
		wstring strName = MF_STATE_NAME[i];
		strName = strName.erase(0, 3);
		AddXMLElement(MLocale::ConvUTF16ToAnsi(strName), mf_group.var[i].nValue, pNPCAttr);
	}

	vector<pair<MUID, HatePoint> > vecHateTables;
	pNPC->GetHateTable().GetHighestTargets(5, vecHateTables);
	for (size_t i=0; i<vecHateTables.size(); ++i)
	{
		pair<MUID, HatePoint>& each = vecHateTables[i];
		sprintf(buff, "Hatetable%d", i+1);
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(each.first);
		wstring strName = L"unknown";
		if (pPlayer)
			strName = pPlayer->GetPlayerInfo()->szName;

		char szText[512];
		sprintf(szText, "%s: %d", MLocale::ConvUTF16ToAnsi(strName).c_str(), each.second);
			
		AddXMLElement(buff, szText, pNPCAttr);
	}

	vector<string> vecCooltimeDescs = pNPC->GetActorCooltimeChecker().GetDescription();
	for (size_t i=0; i<vecCooltimeDescs.size(); ++i)
	{
		const string& each = vecCooltimeDescs[i];
		sprintf(buff, "Cooltime%d", i+1);

		AddXMLElement(buff, each.c_str(), pNPCAttr);
	}

	vector<string> vecTimerStrings = pNPCDebugger->GetTimerStrings();
	for (size_t i=0; i<vecTimerStrings.size(); ++i)
	{
		const string& each = vecTimerStrings[i];
		sprintf(buff, "Timer%d", i+1);

		AddXMLElement(buff, each.c_str(), pNPCAttr);
	}
	
	return xml.GetBuffer();
}

string GAIMonitorPort::ExtractJobQueueForXML( const char* pszJobType, const deque<GJob*>& vecJobs )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNPCAttr = new MXmlElement(pszJobType);
	root->LinkEndChild(pNPCAttr);

	for each (GJob* each in vecJobs)
	{
		MXmlElement* pNew = new MXmlElement("Item");

		if (each)
		{
			const wchar_t* pszJobName = each->GetJobName();
			wstring strJobName = pszJobName;
			strJobName = strJobName.erase(0, 5);
			_SetAttribute(pNew, "JobName",		MLocale::ConvUTF16ToAnsi(strJobName));
			_SetAttribute(pNew, "TaskName",		MLocale::ConvUTF16ToAnsi(each->GetTaskName()));
			_SetAttribute(pNew, "Param",		each->GetParamString());
		}
		else
		{
			_SetAttribute(pNew, "JobName",		"Null");
		}
		
		pNPCAttr->LinkEndChild(pNew);
	}

	return xml.GetBuffer();
}

void GAIMonitorPort::Update( float fDelta )
{
	if (!GConst::RUN_FOR_TEST)
		return;

	if (!GConst::AIMON_ENABLE)
		return;

	if (!m_rgrTick.IsReady(fDelta))
		return;

	RefreshNPCInfo();
}

void GAIMonitorPort::OnJobEnqueue( bool bMissionMode, const GJob* pJob )
{
	VALID(pJob);
	Send(ExtractJobEnqueueLogForXML(bMissionMode, pJob));
}

void GAIMonitorPort::OnJobEvent( bool bMissionMode, const char* szEvent )
{
	Send(ExtractJobEventLogForXML(bMissionMode, szEvent));
}

void GAIMonitorPort::OnJobFinished( const GJob* pJob )
{
	VALID(pJob);
	Send(ExtractJobFinishLogForXML(pJob));
}

string GAIMonitorPort::ExtractJobEnqueueLogForXML( bool bMissionMode, const GJob* pJob )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNew = new MXmlElement("Log");
	root->LinkEndChild(pNew);

	wstring strJobName = pJob->GetJobName();
	strJobName = strJobName.erase(0, 5);

	string strText = "";
	if (bMissionMode)
		strText += "[Mission]";
	else
		strText += "[Behavior]";

	strText += MLocale::ConvUTF16ToAnsi(strJobName);
	strText += " / ";
	strText += pJob->GetParamString();
	strText += MakeJobCountString();
	
	_SetAttribute(pNew, "Type",	"JobEnq");
	_SetAttribute(pNew, "Text",	strText);

	return xml.GetBuffer();
}

string GAIMonitorPort::ExtractJobEventLogForXML( bool bMissionMode, const char* szEvent )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNew = new MXmlElement("Log");
	root->LinkEndChild(pNew);

	string strText = "";
	if (bMissionMode)
		strText += "[Mission]";
	else
		strText += "[Behavior]";

	strText += szEvent;
	strText += MakeJobCountString();

	_SetAttribute(pNew, "Type",	"JobEvent");
	_SetAttribute(pNew, "Text",	strText);

	return xml.GetBuffer();
}

string GAIMonitorPort::ExtractJobFinishLogForXML( const GJob* pJob )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNew = new MXmlElement("Log");
	root->LinkEndChild(pNew);

	wstring strJobName = pJob->GetJobName();
	strJobName = strJobName.erase(0, 5);

	string strText = MLocale::ConvUTF16ToAnsi(strJobName);
	strText += " / ";
	strText += pJob->GetParamString();
	strText += MakeJobCountString();

	_SetAttribute(pNew, "Type",	"JobFin");
	_SetAttribute(pNew, "Text",	strText);

	return xml.GetBuffer();
}

string GAIMonitorPort::ExtractLogForXML( const char* szType, const char* szText )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNew = new MXmlElement("Log");
	root->LinkEndChild(pNew);

	_SetAttribute(pNew, "Type",	szType);
	_SetAttribute(pNew, "Text",	szText);

	return xml.GetBuffer();
}


string GAIMonitorPort::ExtractGLogForXML( int nLevel, const char* temp )
{
	MXml xml;
	MXmlDocument* root = xml.Doc();
	MXmlElement* pNew = new MXmlElement("Log");
	root->LinkEndChild(pNew);

	string strLevel = "Unknown Level";
	switch (nLevel)
	{
	case MLOG_LEVEL_DEBUG:			strLevel = "GLogD";	break;
	case MLOG_LEVEL_INFORMATION:	strLevel = "GLogI";	break;
	case MLOG_LEVEL_WARNING:		strLevel = "GLogW";	break;
	case MLOG_LEVEL_ERROR:			strLevel = "GLogE";	break;
	}

	_SetAttribute(pNew, "Type", strLevel);
	_SetAttribute(pNew, "Text",	temp);

	return xml.GetBuffer();
}

bool GAIMonitorPort::IsSetedNPC( MUID uidTargetNPC ) const
{
	return (m_uidTargetNPC == uidTargetNPC);
}

void GAIMonitorPort::SendCallbackMessageToViewer( const char* pFormat, ... )
{
	char temp[1024];
	va_list args;
	va_start(args,pFormat);
	vsprintf_s(temp,pFormat,args);
	va_end(args);

	Send(ExtractLogForXML("Callback", temp));
}

void GAIMonitorPort::SendEventMessageToViewer( const char* pFormat, ... )
{
	char temp[1024];
	va_list args;
	va_start(args,pFormat);
	vsprintf_s(temp,pFormat,args);
	va_end(args);

	Send(ExtractLogForXML("Event", temp));
}

void GAIMonitorPort::SendGameLogToViewer( int nLevel, const char* temp )
{
	Send(ExtractGLogForXML(nLevel, temp));
}

string GAIMonitorPort::MakeJobCountString()
{
	string strJobCount = "";
	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(m_uidTargetNPC);
	if (!pNPC)
		return "invalid_npc";

	VALID_RET(pNPC->GetModuleAI()->GetBehaviorFSM(), "error");
	int nBJobCount = pNPC->GetJobMgr()->GetCount();
	int nMJobCount = pNPC->GetModuleAI()->GetBehaviorFSM()->GetMissionJobCounter();
	char szBJobCount[128];
	_itoa_s(nBJobCount, szBJobCount, 10);
	char szMJobCount[128];
	_itoa_s(nMJobCount, szMJobCount, 10);
	strJobCount += " (CountB: ";
	strJobCount += szBJobCount;
	strJobCount += " / CountM: ";
	strJobCount += szMJobCount;
	strJobCount += ")";

	return strJobCount;
}

bool GAIMonitorPort::IsEnable()
{
	if (!GConst::RUN_FOR_TEST)
		return false;

	if (!GConst::AIMON_ENABLE)
		return false;

	return true;
}
