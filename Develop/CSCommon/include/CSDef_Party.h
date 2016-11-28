#pragma once

#include "CSDef_Loot.h"

#define PARTY_NAME_LEN		64							// 파티 이름 최대 길이

struct PARTY_SETTING
{
	PARTY_SETTING()
	: m_uidLeader(MUID::ZERO)
	, m_nAutoPartyQuestID(INVALID_ID)
	, m_bPublicParty(false)
	{		
		memset(m_szName, 0, sizeof(m_szName));
	}

	PARTY_SETTING(const MUID& uidLeader, const LOOTING_RULE_DATA& lootingRuleData, const bool bPublicParty, const tstring& strPartyName)
	: m_uidLeader(uidLeader)
	, m_lootingRuleData(lootingRuleData)
	, m_nAutoPartyQuestID(INVALID_ID)
	, m_bPublicParty(bPublicParty)
	{
		_stprintf_s(m_szName, PARTY_NAME_LEN+1, strPartyName.c_str());
	}

	bool operator==(const PARTY_SETTING& partySetting)
	{
		if (m_uidLeader != partySetting.m_uidLeader) return false;
		if (m_lootingRuleData != partySetting.m_lootingRuleData) return false;
		if (m_bPublicParty != partySetting.m_bPublicParty) return false;
		if (0 != _tcscmp(m_szName, partySetting.m_szName)) return false;
		if (m_nAutoPartyQuestID != partySetting.m_nAutoPartyQuestID) return false;

		return true;
	}

	void Init()
	{
		memset(m_szName, 0, sizeof(m_szName));
		m_uidLeader = MUID::ZERO;
	}

	int					m_nAutoPartyQuestID;
	MUID				m_uidLeader;
	LOOTING_RULE_DATA	m_lootingRuleData;
	bool				m_bPublicParty;
	TCHAR				m_szName[PARTY_NAME_LEN+1];
};
