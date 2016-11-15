#ifndef _GDBTASK_DATA_ACCOUNT_H
#define _GDBTASK_DATA_ACCOUNT_H


class GDBT_ACC_INSERT
{
public :
	GDBT_ACC_INSERT() : m_uidPlayer(0) {}
	GDBT_ACC_INSERT(const MUID& uidPlayer, const wstring& strUSER_ID, const wstring& strSITE_CODE, const wstring& strSITE_USER_ID, const wstring strPWD)
		: m_uidPlayer(uidPlayer), m_strUSER_ID(strUSER_ID), m_strSITE_CODE(strSITE_CODE), m_strSITE_USER_ID(strSITE_USER_ID), m_strPWD(strPWD) {}

	MUID	m_uidPlayer;
	wstring m_strUSER_ID;
	wstring m_strSITE_CODE;
	wstring m_strSITE_USER_ID;
	wstring m_strPWD;
};


class GDBT_ACC_LOGIN
{
public :
	GDBT_ACC_LOGIN() : m_uidPlayer(0) {}
	GDBT_ACC_LOGIN(const MUID& uidPlayer, const wstring& strUSER_ID, const wstring& strSITE_CODE, const wstring& strSITE_USER_ID, const wstring& strPWD, const string& strIP)
		: m_uidPlayer(uidPlayer), m_strUSER_ID(strUSER_ID), m_strSITE_CODE(strSITE_CODE), m_strSITE_USER_ID(strSITE_USER_ID), m_strPWD(strPWD), m_strIP(strIP) {}

	MUID	m_uidPlayer;
	wstring m_strUSER_ID;
	wstring m_strSITE_CODE;
	wstring m_strSITE_USER_ID;
	wstring m_strPWD;
	string	m_strIP;	
};


#endif