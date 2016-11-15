#pragma once

class XPvPAreaInfo
{
private:
	bool			m_bInPvPAreaInfo;

	struct XPvPAreaTeamInfo
	{
		uint8			m_nFaction;
		wstring			m_strFactionName;
		void Clear()
		{
			m_nFaction = 0;
			m_strFactionName = L"";
		}
	};

	XPvPAreaTeamInfo	m_Team1;
	XPvPAreaTeamInfo	m_Team2;

	void Clear();
public:
	XPvPAreaInfo();
	void SetInPvPArea(uint8 nFaction1, wstring& strFactionName1, uint8 nFaction2, wstring& strFactionName2);
	void SetOutPvPArea();
	bool IsInPvPArea() { return m_bInPvPAreaInfo; }
	wstring GetTeam1FactionName() { return m_Team1.m_strFactionName; }
	wstring GetTeam2FactionName() { return m_Team2.m_strFactionName; }
};