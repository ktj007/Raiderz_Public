#pragma once

#include "CTransData.h"
#include "MTime.h"

class GEntityPlayer;
class GQuestInfo;

TEST_FORWARD_F(ChallengerQuest, FCQ);

#define CQ_XML_TAG_MAIET			"maiet"
#define CQ_XML_TAG_CQ				"CQ"
#define CQ_XML_TAG_QUEST			"QUEST"
#define CQ_XML_ATTR_ID				"id"
#define CQ_XML_ATTR_MIN_LEVEL		"min_level"
#define CQ_XML_ATTR_MAX_LEVEL		"max_level"
#define CQ_XML_ATTR_MAX_COUNT		"max_count"

class GChallengerQuestMgr
{
	TEST_FRIEND_F(ChallengerQuest, FCQ);
public:
	struct Tier
	{
		int nMinLevel;
		int nMaxLevel;
		int nDailyQuestCount;
		vector<const GQuestInfo*> vecSourceQuests;

		Tier();
	};
public:
	GChallengerQuestMgr(void);
	~GChallengerQuestMgr(void);

	Tier* Get(int nTier);
	size_t GetTierSize();
	void Clear();
	bool Load(const wchar_t* szFileName);
	void Update(float fDelta);

	void ResetDailyQuest();
	void RouteQuestList(GEntityPlayer* pPlayer) const;
	void RouteQuestListRefresh(GEntityPlayer* pPlayer) const;

	void SetRandomSeed(unsigned int nRandomSeed);

	void OnChallengerQuestAccept(GEntityPlayer* pPlayer, const GQuestInfo* pQuestInfo) const;

	void AllQuestFailForTest();

	void OnChallengerQuestComplete(GEntityPlayer* pPlayer);

	bool IsTodayQuest(const GQuestInfo* pInfo) const;
protected:
	virtual tm GetCurrentTime();

private:
	void OnLoadCompleted();
	void InitRandomSeed();
	void Parse_ChallengerQuest( MXmlElement* pElement, MXml* pXml );
	void Parse_Quest( Tier& tier, MXmlElement* pElement, MXml* pXml );
	void CheckQuestEnable(GEntityPlayer* pPlayer, vector<TD_CHALLENGER_QUEST>& outvecTD) const;
	void SaveCurrentTime();
	bool CheckExpiredTime();
	void OnQuestExpired();
	void AllQuestFail();
	void Cooking();
private:
	MRegulator		m_rgrCheckTick;
	unsigned int	m_nRandomSeed;
	vector<Tier>	m_vecSourceTiers;
	vector<const GQuestInfo*>		m_vecDailyQuests;
	tm				m_timeLast;
};
