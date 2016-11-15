#pragma once

#include "GActorObserver.h"

class GActorHateTracker : private GActorObserver, public MTestMemPool<GActorHateTracker>
{
public:
	GActorHateTracker(GEntityActor* pOwner);
	~GActorHateTracker(void);

	// 초기화
	void Clear();

	// 엔트리테이블이 비어있는지 여부
	bool IsEmpty() const;

	// 엔트리 갯수
	size_t GetQty() const;

	// 엔트리테이블 반환
	void GetHateNPCs(vector<MUID>& vecNPCs) const;
	
	// HP 회복시 증가되는 헤이트 처리
	void AddHate_RecoverHP(int nRecoveredHP, float fHateFactor);
	// 탤런트 사용시 증가되는 헤이트 처리
	void AddHate_UseTalent(int nHateAdj);

private:
	// 상대가 자신을 헤이트테이블에서 추가하였을때
	virtual void OnAddHateTarget( GEntityActor* pEnemy ) override;
	// 상대가 자신을 헤이트테이블에서 제거하였을때
	virtual void OnLostHateTarget( MUID uidEnemy ) override;
	// 자신이 죽었을때
	virtual void OnDie() override;
	// 자신이 필드를 변경할때
	virtual void OnChangeField() override;
	// 자신의 객체가 소멸될때
	virtual void OnDestroy() override;
	// 자신이 같은필드에서 순간이동 했을때 (Player 전용)
	virtual void OnWarpSameField() override;
	
private:
	GEntityActor*	m_pOwner;
	set<MUID>		m_setHateNPCs;	
};
