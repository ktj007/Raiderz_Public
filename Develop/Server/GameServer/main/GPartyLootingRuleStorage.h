#pragma once

#include "CSDef_Loot.h"

enum LOOTING_RULE_COMMON;
enum LOOTING_RULE_RARE;
enum LOOTING_RULE_RARE_FOR;

struct LOOTING_RULE_DATA;

class GPartyLootingRuleStorage : public MTestMemPool<GPartyLootingRuleStorage>
{
public:
	GPartyLootingRuleStorage(void);
	~GPartyLootingRuleStorage(void);

	LOOTING_RULE_DATA GetLootingRuleData();
	CID GetRoundRobinLastOrderCID();

	void SetLootingRuleData(const LOOTING_RULE_DATA& lootingRuleData);
	void SetRoundRobinLastOrderCID(CID nLastOrderCID);

private:
	LOOTING_RULE_DATA	m_lootingRuleData;
	CID					m_nRoundRobinLastOrderCID;
};

