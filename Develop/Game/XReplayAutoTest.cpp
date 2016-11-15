#include "stdafx.h"
#include "XReplayAutoTest.h"
#include "XBaseApplication.h"
#include "MMaietTestResult.h"
#include "MHttp.h"
#include "MUnitTestRunner.h"
#include "MLocale.h"
#include "XPath.h"

XReplayAutoTest::XReplayAutoTest() : m_bDoing(false), m_bReplayScreenShot(false)
{
	char szFileName[512]="";
	string szPath = MLocale::ConvUTF16ToAnsi(XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SAVE).c_str());
	sprintf_s(szFileName, "%s\\ReplayTestResult.xml", szPath.c_str());	

	m_pResultStream = new std::ofstream(szFileName);
	m_pTestReporter = new MMaietXmlTestReporter(*m_pResultStream);
}

XReplayAutoTest::~XReplayAutoTest()
{
	SAFE_DELETE(m_pTestReporter);
	SAFE_DELETE(m_pResultStream);
}

void XReplayAutoTest::End()
{
	if (m_bDoing == false) return;

	CollectData(RTCDT_GAME_END);


	if (m_bReplayScreenShot == false)
	{
		OutputReport();
		ReportToHTTPServer();
	}

	m_bDoing = false;
}

void XReplayAutoTest::CollectData( XReplayTestCollectDataType nDataType )
{
	if (!m_bDoing) return;

	switch (nDataType)
	{
	case RTCDT_LOADING_START:
		{
			m_Data.m_nLoadingStartTime = XGetNowTime();
		}
		break;
	case RTCDT_LOADING_END:
		{
			m_Data.m_nLoadingEndTime = XGetNowTime();
		}
		break;
	case RTCDT_GAME_START:
		{
			m_Data.m_nGameStartTime = XGetNowTime();
		}
		break;
	case RTCDT_GAME_END:
		{
			m_Data.m_nGameEndTime = XGetNowTime();
		}
		break;
	case RTCDT_GAME_TICK:
		{
			m_Data.m_nTotalFrame++;
		}
		break;
	}
}

void XReplayAutoTest::Start(bool bReplayScreenShot, XReplayTestBaseline baseline)
{
	m_Data.Clear();
	m_bDoing = true;
	m_bReplayScreenShot = bReplayScreenShot;
	m_Baseline = baseline;

	CollectData(RTCDT_GAME_START);
}

void XReplayAutoTest::OutputReport()
{
	char text[512];

	float fLoadingTime = m_Data.GetLoadingTime();
	float fGameTime = m_Data.GetGameTime();
	uint32 fps = m_Data.GetAverageFPS();

	if (m_Baseline.nFPS > 0)
	{
		sprintf_s(text, "Baseline FPS = %u", m_Baseline.nFPS);
		m_pTestReporter->AddMessage(text);

		m_pTestReporter->AddMessage("-----------------------------------------------------------");

	}

	sprintf_s(text, "LoadingTime = %.1f Sec", fLoadingTime);
	m_pTestReporter->AddMessage(text);

	sprintf_s(text, "PlayTime = %.1f Sec", fGameTime);
	m_pTestReporter->AddMessage(text);

	sprintf_s(text, "FPS = %u", fps);
	m_pTestReporter->AddMessage(text);

	MMaietTestResultList	testResulList;

	m_pTestReporter->ReportSummary(0, 0, 0, fGameTime, testResulList);

}

void XReplayAutoTest::ReportToHTTPServer()
{
	char url[5000];
	char out[65535];

	sprintf_s(url, "http://icecream/monitor/report_replay.php?computer=%s&fps=%d&LoadingTime=%d", MLocale::ConvUTF16ToAnsi(m_Data.GetComputerName().c_str()).c_str(), (int)m_Data.GetAverageFPS(), (int)m_Data.GetLoadingTime());

	if (MHTTP_Get(url, out, 65535))
	{
		mlog("replay report : %s\n", url);
	}
	else
	{
		mlog("replay report failed: %s\n", url);
	}
}

void XReplayAutoTest::Fail( const wchar_t* szMsg )
{
	m_pTestReporter->AddMessage(MLocale::ConvUTF16ToUTF8(szMsg).c_str());
}

wstring XReplayTestData::GetComputerName()
{
	wchar_t    szBuffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD   dwNameSize = MAX_COMPUTERNAME_LENGTH + 1;

	::GetComputerName(szBuffer, &dwNameSize);
	return wstring(szBuffer);
}