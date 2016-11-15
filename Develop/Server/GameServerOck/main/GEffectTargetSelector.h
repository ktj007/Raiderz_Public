#pragma once

#include "GTalentInfo.h"

class GEntityActor;
class GEntitySync;
class GField;
class GRelationChecker;

class GEffectTargetSelector
{
public:
	struct Desc
	{
		const CSEffectInfo& EffectInfo;
		GEntitySync* pSelf;
		GEntitySync* pTarget;
		GEntitySync* pCaster;
		float fAddEffectRadius;
		vec3 vCenterPoint;

		Desc(const CSEffectInfo& _EffectInfo)
			: EffectInfo(_EffectInfo)
		{
			pSelf = NULL;
			pTarget = NULL;
			pCaster = NULL;
			fAddEffectRadius = 0.0f;
			vCenterPoint=vec3::ZERO;
		}
	};
public:
	// 해당 CSEffectInfo에 맞는 대상자들을 찾는다.
	void Select(const Desc& desc,set<GEntityActor*>& result) const;
private:

	// 범위로 목표를 찾아냄
	void GetActors(GField& field, const vec3& vCenterPoint, float fRadius, set<GEntityActor*> &outvecEffectTarget) const;

	// 관계로 목표를 걸러냄
	void FilterRelation(GEntitySync* pSelf, CSEffectInfo::RELATION nRelation, set<GEntityActor*> &outvecEffectTarget) const;

	// 제한인원으로 목표를 걸러냄
	void FilterLimit(vec3 vCenterPoint, int nLimit, set<GEntityActor*> &outvecEffectTarget) const;
	
	// 자신이 파티를 가졌는지 여부
	bool HasParty(GEntityActor* pSelf) const;
};
