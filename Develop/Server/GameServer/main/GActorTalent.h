#pragma once

#include "GActorObserver.h"

class GActorTalent : private GActorObserver, public MTestMemPool<GActorTalent>
{
public:
	GActorTalent(GEntityActor* pOwner);
	~GActorTalent(void);

	bool HasUseTalentPos() const;
	vec3 GetUseTalentPos() const;

private:
	// 탤런트 사용이 시작될때 호출되는 이벤트
	virtual void OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo) override;
private:
	GEntityActor*	m_pOwner;
	// 탤런트를 사용할 때의 위치
	vec3			m_vUseTalentPos;
};
