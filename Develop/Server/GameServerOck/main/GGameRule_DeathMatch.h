#pragma once

#include "GBattleArena.h"

class GEntityPlayer;

class GGameRule_DeathMatch : public GBattleArena::GameRule, public MTestMemPool<GGameRule_DeathMatch>
{
public:
	GGameRule_DeathMatch();

	void SetMaxLifeCounter(int nMaxLifeCounter);

	// 규칙 초기화
	virtual void Init(const vector<const MARKER_INFO*> vecStaringMarkers, const BattleArena::TEAM_MAP& mapEntries) override;
	// 규칙 업데이트
	virtual void Update(float fDelta) override;

	void Rebirth( GEntityPlayer* pPlayer );
	// 게임이 끝났는가?
	virtual bool IsFinished(const BattleArena::TEAM_MAP& mapEntries, MUID& uidWinner) override;
	// 엔트리 멤버중 한명이 죽었을 경우
	virtual void OnTeamMemberDie(GEntityPlayer* pPlayer, const BattleArena::Team& team) override;
	// 엔트리 멤버중 한명이 스스로 부활되었을 경우
	virtual void OnRebirthTeamMemberBySelf( GEntityPlayer* pPlayer, const BattleArena::Team& team ) override;
	// 엔트리 멤버중 한명이 타인이 부활시켰었을 경우
	virtual void OnRebirthTeamMemberByOther( GEntityPlayer* pPlayer, const BattleArena::Team& team ) override;
	// 엔트리 멤버가 강제 종료한 경우
	virtual void OnDisconnect(GEntityPlayer* pPlayer) override;
	// 엔트리가 제거될때 호출
	virtual void OnDeleteTeam(const BattleArena::Team& team) override;
	// 재접속 처리
	virtual void OnRejoin(GBattleArena::Status nGameStatus, GEntityPlayer* pPlayer) override;

	// 스팟 위치 반환
	virtual vec3 GetSpotPos(GEntityPlayer* pPlayer, const BattleArena::Team& team) override;

	virtual GBattleArena::GameRule* Clone() override;
private:
	// 게임이 종료될 때
	void OnFinished(MUID uidWinner);

	// 부활대기에 넣기
	void AddRebirthTimer( const MUID& uidPlayer, GEntityPlayer* pPlayer );
	// 부활대기에 배기
	void EraseRebirthTimer(const MUID& uidPlayer);
	
	void RouteToTeam( const BattleArena::Team& team, minet::MCommand* pCommand );
	void RouteToAllTeam( minet::MCommand* pCommand );
	void RouteScore();
	void RouteDie(const MUID& uidPlayer);
	void RouteKill(GEntityPlayer* pPlayer);
private:
	typedef std::map<MUID, int>								LIFE_MAP;
	typedef std::map<MUID, const MARKER_INFO*>					STARTINGMARKER_MAP;
	typedef std::map<MUID, pair<MRegulator,GEntityPlayer*>>	REBIRTHTIME_MAP;

	LIFE_MAP				m_mapLifeCounter;
	BattleArena::TEAM_MAP  m_mapTeams;
	int						m_nMaxLifeCounter;
	REBIRTHTIME_MAP			m_mapRebirthTimers;	
	STARTINGMARKER_MAP		m_mapStaringMarkers;
	float					m_fElapsedTime;
};
