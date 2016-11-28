#ifndef _G_PLAYER_TALENT_H_
#define _G_PLAYER_TALENT_H_

#include "GTalentInfo.h"

typedef set<int> SET_TALENTID;
class GEntityPlayer;
class GTalentInfo;

struct GTalentEnumInfo;


/// 플레이어가 배운 탤런트들
class GPlayerTalent : public MTestMemPool<GPlayerTalent>
{
private:
	SET_TALENTID	m_setTalentID;			///< 학습된 탤런트
	GEntityPlayer*	m_pOwner;

	int				m_nRemainTP;			///< 남은 TP

	MUID			m_nUsedTalentItemUID;	///< 사용한 탤런트 아이템의 UID

	SET_TALENTID	m_setInternalTalentID;	// This set stores available talents for the player, but only for internal purpose.

public:
	GPlayerTalent(GEntityPlayer* pOwner);
	virtual ~GPlayerTalent() {};

	void Insert(int nTalentID, bool bConsumeTP=true);	
	void Insert(GTalentInfo* pTalentInfo, bool bConsumeTP=true);
	void DeleteAll();	

	void InsertWithLowRankTalent(int nTalentID, bool bUseTP=true);	

	void AddTP(int nTP) { m_nRemainTP += nTP; }
	void SetTP(int nTP) { m_nRemainTP = nTP; }
	int GetTP() { return m_nRemainTP; }
	int GetTotalTP();

	SET_TALENTID& GetContainer() { return m_setTalentID; }
	int GetTrainedStyleTP(TALENT_STYLE nStyle);

	bool IsTrainedTalent(GTalentInfo* pTalentInfo);
	bool IsTrainedTalent(int nTalentID);
	bool IsTrainedGatherTalent(int nRank);
	bool IsTrainedTalentLine(int nTalentLine);

	bool IsEmpty();

	void EnableUseItemTalent(MUID nUsedTalentItemUID);
	bool IsEnableUseItemTalent(int nTalentID);
	void DisableUseItemTalent();
	MUID GetUsedTalentItemUID();

	bool IsTPConsumeTalent(int nTalentID);

	TALENT_STYLE GetMainTalentStyle();

	bool IsStyleTalentLearnable(TALENT_STYLE nLearnStyle);
	void CheckAvailableMasteryTalent(TALENT_STYLE nStyle, vector<int>& outvecTalentID);

	SET_TALENTID GetTopRankTalentID();

private:
	typedef map<int, GTalentInfo*> MAP_TOPRANK_TALENT;
	MAP_TOPRANK_TALENT m_mapTopRankTalent;

	void EnumTalents(GTalentEnumInfo& out, bool bIncludeMasteryTalent=true);

	// TALENT_STYLE GetTalentStyle(GTalentEnumInfo& talentEnumInfo);
	bool IsTrainableTalentStyle(TALENT_STYLE nStyle);
	bool HasHybridPassive(GTalentEnumInfo& talentEnumInfo);

private:
	/// 채집 탤런트 학습 여부 관리
	class GGatherTalentRank
	{
		set<int> m_setGatherTalentRank;
	public:
		void Insert(int nRank);
		void Delete(int nRank);

		bool HasGatherTalent(int nRank);
	};
	GGatherTalentRank m_GatherTalentRank;

	void Delete(int nTalentID, bool bRefreshInternalTalent = true);
	void RefreshInternalTalentID();
};

#endif
