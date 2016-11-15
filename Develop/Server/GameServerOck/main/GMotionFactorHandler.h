#pragma once

#include "CSMotionFactorInfo.h"

class GEntityActor;

/// 실제 모션 팩터 적용 - 위치 수정 등
/// - GMotionFactor에서 GEntityActor를 분리
class GMotionFactorHandler
{
private:
	vec3 MakeKnockbackPos( MF_STATE nMF, GEntityActor* pVictimActor, const GEntityActor* pOpponent, int nWeight, bool bKnockbackDirSyncToOpponentDir );
public:
	GMotionFactorHandler() {}
	~GMotionFactorHandler() {}
	void Handle(GEntityActor* pVictimActor, const GEntityActor* pOpponent, MF_STATE nMF, int nWeight, bool bKnockbackDirSyncToOpponentDir);

	vec3 GetKnockbackDir(MF_STATE nMF, GEntityActor* pVictimActor, const GEntityActor* pOpponent, bool bKnockbackDirSyncToOpponentDir);
};
