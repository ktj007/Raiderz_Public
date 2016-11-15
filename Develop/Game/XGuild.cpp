#include "stdafx.h"
#include "XGuild.h"
#include "XGuildMember.h"

XGuild::XGuild()
: m_nStorageMoney(0)
{

}

XGuild::~XGuild()
{
	Clear();
}

void XGuild::Clear()
{
	m_strName.clear();
	m_strMasterName.clear();
	SAFE_DELETE_MAP(m_mapMember);
	m_nStorageMoney = 0;
}

void XGuild::InsertMember(const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade, bool bOnline, int nFieldID, int nChannelID)
{
	if (NULL == szName) return;

	XGuildMember* pNewMember = new XGuildMember(szName, nLevel, nGrade, bOnline, nFieldID, nChannelID);
	m_mapMember.insert(MAP_GUILD_MEMBER::value_type(szName, pNewMember));
}

void XGuild::DeleteMember(const wchar_t* szName)
{
	if (NULL == szName) return;

	MAP_GUILD_MEMBER::iterator itor = m_mapMember.find(szName);
	if (m_mapMember.end() == itor) return;

	SAFE_DELETE((*itor).second);
	m_mapMember.erase(itor);
}

XGuildMember* XGuild::GetMember(const wchar_t* szName)
{
	if (NULL == szName) return NULL;

	MAP_GUILD_MEMBER::iterator itor = m_mapMember.find(szName);
	if (m_mapMember.end() == itor) return NULL;

	return (*itor).second;
}

bool XGuild::GetIndex( size_t nIndex, XGuildMember** ppOutMember)
{
	MAP_GUILD_MEMBER::iterator itr = m_mapMember.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++)
	{
		if ( itr == m_mapMember.end())		return false;
	}
	*ppOutMember = (*itr).second;
	return true;
}

bool XGuild::IsExist()
{
	if (true == m_strName.empty()) return false;

	return true;
}

void XGuild::OnLine(const wchar_t* szName, int nFieldID, int nChanneldID)
{
	XGuildMember* pGuildMember = GetMember(szName);
	if (NULL == pGuildMember) return;

	pGuildMember->m_bOnline = true;
	pGuildMember->m_nFieldID = nFieldID;
	pGuildMember->m_nChannelID = nChanneldID;
}

void XGuild::OffLine(const wchar_t* szName)
{
	XGuildMember* pGuildMember = GetMember(szName);
	if (NULL == pGuildMember) return;

	pGuildMember->m_bOnline = false;
	pGuildMember->m_nFieldID = 0;
	pGuildMember->m_nChannelID = 0;
}

void XGuild::MoveField( const wchar_t* szName, int nFieldID, int nChanneldID )
{
	XGuildMember* pGuildMember = GetMember(szName);
	if (NULL == pGuildMember) return;

	pGuildMember->m_bOnline = true;
	pGuildMember->m_nFieldID = nFieldID;
	pGuildMember->m_nChannelID = nChanneldID;
}

void XGuild::ChangeGuildMaster(const wchar_t* szOldMasterName, const wchar_t* szNewMasterName)
{
	if (NULL == szOldMasterName) return;
	if (NULL == szNewMasterName) return;

	XGuildMember* pOldMasterMember = GetMember(szOldMasterName);
	if (NULL == pOldMasterMember) return;
	XGuildMember* pNewMasterMember = GetMember(szNewMasterName);
	if (NULL == pNewMasterMember) return;

	pOldMasterMember->m_nGrade = GMG_NORMAL;
	pNewMasterMember->m_nGrade = GMG_MASTER;

	m_strMasterName = szNewMasterName;
}

wstring XGuild::GetMemberList()
{
	wstring memberlist;
	MAP_GUILD_MEMBER::iterator itor;
	for (itor = m_mapMember.begin(); itor != m_mapMember.end();)
	{
		XGuildMember* pGuildMember = (*itor).second;

		memberlist += pGuildMember->m_strName;
		itor++;

		if (itor != m_mapMember.end())
		{
			memberlist += L",";
		}
	}

	return memberlist;
}

void XGuild::SetStorageMoney( int nMoney )
{
	if (MAX_MONEY < nMoney)
	{
		m_nStorageMoney = MAX_MONEY;
	}
	else if (0 > nMoney)
	{
		m_nStorageMoney = 0;
	}
	else
	{
		m_nStorageMoney = nMoney;
	}
}
