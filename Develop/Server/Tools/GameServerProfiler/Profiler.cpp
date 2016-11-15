#include "StdAfx.h"
#include "Profiler.h"
#include "GServer.h"
#include "GSystem.h"
#include "GFieldMgr.h"
#include "GNPCMgr.h"
#include "GPlayerObjectManager.h"
#include "GConfig.h"
#include "MLocale.h"
#include "GField.h"
#include "GFieldInfo.h"
#include "MStatisticsProfiler.h"

#define TOTAL_TICK_ID		0
#define DEFAULT_PROFILEID	1

Profiler::Profiler(void)
: m_nLastTime(0)
, m_nTickCount(0)
, m_pReporter(NULL)
, m_pFileStream(NULL)
{
	m_rgrTickElasedTime.SetTickTime(1.0f);
	m_rgrTickElasedTime.Start();
}

Profiler::~Profiler(void)
{
	Fini();
}

const char* PROFILE_XML_TAG_MAIET		= "maiet";
const char* PROFILE_XML_TAG_CONFIG	= "Config";
const char* PROFILE_XML_TAG_NAME	= "Name";
const char* PROFILE_XML_TAG_FIELDLIST	= "FieldList";
const char* PROFILE_XML_TAG_STARTTIME	= "StartTime";
const char* PROFILE_XML_TAG_DURATIONTIME	= "DurationTime";
const char* PROFILE_XML_TAG_LIMITTIME	= "LimitTime";
const char* PROFILE_XML_TAG_OUTPUTFILENAME	= "OutputFilename";
const char* PROFILE_XML_TAG_OPTION	= "Option";
const char* PROFILE_XML_TAG_REPORTMODE = "ReportMode";

const char* PROFILE_XML_ATTR_PROFILE_ID		= "profile_id";
const char* PROFILE_XML_ATTR_LIMIT_MAX		= "max";
const char* PROFILE_XML_ATTR_LIMIT_AVR		= "avr";
const char* PROFILE_XML_ATTR_LIMIT_PERCENT	= "percent";
const char* PROFILE_XML_ATTR_NPC_FIGHT		= "npc_fight";
const char* PROFILE_XML_ATTR_AILOD_ENABLE	= "ai_lod_enable";

bool Profiler::LoadConfig(string strConfigFileName)
{
	// 설정 파일 읽기
	MXml xml;

	if (!xml.LoadFile(strConfigFileName.c_str())) 
	{
		assert(false && "invalid xml file format\n");
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(PROFILE_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), PROFILE_XML_TAG_CONFIG))
		{
			for (MXmlElement* pChild = pElement->FirstChildElement(); pChild != NULL; pChild = pChild->NextSiblingElement())
			{
				if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_REPORTMODE))
				{
					m_Desc.bReportMode = true;
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_NAME))
				{
					_Contents(m_Desc.strName, pChild, &xml);
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_FIELDLIST))
				{
					_Contents(GConfig::m_strAutoTestFieldlistFileName, pChild);
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_STARTTIME))
				{
					_Contents(&m_Desc.fStartTime, pChild);
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_DURATIONTIME))
				{
					_Contents(&m_Desc.fDurationTime, pChild);
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_OUTPUTFILENAME))
				{
					_Contents(m_Desc.strOutputFilename, pChild);
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_LIMITTIME))
				{
					int nProfileID=-1;
					Record record;
					_Attribute(&nProfileID, pChild, PROFILE_XML_ATTR_PROFILE_ID);
					DCHECK(nProfileID != -1 && "프로파일ID를 설정하지 않음");
					_Attribute(&record.fLimitMaxTime, pChild, PROFILE_XML_ATTR_LIMIT_MAX);
					_Attribute(&record.fLimitAvrTime, pChild, PROFILE_XML_ATTR_LIMIT_AVR);
					_Attribute(&record.fLimitTotalPercent, pChild, PROFILE_XML_ATTR_LIMIT_PERCENT);
					
					m_mapRecords.insert(std::make_pair(nProfileID, record));
				}
				else if (!_stricmp(pChild->Value(), PROFILE_XML_TAG_OPTION))
				{
					bool val;
					_Attribute(&val, pChild, PROFILE_XML_ATTR_NPC_FIGHT);
					if (val)
					{
						// NPC끼리 싸우기
						GConfig::m_strAutoTestType = CONFIG_AUTO_TEST_AI;
					}

					if (_Attribute(&val, pChild, PROFILE_XML_ATTR_AILOD_ENABLE))
					{
						m_Desc.bAILODEnable = val;
					}
				}
			}
		}
	}

	// 시작 시간 설정
	if (m_Desc.fStartTime > 0.0f)
	{
		m_rgrStart.SetTickTime(m_Desc.fStartTime);
		m_rgrStart.Start();
	}
	
	// 종료 시간 설정
	DCHECK(m_Desc.fDurationTime > 0.0f);
	m_rgrDuration.SetTickTime(m_Desc.fStartTime+m_Desc.fDurationTime);
	m_rgrDuration.Start();

	return true;
}

bool Profiler::Init()
{
	mlog("Profiler initialization...\n");
	m_TestSystems.Create();
	m_TestManagers.Create();
	if (gsys.pServer->Create(MLocale::ConvAnsiToUTF16(PROFILER_NAME), !GConfig::m_bNoDB) == false)
	{
		mlog3("ServerApp Create Failed\n");
		return false;
	}

	// 테스트 초기화
	for each (GFieldMgr::FIELD_BY_UID_MAP::value_type each in gmgr.pFieldMgr->GetFieldMap())
	{
		GField* pField = each.second;
		if (!pField->GetInfo())
			continue;

		const_cast<GFieldInfo*>(pField->GetInfo())->m_bAILODEnable = m_Desc.bAILODEnable;
	}

	// 프로파일러 시작
	MInitStatisticsProfile(true);
	MResetStatisticsProfile();
	m_TickProfiler.SetEnabled(true);
	
	// 결과 파일 초기화
	m_pFileStream = new ofstream(m_Desc.strOutputFilename.c_str());
	m_pReporter = new MMaietXmlTestReporter(*m_pFileStream);

	return true;
}

void Profiler::Fini()
{
	// 평균값 검사
	CheckAvrLimitation();
	// 비중 검사
	CheckTotalPercentLimitation();

	// 테스트 중지
	gsys.pServer->Destroy();
	m_TestManagers.Destroy();
	m_TestSystems.Destroy();

	// 결과 파일 종료
	if (m_pReporter != NULL && m_pFileStream != NULL)
	{
		SaveReport();		
	}

	SAFE_DELETE(m_pReporter);
	SAFE_DELETE(m_pFileStream);

	// 프로파일러 중지
	char szFileName[512];
	SYSTEMTIME time;
	::GetLocalTime(&time);
	sprintf_s( szFileName, "log/profile/%d%02d%02d_%02d%02d%02d.txt", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	MSaveStatisticsProfile(szFileName, false);	
	MFinalizeStatisticsProfile();
}

void Profiler::Run()
{
	mlog("Begin Profiling...\n");
	mlog("\tReport Filename: %s\n", m_Desc.strOutputFilename.c_str());
	if (m_Desc.bReportMode)
		mlog("\tReport mode: %s\n", FILENAME_PROFILE);
	else
		mlog("\tBaseline mode: %s\n", m_Desc.strOutputFilename.c_str());

	AddMessage("Name: %s", MLocale::ConvAnsiToUTF8(m_Desc.strName.c_str()).c_str());
	AddMessage("StartTime: %.1f second(s) later", m_Desc.fStartTime);
	AddMessage("DurationTime: %.1f second(s)", m_Desc.fDurationTime);
	AddMessage("Field Qty: %d", gmgr.pFieldMgr->GetFieldQty());

	while(true)
	{
		if (m_nLastTime == 0)
		{
			m_nLastTime = gsys.pSystem->GetNowTime();
			continue;
		}

		unsigned int thistime = gsys.pSystem->GetNowTime();
		unsigned int elapsed = thistime - m_nLastTime;
		m_nLastTime = thistime;
		float fDelta = .001f*(float)elapsed;
		//if (fDelta > MAX_TICK_DELTA) fDelta = MAX_TICK_DELTA;

		if (IsExit(fDelta))
			break;

		OnTick(fDelta);
	}
	AddMessage("Player Qty: %d", gmgr.pPlayerObjectManager->GetPlayersCount());
	AddMessage("NPC Qty: %d", gmgr.pNPCMgr->GetNPCMap().size());
	mlog("Finish Profiling.\n");
}

void Profiler::BeginProfile()
{
	m_TickProfiler.Reset();
	m_TickProfiler.BeginProfile(DEFAULT_PROFILEID, "");
	if (!m_Desc.bReportMode)
	{
		MResetStatisticsProfile();	// 매 틱마다 글로벌 프로파일 초기화
	}
}

void Profiler::EndProfile()
{
	m_TickProfiler.EndProfile(DEFAULT_PROFILEID);
}

void Profiler::CheckMaxLimitation(float fDelta)
{
	for (RecordMap::iterator it = m_mapRecords.begin();
		 it != m_mapRecords.end();
		 ++it)
	{
		int nProfileID	= it->first;
		Record& record	= it->second;

		float fElapsedTime = (nProfileID==TOTAL_TICK_ID)? fDelta : .001f*(float)PFI_GT(nProfileID);
		if (fElapsedTime == 0.0f)
			continue; // 호출되지 않았음

		// 레코드 값 갱신
		if (record.strName.empty())
		{
			record.strName = 
				MLocale::ConvAnsiToUTF8(
					(nProfileID==TOTAL_TICK_ID)? "Gameserver-TotalTickTime" : MStatisticsProfileGetProfileName(nProfileID));
		}
		record.fTotalTime += fElapsedTime; 
		record.fMaxTime = max(record.fMaxTime, fElapsedTime);
		record.fMinTime = min(record.fMinTime, fElapsedTime);
		++record.nCallCount;

		// 최대값 체크
		if (fElapsedTime > record.fLimitMaxTime)
		{
			IncreaseMaxLimitFault(nProfileID);
		}
	}
}

void Profiler::CheckAvrLimitation()
{
	for (RecordMap::iterator it = m_mapRecords.begin();
		it != m_mapRecords.end();
		++it)
	{
		int nProfileID	= it->first;
		Record& record	= it->second;

		if (record.fLimitAvrTime == 0.0f)
			continue; // 평균 베이스라인이 지정되어 있지 않음

		if (record.fTotalTime == 0.0f)
			continue; // 호출되지 않았음

		// 평균값 체크
		if (record.fTotalTime/record.nCallCount > record.fLimitAvrTime)
		{
			IncreaseAvrLimitFault(nProfileID);
		}
	}
}

void Profiler::CheckTotalPercentLimitation()
{
	for (RecordMap::iterator it = m_mapRecords.begin();
		it != m_mapRecords.end();
		++it)
	{
		int nProfileID	= it->first;
		Record& record	= it->second;

		if (record.fLimitTotalPercent == 0.0f)
			continue; // 베이스라인이 지정되어 있지 않음

		if (record.fTotalTime == 0.0f)
			continue; // 호출되지 않았음
		
		// 비중 체크
		if (record.fTotalTime/m_Desc.fDurationTime > record.fLimitTotalPercent)
		{
			IncreaseTotalPercentLimitFault(nProfileID);
		}
	}
}

void Profiler::SaveReport()
{
	MMaietTestResult testResult;
	testResult.testName = PROFILER_NAME;

	// 테스트 시간 기록
	testResult.timeElapsed += m_mapRecords[TOTAL_TICK_ID].fTotalTime;

	
	for each (RecordMap::value_type each in m_mapRecords)
	{
		int nProfileID			= each.first;
		const Record& record	= each.second;

		if (record.strName.empty())
			continue; // 한번도 불리지 않은 프로파일 함수

		// 최대 베이스라인 실패 처리
		if (record.fLimitMaxTime > 0.0f)
		{
			if (m_mapMaxLimitFaults.find(nProfileID) == m_mapMaxLimitFaults.end())
			{
				string strReport = MakeReportString(CHECK_MAXLIMIT, record, nProfileID, 0);
				testResult.successs.push_back(MMaietTestResult::Result::pair(0, strReport));
			}
			else
			{
				int nFaultCount = m_mapMaxLimitFaults[nProfileID];
				string strReport = MakeReportString(CHECK_MAXLIMIT, record, nProfileID, nFaultCount);
				testResult.failures.push_back(MMaietTestResult::Result::pair(0, strReport));
			}
		}
		
		// 평균 베이스라인 실패 처리
		if (record.fLimitAvrTime > 0.0f)
		{
			if (m_mapAvrLimitFaults.find(nProfileID) == m_mapAvrLimitFaults.end())
			{
				string strReport = MakeReportString(CHECK_AVRLIMIT, record, nProfileID, 0);
				testResult.successs.push_back(MMaietTestResult::Result::pair(0, strReport));
			}
			else
			{
				int nFaultCount = m_mapAvrLimitFaults[nProfileID];
				string strReport = MakeReportString(CHECK_AVRLIMIT, record, nProfileID, nFaultCount);
				testResult.failures.push_back(MMaietTestResult::Result::pair(0, strReport));
			}
		}

		// 비중 베이스라인 실패 처리
		if (record.fLimitTotalPercent > 0.0f)
		{
			if (m_mapTotalPercentLimitFaults.find(nProfileID) == m_mapTotalPercentLimitFaults.end())
			{
				string strReport = MakeReportString(CHECK_TOTAL_PERCENT_LIMIT, record, nProfileID, 0);
				testResult.successs.push_back(MMaietTestResult::Result::pair(0, strReport));
			}
			else
			{
				int nFaultCount = m_mapTotalPercentLimitFaults[nProfileID];
				string strReport = MakeReportString(CHECK_TOTAL_PERCENT_LIMIT, record, nProfileID, nFaultCount);
				testResult.failures.push_back(MMaietTestResult::Result::pair(0, strReport));
			}
		}
	}

	// 요약 처리
	int nFailedTestCount = 0;
	int nFaiureCount = 0;
	float fElapsedTime = 0.0f;
	
	m_TestResulList.push_back(testResult);

	for each (MMaietTestResult each in m_TestResulList)
	{
		if (!each.failures.empty())
		{
			nFailedTestCount++;
			nFaiureCount += (int) each.failures.size();
		}

		fElapsedTime += each.timeElapsed;
	}
	
	m_pReporter->ReportSummary((int)m_TestResulList.size(), nFailedTestCount, nFaiureCount, fElapsedTime, m_TestResulList);
}

bool Profiler::IsMeasurable(float fDelta)
{
	if (!m_rgrStart.IsActive())
		return true;

	if (m_rgrStart.IsReady(fDelta))
	{
		m_rgrStart.Stop();
		return true;
	}

	return false;
}

bool Profiler::IsExit(float fDelta)
{
	if (!m_rgrDuration.IsActive())
		return true;	// 지속시간 없다면 즉시 종료

	if (m_rgrDuration.IsReady(fDelta))
		return true;

	return false;
}

void Profiler::OnTick( float fDelta )
{
	PrintElapsedTime(fDelta);

	BeginProfile();
	OnUpdate(fDelta);
	EndProfile();

	if (!IsMeasurable(fDelta))
		return; // 아직 검사 시간이 되지 않았음

	OnMeasurement(fDelta);
}

void Profiler::OnUpdate( float fDelta )
{
	gsys.pServer->Update(fDelta);
}

void Profiler::OnMeasurement( float fDelta )
{
	++m_nTickCount;
	CheckMaxLimitation(fDelta);
}

int Profiler::IsFail()
{
	if (!m_mapMaxLimitFaults.empty())
		return 1;	// 실패

	if (!m_mapAvrLimitFaults.empty())
		return 1;	// 실패	

	if (!m_mapTotalPercentLimitFaults.empty())
		return 1;	// 실패

	return 0; // 성공	
}

void Profiler::IncreaseMaxLimitFault( int nProfileID )
{
	if (m_mapMaxLimitFaults.find(nProfileID) == m_mapMaxLimitFaults.end())
		m_mapMaxLimitFaults.insert(std::make_pair(nProfileID, 1));
	else
		++m_mapMaxLimitFaults[nProfileID];
}

void Profiler::IncreaseAvrLimitFault( int nProfileID )
{
	if (m_mapAvrLimitFaults.find(nProfileID) == m_mapAvrLimitFaults.end())
		m_mapAvrLimitFaults.insert(std::make_pair(nProfileID, 1));
	else
		++m_mapAvrLimitFaults[nProfileID];
}

void Profiler::IncreaseTotalPercentLimitFault( int nProfileID )
{
	if (m_mapTotalPercentLimitFaults.find(nProfileID) == m_mapTotalPercentLimitFaults.end())
		m_mapTotalPercentLimitFaults.insert(std::make_pair(nProfileID, 1));
	else
		++m_mapTotalPercentLimitFaults[nProfileID];
}

void Profiler::AddMessage( const char* pFormat, ... )
{
	char temp[1024];
	va_list args;
	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	m_pReporter->AddMessage(temp);
	mlog("%s\n", temp);
}

void Profiler::PrintElapsedTime(float fDelta)
{
	if (!m_rgrTickElasedTime.IsReady(fDelta))
		return;

	static int s_nElapsedTime = 0;
	s_nElapsedTime += m_rgrTickElasedTime.GetTickCounter();
	mlog("Profile now running... (elapsed_time: %d sec(s))\n", s_nElapsedTime);
}

string Profiler::MakeReportString( CheckType nCheckType, const Record &record, int nProfileID, int nFaultCount )
{
	string strReportName;
	float fLimitTime = 0.0f;
	switch (nCheckType)
	{
	case CHECK_MAXLIMIT:
		fLimitTime = record.fLimitMaxTime; 
		strReportName = "MaxLimit";
		break;
	case CHECK_AVRLIMIT: 
		fLimitTime = record.fLimitAvrTime; 
		strReportName = "AvrLimit";
		break;
	case CHECK_TOTAL_PERCENT_LIMIT: 
		fLimitTime = record.fLimitTotalPercent; 
		strReportName = "TotalPercentLimit";
		break;
		
	default:
		VALID_RET(!"정의되지 않은 테스트타입", "ERROR!!")
	}
	
	if (nFaultCount > 0)
		strReportName += " OverTime";
	else
		strReportName += " Success";
		
	char buff[512];
	sprintf_s(buff, "%s - %s(%d), Call: %d(%.2f%%), Min: %6.3f, Max: %6.3f, Avr: %6.3f, Limit:%6.3f",
		strReportName.c_str(),
		record.strName.c_str(), nProfileID,
		record.nCallCount, record.fTotalTime/m_Desc.fDurationTime*100.0f,
		record.fMinTime, record.fMaxTime, record.fTotalTime/record.nCallCount, fLimitTime);

	if (nFaultCount > 0)
	{
		sprintf_s(buff, "%s, Fault: %4.1f%%(%d/%d)",
			buff, 
			((float)nFaultCount/m_nTickCount)*100, nFaultCount, m_nTickCount);
	}

	return buff;
}