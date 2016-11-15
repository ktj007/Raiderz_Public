#pragma once

#include "CSDef_Loot.h"

#define PARTY_NAME_LEN		32							// 파티 이름 최대 길이

struct PARTY_SETTING
{
	PARTY_SETTING()
	: m_uidLeader(MUID::ZERO)
	, m_nAutoPartyQuestID(INVALID_ID)
	{		
		memset(m_szName, 0, PARTY_NAME_LEN+1);
	}

	PARTY_SETTING(const MUID& uidLeader, const LOOTING_RULE_DATA& lootingRuleData, const tstring& strName)
	: m_uidLeader(uidLeader)
	, m_lootingRuleData(lootingRuleData)
	, m_nAutoPartyQuestID(INVALID_ID)
	{
		_stprintf_s(m_szName, PARTY_NAME_LEN+1, strName.c_str());
	}

	bool operator==(const PARTY_SETTING& partySetting)
	{
		if (m_uidLeader != partySetting.m_uidLeader) return false;
		if (m_lootingRuleData != partySetting.m_lootingRuleData) return false;
		if (0 != _tcscmp(m_szName, partySetting.m_szName)) return false;
		if (m_nAutoPartyQuestID != partySetting.m_nAutoPartyQuestID) return false;

		return true;
	}

	void Init()
	{
		memset(m_szName, 0, PARTY_NAME_LEN+1);
		m_uidLeader = MUID::ZERO;
	}

	int					m_nAutoPartyQuestID;
	MUID				m_uidLeader;
	LOOTING_RULE_DATA	m_lootingRuleData;
	TCHAR				m_szName[PARTY_NAME_LEN+1];
};
