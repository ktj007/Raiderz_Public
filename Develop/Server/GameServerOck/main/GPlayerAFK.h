#pragma once

#include "GActorObserver.h"

class GEntityPlayer;

class GPlayerAFK : private GActorObserver, public MTestMemPool<GPlayerAFK>
{
public:
	GPlayerAFK(GEntityPlayer* pOwner);
	virtual ~GPlayerAFK(void);

	// 틱 갱신
	void Update(float fDelta);

	// 자리비움 상태 반환
	bool IsNowAFK() const;

	// 자리비움 상태 토글
	void Toggle();

	// 자리비움 될때까지 기다린 시간 반환
	float GetTimeoutSecond() const;

private:
	// 자리비움 시작
	void OnAfkBegin();
	// 자리비움 종료
	void OnAfkEnd();

	// 테스트용: 클라에게 알려주는 나레이션
	void TestNarration(const char* pszText);
private:
	// 이동, 공격등 적극적인 행동을 했을 때 (Player 전용)
	virtual void OnDoSomething() override;
	// 자신이 죽었을때
	virtual void OnDie() override;
private:
	GEntityPlayer*	m_pOwner;
	MRegulator		m_rgrTimeout;
	bool			m_bNowAFK;
};
