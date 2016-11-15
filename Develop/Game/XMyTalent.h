#ifndef _XMY_TALENT_H
#define _XMY_TALENT_H

#include "CSChrInfo.h"
#include "XTalentInfo.h"
#include "XBuffInfo.h"
#include "XModEffector.h"
#include <set>
#include "CSTalentInfo.h"
#include "XTalentInfoMgr.h"
using namespace std;

class XMyPassiveTalent
{
private:
	bool	m_bHasExtraPassiveTalent[TEPT_MAX];		///< 특별한 패시브 탤런트를 습득하고 있는지 여부
	int		m_nExtraPassiveTalentLevel[TEPT_MAX];
	XModEffector			m_ModEffector;

public:
	XMyPassiveTalent();
	void GainEffect(XTalentInfo* pTalentInfo);
	void LostEffect(XTalentInfo* pTalentInfo);
	bool HasExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nType)
	{
		if (true == info.talent->IsGivedExtraPassive(nType))
			return true;

		return m_bHasExtraPassiveTalent[nType];
	}
	bool HasExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nType, int nExtraPassiveLevel)
	{
		if (true == info.talent->IsGivedExtraPassive(nType, nExtraPassiveLevel))
			return true;

		if (m_bHasExtraPassiveTalent[nType] == true && m_nExtraPassiveTalentLevel[nType] >= nExtraPassiveLevel)
			return true;

		return false;
	}
	int GetExtraPassiveLevel(TALENT_EXTRA_PASSIVE_TYPE nType)	
	{ 
		return m_nExtraPassiveTalentLevel[nType]; 
	}
};


// 쿨타임 유닛 상태
enum XCOOLTIME_STATE
{
	XCS_END				= 0,		// 탈랜트 사용 없음
	XCS_REQ_USE_TALENT,				// 탈랜트 사용 요청 중 : C -> S
	XCS_CASTING,					// 탈랜트 캐스팅 중 : C
	XCS_REQ_ACT_TALENT,				// 탈랜트 사용 요청 중 : C -> S
	XCS_USE_TALENT,					// 탈랜트 사용 중 : S -> C
	XCS_TIMEOUT,					// 탈랜트 cool time 시간 종료
};

// 쿨타임 유닛
struct XCOOLTIME_UNIT
{
	XCOOLTIME_STATE		nState;
	float				fCoolTime;
	float				fElapsedTime;

	XCOOLTIME_UNIT() : nState( XCS_END), fCoolTime( 0.0f), fElapsedTime( 0.0f)		{}
};

// 쿨타임 메니져
class XCoolTimeMgr
{
private:
	map< int, XCOOLTIME_UNIT>	m_mapCoolTime;

public:
	XCoolTimeMgr();
	virtual ~XCoolTimeMgr();

	void Update( float fDelta);

	void Clear();

	bool IsCoolDown( int nTalentID);					// 탈랜트를 사용할 수 없는지 여부 확인
	float GetRemainedTime(int nTalentID);				// 남은 시간을 구함
	float GetRemainedTimeRatio(int nTalentID);			// 남은 시간의 비율을 구함

	void RequestUseTalent( int nTalentID);				// 탈랜트 사용 요청 : C -> S
	void CastingTalent( int nTalentID);					// 탈랜트 캐스팅 : C
	void RequestActTalent( int nTalentID);				// 탈랜트 사용 요청 : C -> S
	void UseTalent( int nTalentID);						// 탈랜트 사용 : S -> C
	void Canceled( int nTalentID);						// 탈랜트 취소 : S -> C

	void SetRemained( int nTalentID, float fRemainedTime);		// 이미 존재하는 탈랜트의 남은 cool time 시간 설정 : S -> C

protected:
	void _add_cooltime( int nTalentID, XCOOLTIME_STATE nState, float fCoolTime, float fElapsedTime =0.0f);
	bool IsToggleTalent(int nTalentID);
	bool IsToggleTalent(XTalentInfo* pTalentInfo);

private:
	vector<int> GetRelativeTalentsID(CSTalentInfo* pTalentInfo);
};



typedef set<int> SET_TALENTID;

class XMyTalent
{
private:
	/// 채집 탤런트 학습 여부 관리
	class XGatherTalentRank
	{
		set<int> m_setGatherTalentRank;
	public:
		void Insert(int nRank);
		void Delete(int nRank);

		bool HasGatherTalent(int nRank);
	};
	XGatherTalentRank m_GatherTalentRank;

private:
	XMyPassiveTalent		m_Passive;
	SET_TALENTID			m_setTalentID;			///< 일반적으로 배운 탤런트
	int						m_nRemainTP;			///< 사용할수 있는 TP

	SET_TALENTID			m_setStyleTalentID[9];		///< 
	XModEffector			m_ModEffector;

	SET_TALENTID			m_setSocialTalentID;	///< 소셜탤런트 ID
	SET_TALENTID			m_setBuffTalentID;		///< 버프 걸렸을때 사용 할수 있는 탤런트 ID

public:
	XCoolTimeMgr		CoolTime;
	

	XMyTalent()
	{
		Init();
	}

	void Init()
	{
		m_nRemainTP = 0;

		InitTalent();
	}

	void InitTalent()
	{
		m_setTalentID.clear();

		for (int i=0; i< 7; i++)
		{
			m_setStyleTalentID[i].clear();
		}
	}

	void GainInstantEffect(XTalentInfo* pTalentInfo);
	void GainMaintainEffect(XTalentInfo* pTalentInfo);

	void LostMaintainEffect(XTalentInfo* pTalentInfo);
	void LostMaintainEffect(int nTalentID);

	bool HasExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nType)	{ return m_Passive.HasExtraPassiveTalent(nType); }
	bool HasExtraPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nType, int nExtraPassiveLevel)	{ return m_Passive.HasExtraPassiveTalent(nType, nExtraPassiveLevel); }
	int GetExtraPassiveLevel(TALENT_EXTRA_PASSIVE_TYPE nType)	{ return m_Passive.GetExtraPassiveLevel(nType); }

	bool Train(XTalentInfo* pTalentInfo, bool bConsumeTP=true);
	void Untrain(int nTalentID);
	void UntrainAll();

	void SetSocialTalentID(int nID);

	void GetAllTalentID(vector<int>& outvecTalentID);

	int GetTP() { return m_nRemainTP; }
	void SetTP(int nTP) { m_nRemainTP = nTP; }
	void AddTP(int nTP) { m_nRemainTP += nTP; }

	bool IsLearnedTalent(int nTalentID);
	bool IsLearnedTalent(XTalentInfo* pTalentInfo);
	bool IsLearnedTalentLine(int nTalentLine);
	bool IsLearnedGatherTalent(int nRank);

	void GetHighestRankListOfTalent(vector<XTalentInfo*>& outvecTalentID);
	int GetHighestRankByTalentLine( int nTalentID);

	int GetUsedTP()							{ return m_setTalentID.size();}
	int GetStyleUsedTP(TALENT_STYLE nStyle)
	{
		if (nStyle >= TS_MAX || nStyle <= TS_NONE)
		{
			return 0;
		}

		const SET_TALENTID& setStyleTalentID = m_setStyleTalentID[nStyle];

		int nStyleUsedTP = 0;
		for each (int nID in setStyleTalentID)
		{
			CSTalentInfo* pTalentInfo = info.talent->Find(nID);
			if (NULL == pTalentInfo) continue;
			if (true == pTalentInfo->m_bByLicense) continue;

			nStyleUsedTP++;
		}

		return nStyleUsedTP;
	}

	size_t GetTalentCount() const					{ return m_setTalentID.size();			}
	int GetTalentID( int index);

	int GetEmotionalTalentCount() const;
	int GetEmotionalTalentID( int index);

	void SetBuffTalentID(int arrPalettes[]);
	void ReSetBuffTalentID();
	bool IsUsableBuffTalentID(int nID);
	int GetBuffTalentCount()						{ return m_setBuffTalentID.size(); }
};

#endif // _XMY_TALENT_H