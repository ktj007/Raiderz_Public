#include "StdAfx.h"
#include "GGuildMember.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"

GGuildMember::GGuildMember(int64 nAID, int nCID, const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade)
: m_nAID(nAID)
, m_nCID(nCID)
, m_strName(szName)
, m_nLevel(nLevel)
, m_nGrade(nGrade)
, m_nFieldID(0)
, m_nChannelID(INVALID_CHANNELID)
{
	m_bOnline = gmgr.pPlayerObjectManager->IsExist(nCID);
}

GGuildMember::~GGuildMember()
{
}

void GGuildMember::OnLine(int nFieldID, int nChannelID)
{
	m_bOnline = true;
	m_nFieldID = nFieldID;
	m_nChannelID = nChannelID;
}

void GGuildMember::OffLine()
{
	m_bOnline = false;
	m_nFieldID = 0;
	m_nChannelID = 0;
}

void GGuildMember::MoveField( int nFieldID, int nChannelID )
{
	m_nFieldID = nFieldID;
	m_nChannelID = nChannelID;
}

void GGuildMember::ChangeGrade(GUILD_MEMBER_GRADE nGrade)
{
	m_nGrade = nGrade;
}

bool GGuildMember::IsOnline()
{
	return m_bOnline;
}

int GGuildMember::GetFieldID()
{
	return m_nFieldID;
}

int GGuildMember::GetChannelID()
{
	return m_nChannelID;
}