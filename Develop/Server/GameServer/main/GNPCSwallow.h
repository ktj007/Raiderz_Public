#pragma once

#include "GActorObserver.h"

class GEntityNPC;
class GEntityActor;
class GTalentInfo;
class GNPCInfo;

class GNPCSwallow : public GActorObserver, public MTestMemPool<GNPCSwallow>
{
public:
	GNPCSwallow(GEntityNPC* pOwner);
	~GNPCSwallow(void);

	// 삼킬 수 있는지 여부
	bool IsSwallowable();
	// 삼키고 있는 상태인지 여부
	bool IsSwallowing();
	// 삼키기
	bool Swallow( GEntityActor* pTarget, GTalentInfo* pTalentInfo );
	// 뱉기
	bool Spewup(bool bInterrupt);

private:
	// 자신이 죽었을때 호출되는 이벤트
	virtual void OnDie() override;
	// 개별적인 버프 얻기 구현함수
	void GainBuffImpl(GEntityActor* pTarget, GTalentInfo* pTalentInfo, int nBuffID );
	// 먹기 관련 버프 얻기
	void GainBuff(GEntityActor* pTarget, GNPCInfo* pNPCInfo, GTalentInfo* pTalentInfo );
	// 먹기 관련 버프 제거
	void LostBuff(GEntityActor* pTargetActor);
	// 뱉기 패킷 전송
	void RouteSpewup(bool bInterrupt);
private:
	GEntityNPC* m_pOwner;
	MUID m_uidTarget;
};
