#pragma once

#include "MMaietXmlTestReporter.h"
#include <fstream>

// 로딩 시간
// 게임 플레이 시간
// 총 프레임
// 평균 프레임
// 초 별 프레임
// 맵 이름

class XReplayTestData
{
public:
	uint32 m_nLoadingStartTime;
	uint32 m_nLoadingEndTime;
	uint32 m_nGameStartTime;
	uint32 m_nGameEndTime;
	int m_nTotalFrame;
public:
	XReplayTestData()
	{
		Clear();
	}
	void Clear() 
	{
		m_nTotalFrame = 0;
		m_nLoadingStartTime = 0;
		m_nLoadingEndTime = 0;
		m_nGameStartTime = 0;
		m_nGameEndTime = 0;
	}
	int GetTotalFrame()
	{
		return m_nTotalFrame;
	}
	uint32 GetAverageFPS()
	{
		if (m_nGameEndTime == 0) return 0;

		uint32 nElapsedTime = (m_nGameEndTime - m_nGameStartTime) / 1000;
		return (m_nTotalFrame / nElapsedTime);
	}
	float GetLoadingTime()
	{
		float fTime = float(m_nLoadingEndTime - m_nLoadingStartTime) / 1000.0f;
		
		return fTime;
	}
	float GetGameTime()
	{
		float fTime = float(m_nGameEndTime - m_nGameStartTime) / 1000.0f;
		return fTime;
	}
	wstring GetComputerName();
};

struct XReplayTestBaseline
{
	uint32		nFPS;

	XReplayTestBaseline() : nFPS(0) {}
};

enum XReplayTestCollectDataType
{
	RTCDT_LOADING_START = 0,
	RTCDT_LOADING_END,
	RTCDT_GAME_START,
	RTCDT_GAME_END,
	RTCDT_GAME_TICK
};

class XReplayAutoTest
{
private:
	XReplayTestData		m_Data;
	XReplayTestBaseline	m_Baseline;	// 베이스 라인을 못넘으면 테스트 실패
	bool m_bDoing;
	bool m_bReplayScreenShot;

	MMaietXmlTestReporter*	m_pTestReporter;
	std::ofstream*			m_pResultStream;

	void OutputReport();
	void ReportToHTTPServer();
public:
	XReplayAutoTest();
	~XReplayAutoTest();
	void Start(bool bReplayScreenShot, XReplayTestBaseline baseline=XReplayTestBaseline());
	void End();
	void Fail(const wchar_t* szMsg);
	void CollectData(XReplayTestCollectDataType nDataType);
};
