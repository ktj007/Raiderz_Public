#pragma once

#include "GBattleArenaDefine.h"
#include "MTime.h"

class GEntityPlayer;
struct MARKER_INFO;

namespace minet {
class MCommand;
};

class GBattleArena : public MTestMemPool<GBattleArena>
{
public:
	enum Status
	{
		INVALID = 0,	// 무효한 상태
		PREPARE,		// 참가자들 대기
		COUNTING,		// 대전 대기중
		PLAYING,		// 대전중
		SCOREBOARD,		// 점수판 보여주기
		FINISH,			// 종료
	};

	class GameRule
	{
	public:
		virtual ~GameRule() {}

		virtual GameRule* Clone() = 0;

		// 규칙 초기화
		virtual void Init(const vector<const MARKER_INFO*> vecStaringMarkers, const BattleArena::TEAM_MAP& mapEntries) {}
		// 규칙 업데이트
		virtual void Update(float fDelta) {}
		// 게임이 끝났는가?
		virtual bool IsFinished(const BattleArena::TEAM_MAP& mapEntries, MUID& uidWinner) = 0;
		// 엔트리 멤버중 한명이 죽었을 경우
		virtual void OnTeamMemberDie(GEntityPlayer* pPlayer, const BattleArena::Team& team) {}
		// 엔트리 멤버중 한명이 스스로 부활되었을 경우
		virtual void OnRebirthTeamMemberBySelf( GEntityPlayer* pPlayer, const BattleArena::Team& team ) {}
		// 엔트리 멤버중 한명이 타인이 부활시켰었을 경우
		virtual void OnRebirthTeamMemberByOther( GEntityPlayer* pPlayer, const BattleArena::Team& team ) {}
		// 엔트리 멤버가 강제 종료한 경우
		virtual void OnDisconnect(GEntityPlayer* pPlayer) {}
		// 엔트리가 제거될때 호출
		virtual void OnDeleteTeam(const BattleArena::Team& team) {}
		// 재접속 처리
		virtual void OnRejoin(GBattleArena::Status nGameStatus, GEntityPlayer* pPlayer) = 0;
		// 스팟 위치 얻기
		virtual vec3 GetSpotPos(GEntityPlayer* pPlayer, const BattleArena::Team& team) { return vec3::ZERO; }
	};

	class Lisener
	{
	public:
		virtual void OnBattleArenaShowScoreBoard(const BattleArena::TEAM_MAP& mapEntries)	{}
		virtual void OnBattleArenaComplete(const BattleArena::TEAM_MAP& mapEntries)			{}
	};

	GBattleArena(MUID uid, GameRule* pRule);
	~GBattleArena(void);

	// 리스너 등록
	void RegisterLisener(Lisener* pLisener);

	// UID 반환
	MUID GetUID()							{ return m_UID; }
	// 투기장상태 반환
	Status GetStatus() const;
	// 투기장상태 변경
	void ChangeStatus(Status nStatus);
	// 새로운 참가
	void Join(MUID uidTeam, BattleArena::Team& team);
	// 틱 처리, 반환값은 해당 투기장이 만료됐는지 여부
	bool Update(float fDelta);
	
	// 참여자 인원수 반환
	size_t GetTeamQty() const						{ return m_mapEntries.size(); }
	// 참여자 반환
	const BattleArena::Team* FindTeam(MUID uidTeam) const;

	// 투기장 동적 필드 그룹 UID 반환
	MUID GetFieldGroupUID() { return m_uidFieldGroup; }

	// 엔트리 멤버중 한명이 죽었을 경우
	void OnTeamMemberDie(GEntityPlayer* pPlayer, const BattleArena::Team& team);
	// 엔트리 멤버중 한명이 스스로 부활되었을 경우
	void OnRebirthTeamMemberBySelf( GEntityPlayer* pPlayer, const BattleArena::Team& team );
	// 엔트리 멤버중 한명이 타인이 부활시켰었을 경우
	void OnRebirthTeamMemberByOther( GEntityPlayer* pPlayer, const BattleArena::Team& team );
	// 엔트리 멤버가 강제 종료한 경우
	void OnDisconnect(GEntityPlayer* pPlayer);
	// 엔트리가 제거될때 호출
	void OnDeleteTeam(const BattleArena::Team& team);

	// 게임이 준비중인지 여부
	bool IsGamePreparing() const;
	// 게임이 진행중인지 여부
	bool IsGamePlaying() const;
	// 게임이 종료중인지 여부
	bool IsGameFinishing() const;

	// 재접속 처리
	void OnRejoin(GEntityPlayer* pPlayer);

	// 스팟 위치 반환
	vec3 GetSpotPos(GEntityPlayer* pPlayer, const BattleArena::Team& team);
	
private:
	// 새로운 참가가 있을때
	void OnJoin(const BattleArena::Team& team);
	// 게임을 시작할 준비가 되었는지 여부
	bool IsAllReady();
	// Enter 이벤트
	void OnPrepareEnter();
	void OnCountingEnter();
	void OnPlayingEnter();
	void OnScoreBoardEnter();
	void OnFinishEnter();
	// Leave 이벤트
	void OnPrepareLeave();
	void OnCountingLeave();
	void OnPlayingLeave();
	void OnScoreBoardLeave();
	void OnFinishLeave();
	// Update 이벤트
	void OnPrepareUpdate(float fDelta);
	void OnCountingUpdate(float fDelta);
	void OnPlayingUpdate(float fDelta);
	void OnScoreBoardUpdate(float fDelta);
	void OnFinishUpdate(float fDelta);
	
	// 엔트리에게 패킷 전송
	void RouteToTeam(const BattleArena::Team& team, minet::MCommand* pCommand);
	// 모든 엔트리에게 패킷 전송
	void RouteToAllTeam(minet::MCommand* pCommand);
	// 시작 마커 추출
	void ExtractStartingMarkers();
	// 모든 엔트리 멤버들을 동적필드로 이동
	bool MoveAllTeamToTrialField();
	// 모든 엔트리 멤버들을 시작필드 위치로 이동
	bool MoveAllTeamToEnterPos();

	// 투기장 UID를 플레이어에게 부여
	void SetBattleArenaUIDToTeam( BattleArena::Team &team );
	// 투기장 UID를 플레이어로부터 제거
	void ClearBattleArenaUIDToTeam();

	// 생성된 투기장 동적 필드 UID 저장
	void SetFieldGroupUID(MUID uidCreatedFieldGroup) { m_uidFieldGroup = uidCreatedFieldGroup; }

	// 종료규칙 초기화
	void InitGameRules();
	// 종료규칙 검사
	bool CheckGameRules(MUID& uidWinner);
	// 게임 규칙 업데이트
	void UpdateGameRules( float fDelta );

	// 엔트리 멤버 모두 부활
	void RebirthToAllTeam();
	// 엔트리 멤버 모두 회복
	void FullHealToAllTeam();

	// 종료 조건 처리
	void ProcessFinish();
private:
	MUID		m_UID;
	Status		m_nStatus;
	BattleArena::TEAM_MAP	m_mapEntries;
	vector<Lisener*>		m_vecLiseners;
	// 종료 규칙
	GameRule*				m_pGameRule;
	// 게임 시작 카운터
	MRegulator				m_rgrStartingCounter;
	// 점수판 카운터
	MRegulator				m_rgrScoreBoardCounter;
	// 승자 엔트리
	MUID					m_uidWinner;
	// 시작 마커 모음
	vector<const MARKER_INFO*>	m_vecStaringMarkers;
	// 투기장 동적 필드 그룹 UID
	MUID					m_uidFieldGroup;
};
