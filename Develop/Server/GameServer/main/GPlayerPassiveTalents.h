#pragma once

#include "GTalentEffector.h"

class GTalentInfo;

class GPlayerPassiveTalent
{
public:
	// 패시브 효과를 얻음
	void GainEffect( GTalentInfo* pTalentInfo );
	// 패시브 효과를 잃음
	void LostEffect( GTalentInfo* pTalentInfo );
	// 해당 패시브 타입의 효과를 받고 있는지 여부를 반환
	bool HasEffect(TALENT_EXTRA_PASSIVE_TYPE nPassiveType) const;
	// 해당 패시브 타입의 특정 효과레벨 이상의 효과를 받고 있는지 여부를 반환
	bool HasEffect(TALENT_EXTRA_PASSIVE_TYPE nPassiveType, int nExtraPassiveLevel) const;
	// 패시브 효과의 값을 반환
	int  GetPassiveValue(TALENT_EXTRA_PASSIVE_TYPE nPassiveType) const;
	// 소유한 모든 패시브탤런트의 효과를 얻음
	void ApplyEffect( TALENT_CONDITION nCondition, GEntityActor* pThis, GEntityActor* pVictim );

	int GetRank(TALENT_EXTRA_PASSIVE_TYPE nPassiveType);
private:
	// 패시브 효과를 추가
	void AddEffect(TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nValue);
	// 패시브 효과를 제거
	void RemoveEffect(TALENT_EXTRA_PASSIVE_TYPE nEffectType);
	// 해당 탤런트 라인에 맞는 탤런트정보를 반환, 없다면 NULL 반환
	GTalentInfo* GetEffect(int nTalentLine);
	// 해당 패시브 특수값을 얻음, 패시브 효과가 존재하지 않는다면 false를 반환
	bool GetEffectValue(TALENT_EXTRA_PASSIVE_TYPE nEffectType, int& nEffectValue) const;
	// 해당 패시브 특수값을 변경함, 패시브 효과가 존재하지 않는다면 false를 반환
	bool ModifyEffectValue(TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nEffectValue);
	bool IsRemovableEffect(TALENT_EXTRA_PASSIVE_TYPE nEffectType);
	// 무기타입을 패시브 타입으로 변경해 반환
	TALENT_EXTRA_PASSIVE_TYPE  ConvertToPassiveType(WEAPON_TYPE nType);
	void GainEffectImpl( GTalentInfo* pTalentInfo, TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nExtraPassiveParam );
	void LostEffectImpl(GTalentInfo* pTalentInfo, TALENT_EXTRA_PASSIVE_TYPE nEffectType, int nExtraPassiveParam);
private:
	typedef map<TALENT_EXTRA_PASSIVE_TYPE, int> MAP_EXTRA_PASSIVE_TYPE;	// 
	typedef map<int, GTalentInfo*> MAP_TALENTS;		
	// 패시브 특수값 컨테이너, second=CustomValue
	MAP_EXTRA_PASSIVE_TYPE			m_mapPassiveValues;
	// 패시브 탤런트 컨테이너, // first=TalentLine
	MAP_TALENTS			m_mapPassiveTalents;
	// 패시브 효과를 적용하기 위한 이펙터
	GTalentEffector		m_TalentEffector;
};

