#pragma once

#include "GBattleArena.h"

class GEntityActor;
class GEntityPlayer;

class GPlayerBattleArena : public MTestMemPool<GPlayerBattleArena>
{
public:
	GPlayerBattleArena(GEntityPlayer* pOwner);

	// 투기장에 들어가 있는가?
	bool IsEntered() const;
	// 투기장 경기를 하고 있는가?
	bool IsNowPlaying() const;
	// 투기장 점수판을 보고 있는가?
	bool IsNowDisplaySocreboard() const;

	// 참가 신청
	bool RegisterEntry();
	// 참가 취소
	bool DeregisterEntry();

	// 투기장 UID 관리
	void SetBattleArenaUID(MUID uid)	{ m_uidBattleArena = uid; }
	MUID GetBattleArenaUID() const		{ return m_uidBattleArena; }

	// 투기장 상태 반환
	GBattleArena::Status GetStatus() const;

	// Owner가 죽었을때 호출
	void OnDie();

	// Owner가 스스로 부활되었을때 호출
	void OnRebirthBySelf();
	// Owner가 타인에의해 부활되었을때 호출
	void OnRebirthByOther();
	// onwer가 강제 종료한 경우
	void OnDisconnect();

private:
	GEntityPlayer*	m_pOwner;
	MUID			m_uidBattleArena;
};
