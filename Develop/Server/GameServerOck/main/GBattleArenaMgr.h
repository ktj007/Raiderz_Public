#pragma once

#include "GBattleArena.h"
#include "GBattleArenaMatcher.h"

class GEntityPlayer;
typedef std::map<MUID, GBattleArena*>	BATTLEARENA_MAP;

class GBattleArenaMgr : public GBattleArenaMatcher::Lisener, public MTestMemPool<GBattleArenaMgr>
{
public:
	GBattleArenaMgr(void);
	~GBattleArenaMgr(void);

	// 테스트용 규칙 추가
	void CreateTestRules();
	// 틱 처리
	void Update(float fDelta);
	// 참가 신청
	bool RegisterTeam(GEntityPlayer* pPlayer);
	// 참가 취소
	bool DeregisterTeam(GEntityPlayer* pPlayer);
	bool DeregisterTeam( MUID uidTeam );
	// 참가자인지 확인
	bool IsEntryTeam(GEntityPlayer* pPlayer);
	bool IsEntryTeam( MUID uidTeam );
	// 참가 엔트리 수 반환
	size_t GetTeamQty() const;
	// 투기장 수 반환
	size_t GetBattleArenaQty() const;
	// 엔트리 반환
	BattleArena::Team FindTeam(MUID uidTeam) const;
	// 투기장 반환
	GBattleArena* FindBattleArena(MUID uidBattleArena) const;
	// 매처 반환
	inline GBattleArenaMatcher& GetMatcher()		{ _ASSERT(m_pMatcher); return *m_pMatcher; }
	// 매처 반환
	inline GBattleArenaMatcher& GetMatcher() const	{ _ASSERT(m_pMatcher); return *m_pMatcher; }

	// 재접속 처리
	void TryRejoin(GEntityPlayer* pPlayer);

	// 종료규칙 추가
	void AddGameRule(GBattleArena::GameRule* pRule);
	// 종료규칙 초기화
	void ClearGameRules();

	// 엔트리가 제거될때 호출
	void OnDeleteTeam(MUID uidTeam);

	// 투기장 상대자인지 여부
	bool IsBattleArenaOpponent(GEntityPlayer* pPlayer1, GEntityPlayer* pPlayer2) const;

	// 스팟 위치 반환
	vec3 GetSpotPos(GEntityPlayer* pPlayer) const;

protected:
	virtual void CreateDefaultMatcher();
private:
	// 엔트리가 매칭에 성공
	virtual void OnMatched(BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent) override;
	// 투기장 게임이 종료됨
	void OnBattleArenaFinished(MUID uidBattleArena, GBattleArena* pBattleArena);
	// 투기장 할당
	static GBattleArena* NewBattleArena(GBattleArena::GameRule* pRule);
	// 투기장 생성
	void CreateBattleArena(BattleArena::Team& teamRequester, BattleArena::Team& teamOpponent);
	// 투기장 제거
	void DestroyBattleArena(GBattleArena* pBattleArena);
private:
	GBattleArenaMatcher*	m_pMatcher;
	BATTLEARENA_MAP			m_mapBattleArena;
	// 종료 규칙목록
	vector<GBattleArena::GameRule*>			m_vecGameRules;
};
