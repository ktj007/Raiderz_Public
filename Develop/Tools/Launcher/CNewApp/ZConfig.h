#pragma once

class ZConfig
{
public:
	/// CONFIG
	static bool		m_bHideWebPage;
	static string	m_strWebPageURL;

	/// LAUNCH
	static bool		m_bStaticLaunch;
	static string	m_strStaticServerName;		///< ServerName
	static string	m_strStaticIPAddress;		///< IP Address
	static bool		m_bSkipSelectWorldForPWE;	///< Skip Select World For PWE

	///	PATCH
	static bool		m_bSkipUpdate;			///< 업데이트를 건너 뛰는지 확인
	static string	m_strPatchServAddr;		///< 패치 서버 주소
	static string	m_strPatchServDir;		///< 패치 서버 가상 디렉터리


	static bool		Init();

};
