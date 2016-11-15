#include "StdAfx.h"
#include "XGuildMember.h"

XGuildMember::XGuildMember(const wchar_t* szName, int nLevel, GUILD_MEMBER_GRADE nGrade, bool bOnline, int nFieldID, int nChannelID)
: m_strName(szName)
, m_nLevel(nLevel)
, m_nGrade(nGrade)
, m_bOnline(bOnline)
, m_nFieldID(nFieldID)
, m_nChannelID(0)
{
}

XGuildMember::~XGuildMember()
{
}
