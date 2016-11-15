#pragma once

#include "MCommand.h"
#include "MTime.h"

using namespace minet;

class GField;
class GEntityPlayer;
class GEntityActor;
class GEntityNPC;

enum DUEL_STATE
{
	DUEL_QUESTION = 0,
	DUEL_READY,
	DUEL_FIGHTING,
	DUEL_FINISH,
};

#define DUEL_READY_COUNT	3
#define DUEL_READY_TIME		DUEL_READY_COUNT*1.0f
#define DUEL_RADIUS			3000.0f


//
// GDuel
//
class GDuel : public MTestMemPool<GDuel>
{
	enum DUEL_TEAM
	{
		TEAM_NONE=-1,
		TEAM_1=0,
		TEAM_2=1,
	};
public:
	GDuel(MUID uid, GField* pDuelField, GEntityPlayer* pFighter1, GEntityPlayer* pFighter2);
	GDuel( MUID uid, GField* pDuelField, const vector<GEntityPlayer*>& pFighters1, const vector<GEntityPlayer*>& pFighters2 );
	virtual ~GDuel();

	// 해당 팀 구하기
	GDuel::DUEL_TEAM GetTeam( MUID uidPlayer );
	// 틱마다 호출됨
	void Update(float fDelta);
	// 결투가 준비됐을때 호출
	void Ready();
	// 결투가 취소됐을때 호출
	void Cancel();
	// 결투자 중 한명이 죽었을때 호출
	void Killed(MUID uidDeadMan);
	// 결투자 중 한명이 접속을 끊었을때 호출
	void Disconnect( GEntityPlayer* pDisconnector );
	// 해당 액터가 결투자인지 확인하는 함수
	bool IsDuelMember(GEntityActor* pInvoker);
	bool IsDuelMember( MUID uidInvoker );
	// 결투 객체의 UID를 반환
	MUID GetUID()			{ return m_UID; }
	// 결투 상태를 반환
	DUEL_STATE		GetState()			{ return m_nState; }
	// 결투가 종료됐는지 반환
	bool			IsFinished()		{ return m_nState == DUEL_FINISH; }
	// 결투 공간의 중점을 반환
	vec3			GetCenterPos()		{ return m_vCenterPos; }
	// 상대팀 반환
	DUEL_TEAM GetOpponentTeam(DUEL_TEAM nTeam);
	// 상대팀 반환
	vector<MUID>& GetOpponentTeamUIDList(DUEL_TEAM nTeam);
	// 팀 UID 반환
	vector<MUID>& GetTeamUIDList(DUEL_TEAM nTeam);
	// 팀 UID 반환
	vector<MUID>& GetAllFightersUIDList();
	// 결투자들에게 커맨드를 전송	
	void RouteToFighters(MCommand* pNewCmd);
	// 적인지 반환
	bool IsEnemy( MUID uidPlayer, MUID uidOpponent );
	// 플레이어 구하기
	GEntityPlayer* GetPlayer(MUID uidPlayer);
	// 탈락한 플레이어인지 여부
	bool IsLoser(MUID uid);
	// 대표자 구하기
	GEntityPlayer* GetLeader1();
	GEntityPlayer* GetLeader2();

	// 관람 가능 여부
	bool IsObservable() const;
	// 관람 카운터 증가
	void IncreaseObserveCount();
	// 관람 카운터 감소
	void DecreaseObserveCount();
	// 관람 카운터 반환
	int GetObserveCount() const;

	// 파티결투 인지 여부
	bool IsSingleDuel();

protected:
	DUEL_STATE		m_nState;

private:
	// 결투를 시작한다.
	void Duel();
	// 결투를 종료한다.
	void Finished();
	// 준비가 완료된 조건인지 확인
	bool CheckReady();
	// 결투 종료여부 화인
	bool CheckFinish();
	// 결투자중에 도망친 사람이 있는지 확인
	void CheckRunaway();
	bool CheckRunawayImpl(MUID uid);
	// 결투 공간의 중점을 계산
	void CalcCenterPoint();
	// 결투깃발을 생성시킨다.
	void CreateFlag();
	// 결투깃발을 소멸시킨다.
	void DestroyFlag();

	// 준비 커맨드를 전송 (결투자들에게)
	void RouteReady();
	// 결투 커맨드를 전송 (결투자들에게)
	void RouteFight();
	// 완료 커맨드를 전송 (결투자들에게)
	void RouteFinished();
	// 결투취소 커맨드를 전송 (결투자들에게)
	void RouteCancel();
	// 결투 카운터 커맨드를 전송 (결투자들에게)
	void RouteReadyCount();
	// 결투 패배한 사람 알림
	void RouteDefeated(GEntityPlayer* pDefeater);

	// 결투 초기화
	void Cleanup();
	// 해당 플레이어의 디버프 전부 제거
	void RemoveAllDebuff(GEntityPlayer* pPlayer);

	// 플레이어 참가
	void InsertFighter(DUEL_TEAM nTeam, GEntityPlayer* pPlayer);
	// 패배자 추가
	void InsertLoser( MUID uidPlayer );
	
	// 파티장 구하기
	GEntityPlayer* GetLeaderImpl(const vector<MUID>& vecFighters);

private:
	typedef map<MUID, DUEL_TEAM> DuelTeamIndexMap;

	DuelTeamIndexMap	m_mapDuelTeamIndex;
	MUID				m_UID;
	int					m_nReadyCount;
	vector<MUID>	m_vecAllFighters;
	vector<MUID>	m_vecFighters1;
	vector<MUID>	m_vecFighters2;
	set<MUID>		m_setLosers1;
	set<MUID>		m_setLosers2;
	DUEL_TEAM		m_nWinTeam;
	MRegulator		m_rgrReady;
	vec3			m_vCenterPos;
	GField*			m_pField;
	GEntityNPC*		m_pFlag;
	int				m_nObserveCount;
};