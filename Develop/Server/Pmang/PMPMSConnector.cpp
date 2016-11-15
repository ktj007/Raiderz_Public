#include "StdAfx.h"
#include "PMPMSConnector.h"
#include "PMSConn.h"
#include "PmRegionStat.h"
#include "PMServerInitLogger.h"
#include "MLocale.h"

//////////////////////////////////////////////////////////////////////////
//
//	PMPMSConnector::TokenPszConverter
//
//////////////////////////////////////////////////////////////////////////

PMPMSConnector::TokenPszConverter::TokenPszConverter( const wstring strCommandLine ) : pszTokens(NULL)
{
	wstring delimiters = L" ";
	wstring::size_type lastPos = strCommandLine.find_first_not_of(delimiters, 0);
	wstring::size_type pos = strCommandLine.find_first_of(delimiters, lastPos);

	while (wstring::npos != pos || wstring::npos != lastPos)
	{
		m_vecTokens.push_back(strCommandLine.substr(lastPos, pos - lastPos));
		lastPos = strCommandLine.find_first_not_of(delimiters, pos);
		pos = strCommandLine.find_first_of(delimiters, lastPos);
	}

	if (!m_vecTokens.empty())
	{
		pszTokens = new LPSTR[Argc()];
		for (size_t i=0; i<m_vecTokens.size(); ++i)
		{
			int size = m_vecTokens[i].length()+1;
			pszTokens[i] = new char[size];

			wsprintfA(pszTokens[i], "%S", m_vecTokens[i].c_str());			
		}
	}
}

PMPMSConnector::TokenPszConverter::~TokenPszConverter()
{
	if (!m_vecTokens.empty())
	{
		for (size_t i=0; i<Argc(); ++i)
		{
			SAFE_DELETE_ARRAY(pszTokens[i]);
		}
		SAFE_DELETE_ARRAY(pszTokens);
	}
}

DWORD PMPMSConnector::TokenPszConverter::Argc() const
{
	return m_vecTokens.size();
}

LPSTR* PMPMSConnector::TokenPszConverter::Argv() const
{
	return pszTokens;
}



//////////////////////////////////////////////////////////////////////////
//
//	PMPMSConnector
//
//////////////////////////////////////////////////////////////////////////

PMPMSConnector::PMPMSConnector()
: m_bConnected(false)
, m_dwSSN(0)
, m_dwCategory(0)
{
	m_pRegionStat = new PmRegionStat();
}

PMPMSConnector::~PMPMSConnector()
{	
	m_bConnected = false;
	SAFE_DELETE(m_pRegionStat);
}

void PMPMSConnector::SetGameInfo( DWORD dwSSN, DWORD dwCategory )
{
	m_dwSSN = dwSSN;
	m_dwCategory = dwCategory;
}

DWORD PMPMSConnector::Connect( DWORD argc, LPSTR argv[], IPMSObject* pCallbackObj )
{
	if (m_bConnected)
		return PMSC_ERR_RUN_ALREADY; // 이미 연결되어 있음

	// 초기화 ------------------------
	DWORD nRet = ::PMSInitConn(argc, argv);

	if (nRet != PMSC_ERR_OK)
		return nRet; // 초기화 실패

	// 연결 ------------------------
	_ASSERT(pCallbackObj);
	nRet =
		::PMSRunConn(pCallbackObj);

	if (nRet != PMSC_ERR_OK)
		return nRet; // 연결 실패

	// 연결됨 ------------------------
	OnConnected();

	return PMSC_ERR_OK;
}

DWORD PMPMSConnector::Connect( const wstring strCommandLine )
{
	TokenPszConverter token_converter(strCommandLine);

	DWORD nRet =
		Connect(token_converter.Argc(), token_converter.Argv(), GetPMSObject());

	return nRet;
}

void PMPMSConnector::Stop()
{
	if (!m_bConnected)
		return; // 연결되어 있지 않음

	::PMSStopConn();

	OnStopped();
}

void PMPMSConnector::OnConnected()
{
	m_bConnected = true;
}

void PMPMSConnector::OnStopped()
{
	m_bConnected = false;
}

BOOL PMPMSConnector::SendWarningMsg( DWORD dwErrLvl, LPCTSTR pszWarningMsg, LPCTSTR pszTreatMsg ) const
{
	return ::PMSSendWarningMsg(dwErrLvl, MLocale::ConvTCHARToAnsi(pszWarningMsg).c_str(), MLocale::ConvTCHARToAnsi(pszTreatMsg).c_str(), m_dwSSN, m_dwCategory);
}

void PMPMSConnector::SendWarningMsg() const
{
	SendWarningMsg(::GetServerInitResultErrLevel(), ::GetServerInitWarningMsg(), ::GetServerInitTreatMsg());
}

LPCSTR PMPMSConnector::GetConfigFileName()
{
	return ::PMSGetConfigFileName();
}

DWORD PMPMSConnector::GetStatus()
{
	return ::PMSGetStatus();
}

DWORD PMPMSConnector::GetSSN() const
{
	return m_dwSSN;
}

DWORD PMPMSConnector::GetCategory() const
{
	return m_dwCategory;
}

PmRegionStat* PMPMSConnector::GetRegionStat() const
{
	return m_pRegionStat;
}
