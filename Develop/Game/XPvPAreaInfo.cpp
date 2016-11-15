#include "stdafx.h"
#include "XPvPAreaInfo.h"

XPvPAreaInfo::XPvPAreaInfo()
{
	Clear();
}

void XPvPAreaInfo::SetInPvPArea( uint8 nFaction1, wstring& strFactionName1, uint8 nFaction2, wstring& strFactionName2 )
{
	m_bInPvPAreaInfo = true;

	m_Team1.m_nFaction = nFaction1;
	m_Team1.m_strFactionName = strFactionName1;

	m_Team2.m_nFaction = nFaction2;
	m_Team2.m_strFactionName = strFactionName2;
}

void XPvPAreaInfo::SetOutPvPArea()
{
	Clear();
}

void XPvPAreaInfo::Clear()
{
	m_bInPvPAreaInfo = false;
	m_Team1.Clear();
	m_Team2.Clear();
}