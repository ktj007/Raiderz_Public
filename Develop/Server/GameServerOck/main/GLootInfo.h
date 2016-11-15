#ifndef _GLOOT_INFO_H_
#define _GLOOT_INFO_H_

#include "CSSoul.h"

struct LOOT_ITEM_INFO : public MTestMemPool<LOOT_ITEM_INFO>
{
	int		m_nID;				// 아이템의 ID
	float	m_fRate;			// 아이템이 나올 확률
	int		m_nMin;				// 아이템이 떨어지는 최소 갯수
	int		m_nMax;				// 아이템이 떨어지는 최대 갯수
	int		m_nBreakPart;		// 브레이커블 파츠가 파괴되었을때 드랍되는 아이템.
	int		m_nQuestPVPFieldID;	// 퀘스트PVP 지역에서만 드랍되는 아이템
	vector<int>	m_vecQuestID;	// 퀘스트가 하나라도 있는 파티원에게만 드랍되는 아이템	

	LOOT_ITEM_INFO()
	: m_nID(0)
	, m_fRate(100.0f)
	, m_nMin(1)
	, m_nMax(1)
	, m_nBreakPart(0)
	, m_nQuestPVPFieldID(false)
	{

	}
};

struct LOOT_MONEY_INFO : public MTestMemPool<LOOT_MONEY_INFO>
{
	float	m_fRate;			// 실버가 나올 확률
	int		m_nMin;				// 실버가 떨어지는 최소량
	int		m_nMax;				// 실버가 떨어지는 최대량

	LOOT_MONEY_INFO()
	: m_fRate(100.0f)
	, m_nMin(1)
	, m_nMax(1)
	{

	}
};

struct LOOT_SOUL_INFO : public MTestMemPool<LOOT_SOUL_INFO>
{
	SOUL_TYPE m_nType;

	float	m_fRate;		// 영혼이 나올 확률
	float	m_fBPart1Rate;	// 파츠1 파괴시 확률 증가량
	float	m_fBPart2Rate;	// 파츠2 파괴시 확률 증가량
	float	m_fBPart3Rate;	// 파츠3 파괴시 확률 증가량
	float	m_fBPart4Rate;	// 파츠4 파괴시 확률 증가량

	int		m_nMin;				// 영혼이 떨어지는 최소량
	int		m_nMax;				// 영혼이 떨어지는 최대량

	LOOT_SOUL_INFO()
	: m_nType(static_cast<SOUL_TYPE>(0))
	, m_fRate(100.0f)
	, m_fBPart1Rate(0.0f)
	, m_fBPart2Rate(0.0f)
	, m_fBPart3Rate(0.0f)
	, m_fBPart4Rate(0.0f)
	, m_nMin(1)
	, m_nMax(1)
	{

	}
};

typedef vector<LOOT_ITEM_INFO*> LOOT_ITEM_VEC;
typedef vector<LOOT_MONEY_INFO*> LOOT_MONEY_VEC;
typedef vector<LOOT_SOUL_INFO*> LOOT_SOUL_VEC;
class GLootInfo : public MTestMemPool<GLootInfo>
{
public:
	bool m_bBPartLoot;
	LOOT_ITEM_VEC m_vecLootItems;
	LOOT_MONEY_VEC m_vecLootMoneys;
	LOOT_SOUL_VEC m_vecLootSouls;
	vector<int> m_vecLootBuffs;
	int	m_nID;
	int m_nLimitDropQuantity;		// 동시에 나올수 있는 아이템의 최대 개수(퀘스트와 브레이크파트는 영향받지 않음)
	int m_nTalentID;				// 특정한 탤런트를 사용해 얻을수 있는 경우 세팅
	bool m_bValidation;
public:
	GLootInfo();
	~GLootInfo();

	void Clear();
};

#endif //_GLOOT_INFO_H_
