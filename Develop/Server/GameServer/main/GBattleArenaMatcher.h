#pragma once

#include "GBattleArena.h"
#include "GBattleArenaDefine.h"

class GEntityPlayer;

class GBattleArenaMatcher : public GBattleArena::Lisener, public MTestMemPool<GBattleArenaMatcher>
{
public:
	GBattleArenaMatcher(void);
	~GBattleArenaMatcher(void);

	class Rule
	{
	public:
		virtual ~Rule() {}

		virtual bool IsMatch(const BattleArena::Team& teamRequester, const BattleArena::Team& teamOpponent) = 0;
	};

	class Lisener
	{
	public:
		// 엔트리가 매칭에 성공
		virtual void OnMatched(BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent)	{}
	};

	// 매칭규칙 추가
	void AddRule(Rule* pRule);
	// 매칭규칙 초기화
	void ClearRules();
	// 리스너 등록
	void SetLisener(Lisener* pLisener)		{ m_pLisener = pLisener; }

	// 참가 신청
	bool RegisterTeam(GEntityPlayer* pPlayer);
	
	// 참가 취소
	bool DeregisterTeam(GEntityPlayer* pPlayer);
	// 참가 취소
	bool DeregisterTeam(MUID uidTeam);
	
	// 참가자인지 확인
	bool IsEntryTeam(GEntityPlayer* pPlayer);
	// 참가자인지 확인
	bool IsEntryTeam(MUID uidTeam);
	// 참가 엔트리 수
	size_t GetTeamQty() const				{ return m_mapTeams.size(); }
	// 엔트리 반환
	BattleArena::Team Lookup(MUID uidTeam) const;
	// 투기장에서 적인지
	bool IsEnemy(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2);

protected:
	// 엔트리가 등록되었을 때
	virtual void OnRegisterTeam(BattleArena::Team& team);
	
	// 엔트리가 등록취소 되었을 때
	virtual void OnDeregisterTeam(BattleArena::Team& team);
	
	// 엔트리가 매칭을 시도할 때
	virtual void OnTryMatch(BattleArena::Team& team);
	// 엔트리가 매칭에 성공
	virtual void OnMatched(BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent);
private:
	bool CheckRules(const BattleArena::Team& teamRequester, const BattleArena::Team& teamOpponent);
	void RouteToTeam( const BattleArena::Team& team, minet::MCommand* pCommand );

	virtual void OnBattleArenaComplete(const BattleArena::TEAM_MAP& mapTeams);

	bool AddTeam( GEntityPlayer* pPlayer );
	void DeleteTeam( const BattleArena::Team &team );

	void RouteRegister( BattleArena::Team& team );
	void RouteDeregister( BattleArena::Team& team );
protected:
	// 투기장 참가자
	BattleArena::TEAM_MAP	m_mapTeams;
	// 규칙목록
	vector<Rule*>			m_vecRules;
	// 리스너
	Lisener*				m_pLisener;
};
