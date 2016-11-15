#pragma once

#include "MTstring.h"

class SNetworkCardInfo
{
public:
	SNetworkCardInfo(const wchar_t* szIP) : m_nID(0), m_strIP(szIP)	{}

	void	SetID(int nID)	{ m_nID = nID; }
	int		GetID()			{ return m_nID; }
	wstring	GetIP()			{ return m_strIP; }

private:
	int		m_nID;
	wstring	m_strIP;
};

class SNetworkCardMgr
{
public:
	~SNetworkCardMgr();

	bool	Init();
	void	Clear();

	bool	RegCard(wstring strIP, int nID);

	int		GetNetworkCardCount()		{ return (int)(m_listNetCardInfo_Reg.size() + m_listNetCardInfo_Unreg.size()); }
	int		GetRegistedNetworkCardCount()	{ return (int)m_listNetCardInfo_Reg.size(); }
	wstring	GetIP(int nNetworkCardID = 0);

	static SNetworkCardMgr* GetInstance();

private:
	SNetworkCardMgr();

	list<SNetworkCardInfo>	m_listNetCardInfo_Reg;
	list<SNetworkCardInfo>	m_listNetCardInfo_Unreg;
};

/// 인스턴스를 구함
inline SNetworkCardMgr* GetNetworkCardMgr()
{
	return SNetworkCardMgr::GetInstance();
}
