#include "StdAfx.h"
#include "GPartyLootingRuleStorage.h"

GPartyLootingRuleStorage::GPartyLootingRuleStorage(void)
{
	m_nRoundRobinLastOrderCID = 0;
}

GPartyLootingRuleStorage::~GPartyLootingRuleStorage(void)
{
}

LOOTING_RULE_DATA GPartyLootingRuleStorage::GetLootingRuleData()
{
	return m_lootingRuleData;
}

int GPartyLootingRuleStorage::GetRoundRobinLastOrderCID()
{
	return m_nRoundRobinLastOrderCID;
}

void GPartyLootingRuleStorage::SetLootingRuleData(const LOOTING_RULE_DATA& lootingRuleData)
{
	m_lootingRuleData = lootingRuleData;
}

void GPartyLootingRuleStorage::SetRoundRobinLastOrderCID(int nLastOrderCID)
{
	m_nRoundRobinLastOrderCID = nLastOrderCID;
}
