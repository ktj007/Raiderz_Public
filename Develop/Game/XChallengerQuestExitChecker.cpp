#include "StdAfx.h"
#include "XChallengerQuestExitChecker.h"
#include "XPost_Channel.h"
#include "XStrings.h"

#define CHALLENGER_QUEST_EXITTIME 180.f		//3분

XChallengerQuestExitChecker::XChallengerQuestExitChecker(void)
{
	Stop();
}

XChallengerQuestExitChecker::~XChallengerQuestExitChecker(void)
{
	Stop();
}

void XChallengerQuestExitChecker::Start()
{
	m_vecCheckTime.push_back(180);
	m_vecCheckTime.push_back(120);
	m_vecCheckTime.push_back(60);
	m_vecCheckTime.push_back(30);
	m_vecCheckTime.push_back(15);
	m_vecCheckTime.push_back(10);
	m_vecCheckTime.push_back(5);
	m_vecCheckTime.push_back(3);

	if (global.ui)
	{
		global.ui->OnPresentationUpper( XGetStr( L"CHALLENGERQUEST_COMPLETED"));
		global.ui->UI_Victory();
	}

	m_Regulator.SetTickTime(CHALLENGER_QUEST_EXITTIME);
	m_Regulator.Start();
}

void XChallengerQuestExitChecker::Stop()
{
	if (global.ui)
	{
		global.ui->UI_VictoryEnd();
	}

	m_vecCheckTime.clear();
	m_Regulator.Stop();
	m_bHide	= false;
}

bool XChallengerQuestExitChecker::CheckTime( int nTickTime )
{
	for (list<int>::iterator itor = m_vecCheckTime.begin();
		itor != m_vecCheckTime.end(); ++itor)
	{
		if (nTickTime <= (*itor)) return true;
	}

	return false;
}

void XChallengerQuestExitChecker::UIVictoryHide()
{
	if (m_bHide == true) return;

	int nTickTime = (int)(m_Regulator.GetTickTime() - m_Regulator.GetElapsedTime());

	if(nTickTime < 165)
	{
		if (global.ui)
		{
			global.ui->UI_VictoryEnd();
		}

		m_bHide = true;
	}
}

void XChallengerQuestExitChecker::Update( float fDelta )
{
	if(m_Regulator.IsActive() == false) return;

	if(m_Regulator.IsReady(fDelta))
	{
		Stop();
		XPostChannel_LeaveDynamicField();
	}
	else
	{
		UIVictoryHide();

		int nTickTime = (int)(m_Regulator.GetTickTime() - m_Regulator.GetElapsedTime());
		if(CheckTime(nTickTime) == false) return;

		int checktime = m_vecCheckTime.front();
		m_vecCheckTime.pop_front();

		XRemainTime RemainTime;
		RemainTime.Set(checktime*1000);

		// 1분 이상은 분 표시, 이하는 초 표시
		wstring strMsg = CSFormatString::Format( XGetStr( L"CHALLENGERQUEST_EXITFIELD"), FSParam(RemainTime.nMin, RemainTime.nSec));
		global.ui->OnPresentationUpper(strMsg);
	}
}
