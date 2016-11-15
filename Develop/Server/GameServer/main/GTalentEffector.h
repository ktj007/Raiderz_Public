#pragma once

#include "GEffectTargetSelector.h"
#include "GExtraActiveTalentRunner.h"
#include "CSTalentInfo.h"
#include "GModEffector.h"
#include "GActorDamageCalculator.h"

class GEntityActor;
class GTalentInfo;

// 역활: 마법관련 탤런트의 효과를 액터에게 적용시킴
class GTalentEffector
{
public:
	struct Info
	{
		GTalentInfo* pTalentInfo;
		TALENT_CONDITION nTalentCondition;
		GField* pField;
		GEntityActor* pUser;
		GEntityActor* pTarget;
		vec3		vGroundPoint;
		bool bIgnoreMF;

		Info()
			: pTalentInfo(NULL), nTalentCondition(TC_NONE), pField(NULL), pUser(NULL), pTarget(NULL), bIgnoreMF(false), vGroundPoint(vec3::ZERO)
		{}
	};
public:
	// 효과를 얻는 타켓 목록을 만들어 탤런트 효과 모두(버프 포함)를 적용
	void ApplyTalent(Info& info);
	// Target에게 탤런트의 단일 효과를 줌
	bool GainTalentEffect(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pTarget, bool bIgnoreMF, bool bTalentInfoModified=false);
	// Target에게 탤런트의 단일 효과를 없앰
	void LostTalentEffect(GEntityActor* pUser, GTalentInfo* pTalentInfo, GEntityActor* pTarget);
	// Target에게 탤런트 피해 효과를 줌
	bool GainDamageAndHeal(GEntityActor* pUser, GEntityActor* pTarget, GTalentInfo* pTalentInfo, GDamageRangedInfo infoDamage, GHealRangedInfo infoHeal, bool bIgnoreMF, bool bNeedDamageRecalc=false, float fDamageApplyRate=1.0f);
protected:
	// Victim을 중심으로 타겟타입에 해당되는 모든 액터에게 버프 획득을 시도
	void GainSingleBuff( const Info& info, GTalentInfo* pTalentInfo, const CSBuffEnchantInfo& BuffInvokeInfo );
	// 피해를 입히는 탤런트인지 여부
	bool IsNeedEffect(GTalentInfo* pTalentInfo);	
	// 버프의 관련된 효과를 처리
	void GainTalentBuff(const Info& info, GTalentInfo* pTalentInfo);
	// 탤런트의 인스턴트 모디파이어 효과를 얻음 패킷 발송
	void RouteGainInstantEffect(GEntityActor* pUser, GTalentInfo* pTalentInfo);	

private:
	// 탤런트로 회복됨
	void OnHealed( GEntityActor* pTarget, GTalentInfo* pTalentInfo, int nHealHPAmount );
	// 탤런트에 즉시 효과가 있는지 여부
	bool IsInstantEffect( GTalentInfo* pTalentInfo );
protected:
	// 백스텝 탤런트의 경우 백히트면 연계 탤런트 정보로 바꾼다.
	GTalentInfo* ModifyBackHitTalentInfo(GTalentInfo* pTalentInfo, GEntityActor* pTarget);
private:
	GEffectTargetSelector		m_EffectTargetSelector;
	GExtraActiveTalentRunner	m_ExtraActiveTalentRunner;
	GModEffector				m_ModEffector;
	GActorDamageCalculator		m_DamageCalculator;
};	
