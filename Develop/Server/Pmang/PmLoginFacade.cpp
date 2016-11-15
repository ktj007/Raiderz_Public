#include "stdafx.h"
#include "PmLoginFacade.h"
#include "PmLoginTimeChecker.h"
#include "PmAuthenticator.h"
#include "PmLoginStreamParser.h"
#include "PmDef.h"

PmLoginFacade::PmLoginFacade(wstring strServerKey, uint32 nToleranceSeconds)
{
	m_pTimeChecker = new PmLoginTimeChecker(nToleranceSeconds);
	m_pAuthenticator = new PmAuthenticator(strServerKey);
}

PmLoginFacade::~PmLoginFacade()
{
	delete m_pTimeChecker;
	delete m_pAuthenticator;
}

bool PmLoginFacade::Login(wstring strLoginStream, PmUserData* poutUserData)
{
	// 로그인 문자열 유효성 검사
	PmLoginStreamParser parser(strLoginStream);

	if (!parser.Parse())
	{
		return false;
	}


	// 로그인 요청 시간 검사
	time_t timeLogin = parser.GetTimeStamp();

	time_t timeCurrent;
	time(&timeCurrent);

	if (!m_pTimeChecker->Verify(timeLogin, timeCurrent))
	{
		return false;
	}


	// 로그인 해쉬코드 검사
	wstring strHashKeyStream = parser.GetHashKeyStream();
	wstring strHashedValue = parser.GetHashedValue();

	if (!m_pAuthenticator->Verify(strHashedValue, strHashKeyStream))
	{
		return false;
	}	


	// 사용자 정보 기입
	if (poutUserData != NULL)
	{
		*poutUserData = parser.GetUserData();
	}


	return true;
}
