#pragma once

#include "GTalentInfo.h"

class GEntityActor;

// 역할: ExtraActive 탤런트에만 쓰이는 효과를 구현해놓은 클래스
//			ExtraActive는 1)자기자신에게 쓰여지며 2)탤런트를 사용하는 시점에 불리우며 3)일반적인 탤런트 프로퍼티로 정의할 수 없는 경우의 액티브 탤런트를 뜻한다.
class GExtraActiveTalentRunner
{
public:
	// 엑스트라 액티브 탤런트를 적용 시킴, 탤런트 라인 적용을 먼저 시도하고 해당이 안되면 특정 엑스트라 탤런트 적용
	void Apply(GEntityActor* pUser, vec3 vPos, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
private:
	// 저격모드 (PC전용)
	void onTogglePrecision( GEntityActor* pTargetActor );
	// 은신모드 (PC전용)
	void onToggleSneak( GEntityActor* pTargetActor, int nSneakBuffType );
	// 강신모드 (PC전용)
	void onApplyAdvent( GEntityActor* pTargetActor );
	// 스폰 (PC전용)
	void onSpawn(GEntityActor* pUser, int nNPCID, int nDespawnTime);
	// 부활
	void onRebirth(GEntityActor* pUser, GEntityActor* pTarget, int nRecoverRate);
	// 포탈
	void onPortal(GEntityActor* pTarget, int nFieldID, int nMarkerID);
	// 포탈 여관
	void onPortal_SoulBinding(GEntityActor* pTarget);
	// 도발
	void onTaunt(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 버프 엔티티 생성
	void onBuffEntity(GEntityActor* pUser, vec3 vPos, int nIncludeBuffID, int nLimitQty);
	// 가장최근의 마법류 디버프(100번대) 해제
	void onDispel(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 가장최근의 독 디버프(200번대) 해제
	void onCurePoison(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 가장최근의 질병류 디버프(300번대) 해제
	void onCureDisease(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 가장최근의 저주류 디버프(400번대) 해제
	void onCureCurse(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 가장최근의 이동불가형 디버프(500번대) 해제
	void onDispelMesmerize(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 가장최근의 부활 페널티 디버프(600번대) 해제
	void onDispelDeathPenalty(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);

	// 대상자의 이로운 버프 제거
	void onDispelBuff(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
	// 대상자의 모든 포커스 제거, NPC용
	void onDispelFocus(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);	
	// 대상자의 무기변환 버프 제거
	void onDispelBPart(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pVictim);
};	
