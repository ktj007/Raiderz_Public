#pragma once

#include "GServerSystems.h"
#include "GServerManagers.h"
#include "MMaietXmlTestReporter.h"
#include "GGameServerProfiler.h"
#include "GTickRegulator.h"

#define PROFILER_NAME "GameServerProfiler"

class Profiler
{
private:
	enum CheckType
	{
		CHECK_MAXLIMIT,
		CHECK_AVRLIMIT,
		CHECK_TOTAL_PERCENT_LIMIT,
	};
public:
	struct Description
	{
		string	strName;
		float	fStartTime;				///< 테스트 시작 시간 (단위: 초)
		float	fDurationTime;			///< 테스트 처리 시간, 시작시간부터 계산 (단위: 초)
		string  strOutputFilename;		///< 출력될 파일명
		bool	bAILODEnable;
		bool	bReportMode;			///< 보고서만 출력하는 모드인지 여부

		Description()
			: strName("untitled_profilename")
			, fStartTime(0.0f)
			, fDurationTime(0.0f)
			, strOutputFilename("GameServerProfile")
			, bAILODEnable(true)
			, bReportMode(false)
		{}
	};

	struct Record
	{
		string strName;
		// 틱당 최대 제한시간
		float fLimitMaxTime;
		// 틱당 평균 제한시간
		float fLimitAvrTime;
		// 전체 시간중의 비중
		float fLimitTotalPercent;
		float fMinTime;
		float fMaxTime;
		float fTotalTime;
		int	  nCallCount;

		Record()
			: fLimitMaxTime(0.0f)
			, fLimitAvrTime(0.0f)
			, fLimitTotalPercent(0.0f)
			, fMinTime(0.0f)
			, fMaxTime(0.0f)
			, fTotalTime(0.0f)
			, nCallCount(0)
		{}
	};

public:
	Profiler(void);
	~Profiler(void);

	bool LoadConfig(string strConfigFileName);
	bool Init();
	void Fini();
	void Run();
	int IsFail();
private:
	void EndProfile();
	void BeginProfile();
	void CheckMaxLimitation(float fElapsedTime);
	void CheckAvrLimitation();
	void CheckTotalPercentLimitation();
	void SaveReport();
	string MakeReportString( CheckType nCheckType, const Record &record, int nProfileID, int nFaultCount );
	bool IsMeasurable(float fDelta);
	bool IsExit(float fDelta);
	void IncreaseMaxLimitFault(int nProfileID);
	void IncreaseAvrLimitFault( int nProfileID );
	void IncreaseTotalPercentLimitFault( int nProfileID );
	void AddMessage( const char* pFormat, ... );
	void OnTick(float fDelta);
	void OnUpdate(float fDelta);
	void OnMeasurement(float fDelta);
	void PrintElapsedTime(float fDelta);
private:
	typedef map<int,Record>	RecordMap;
	typedef map<int,int>	FaultMap;
private:
	MMaietTestResultList	m_TestResulList;
	Description				m_Desc;
	GServerSystems			m_TestSystems;
	GServerManagers			m_TestManagers;
	GGameServerProfiler		m_TickProfiler;
	MRegulator				m_rgrStart;
	MRegulator				m_rgrDuration;
	GTickRegulator			m_rgrTickElasedTime;
	unsigned int			m_nLastTime;
	int						m_nTickCount;
	RecordMap				m_mapRecords;
	FaultMap				m_mapMaxLimitFaults;
	FaultMap				m_mapAvrLimitFaults;
	FaultMap				m_mapTotalPercentLimitFaults;
	MMaietXmlTestReporter*	m_pReporter;
	ofstream*				m_pFileStream;
	
};
