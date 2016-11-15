#pragma once

// 일반 전리품 획득
enum LOOTING_RULE_COMMON
{
	LRC_NONE = 0,

	LRC_FREE_FOR_ALL,
	LRC_ROUND_ROBIN,
	LRC_MASTER_LOOT,

	LRC_MAX
};

// 고급 전리품 획득
enum LOOTING_RULE_RARE
{
	LRR_NONE = 0,

	LRR_FREE_FOR_ALL,
	LRR_ROLL_DICE,
	LRR_MASTER_LOOT,

	LRR_MAX
};

// 고급 전리품 획득 적용 대상 아이템 등급
enum LOOTING_RULE_RARE_FOR
{
	LRRF_NONE = 0,

	LRRF_RARE_OR_HIGHER,
	LRRF_LEGENDARY_OR_HIGHER,
	LRRF_EPIC,

	LRRF_MAX
};

struct LOOTING_RULE_DATA
{
	LOOTING_RULE_DATA()
	: m_nLRC(LRC_ROUND_ROBIN)
	, m_nLRR(LRR_ROLL_DICE)
	, m_nLRRF(LRRF_RARE_OR_HIGHER)
	{
	}


	LOOTING_RULE_DATA(LOOTING_RULE_COMMON nLRC,	LOOTING_RULE_RARE nLRR, LOOTING_RULE_RARE_FOR nLRRF)
	: m_nLRC(nLRC)
	, m_nLRR(nLRR)
	, m_nLRRF(nLRRF)
	{
	}

	bool operator==(const LOOTING_RULE_DATA& lootingRuleData)
	{
		if (m_nLRC != lootingRuleData.m_nLRC) return false;
		if (m_nLRR != lootingRuleData.m_nLRR) return false;
		if (m_nLRRF != lootingRuleData.m_nLRRF) return false;

		return true;
	}

	bool operator!=(const LOOTING_RULE_DATA& lootingRuleData)
	{
		if (*this == lootingRuleData) return false;

		return true;
	}

	LOOTING_RULE_COMMON		m_nLRC;				// 일반 전리품 루팅룰
	LOOTING_RULE_RARE		m_nLRR;				// 고급 전리품 루팅룰
	LOOTING_RULE_RARE_FOR	m_nLRRF;			// 고급 전리품 루팅룰 적용 대상
};

enum DROP_CAULE
{
	DC_NONE = 0,

	DC_KILL,
	DC_INTERACT_MORTAL,
	DC_INTERACT_IMMORTAL,
	DC_GATHER,

	DC_MAX
};


enum DROP_ITEM_TYPE
{
	DIT_NONE = 0,

	DIT_GETTABLE,
	DIT_ROLLABLE,
	DIT_MASTERLOOTABLE,

	DIT_MAX
};

enum UNGETTABLE_ITEM_REASON
{
	UIR_NONE = 0,
	UIR_OTHERSERVER,
	UIR_OTHERFIELD,
	UIR_UNADDABLEITEM,
	UIR_MAX
};