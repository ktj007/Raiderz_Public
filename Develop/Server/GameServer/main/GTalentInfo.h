#ifndef _GTALENT_INFO_H
#define _GTALENT_INFO_H

#include "CSTalentInfo.h"
#include "CSTalentInfoHelper.h"
#include "GDef.h"
#include "GTalentDef.h"

using namespace cscommon_server;

class GTalentInfoMgr;
class GEntityActor;

/// 탤런트 정보
class GTalentInfo : public CSTalentInfo, public MTestMemPool<GTalentInfo>
{
private:
	friend class GTalentInfoMgr;
	// 여기 값들은 xml에서 가져오지 않고, 따로 설정하는 것들이다.
	bool					m_bExistCastingFX;
	bool					m_bExistUseFX;
	bool					m_bExistModValue;
	bool					m_bExistProjectileEvent;
	bool					m_bIsProjectileType;
	bool					m_bDelayedAct;
	float					m_fDistanceToLongestPoint;
	float					m_fExpiredTime; 
	float					m_fLeastProjectileDistance;
	vec3					m_vLowestProjectileStartPos;
	MBox					m_bxHit;
	bool					m_bHasExtraActiveEvent;
	bool					m_bHasAttack; // 공격적인 요소가 있는지
	
private:
	void					InitModValues();
	void					InitDistanceToLongestPoint();
	void					InitExpiredTime();
	void					InitActExtendTime();
	void					InitEvent();
	void					InitLeastProjectileDistance();
	void					InitHitAABB();
	void					InitCheckHeal();
	void					InitHasAttack();

public:
	
	// 특수하게 설정된 탤런트의 식별명 (외치기 숙련같은 효과에 쓰임)
	wstring					m_strSpecialization;
	// 히트시 버블이 생길지 여부
	bool					m_bBubbleHit;					
	// 비전투에만 사용가능한지 여부
	bool					m_bNonCombatOnly;
	// 해당 탤런트를 적 뒤에서 사용했을때 효과를 대채할 탤런트
	int						m_nBackHitTalent;
	// 목표 액터와 겹쳤을때 때릴 수 있는지 여부
	bool					m_bHitOverlap;
	// 개발자만 사용할 수 있는지 여부
	bool					m_bUseOnlyGM;		
	// 힐 탤런트인지 여부
	bool					m_bHeal;
	// 회피 탤런트 인지 여부
	bool					m_bIsDodge;

	// Sound --------------------------
	wstring					m_szTargetSound;		// 타겟 사운드(Not Used)
	wstring					m_szTargetLoopingSound;	// 타겟 루프되는 사운드(Not Used)

	// AI -----------------------------
	float					m_fMinAITime;			// NPC가 가드를 성공했을때 사용되는 최소 AITime
	float					m_fTalentStartRange;	// NPC 공격 시작 거리 (이 값이 0.0f이면 이 값은 무시되고 캡슐 거리로 판단합니다.)

	GTalentInfo();
	virtual ~GTalentInfo();
	virtual void 			Cooking();
	virtual void			CopyAll(CSTalentInfo* pTarTalentInfo) override;
		
	bool					IsExistcastingFX()		{ return m_bExistCastingFX; }
	bool					IsExistUseFX()			{ return m_bExistUseFX; }
	bool					IsExistModValue()		{ return m_bExistModValue; }
	bool					isAttackable()			{ return !m_HitInfo.m_vSegments.empty(); }
	bool					IsMoving();
	bool					IsKnockbackDirSyncToAttackerDir() { return IsMoving(); }
	bool					IsAggressive()			{ return (	m_EffectInfo.m_nRelation == CSEffectInfo::RELATION_ENEMY); }
	bool					IsRequireMoveSpeed()	{ return m_fRequireMoveSpeed >= 0.0f; }
	bool					IsExtraActive()			{ return (m_nExtraActive != TEAT_NONE); }
	bool					IsGuard()				{ return m_nSkillType == ST_GUARD; }
	bool					IsProjectileType()		{ return m_bIsProjectileType; }
	bool					IsDelayedAct()			{ return m_bDelayedAct; }
	bool					HasBuff()				{ return (m_Buff1.nID != INVALID_BUFF_ID || m_Buff2.nID != INVALID_BUFF_ID); }
	bool					HasEvent()				{ return !m_Events.m_vecEvents.empty(); }
	bool					HasActEvent()			{ return !m_Events.m_vecActEvents.empty(); }
	bool					HasExtraActiveEvent()	{ return m_bHasExtraActiveEvent; }
	bool					HasCastingTime()		{ return m_fCastingTime > 0.0f; }
	bool					HasProjectileEvent()	{ return m_bExistProjectileEvent; }
	bool					HasHeal() const; // 해당 탤런트에 포함된 모든 기능(버프 등)중에 힐이 있는지 여부
	
	vec3					GetLowestProjectileStartPos()	{ return m_vLowestProjectileStartPos; }
	float					GetDistanceToLongestPoint() { return m_fDistanceToLongestPoint; }
	float					GetExpiredTime()			{ return m_fExpiredTime; }
	float					GetLeastProjectileDistance() { return m_fLeastProjectileDistance; }
	const MBox&				GetHitAABB()				{ return m_bxHit; }

	int						MakeDamageFromMinMaxDamage();
	bool					IsUseAvoidTime()			{ return m_bUseAvoidTime; }
	bool					IsUseInvincibleTime()		{ return m_bUseInvincibleTime; }
	bool					IsUseSuperarmorTime()		{ return m_bUseSuperarmorTime; }

	bool					IsEffective(GEntityActor* pReqActor, GEntityActor* pTarActor);
	bool					IsMovingWithCasting();

	void					RemoveExpiredTime()		{ m_fExpiredTime = 0.0f; }
	
	bool					HasAttack()				{ return m_bHasAttack; }

	bool					IsNPCTalent();	
	bool					IsPortalTalent();
	void					InitDodge();
};



#define TALENT_PROPERTY_SHOUT	L"shout"
#define TALENT_PROPERTY_SAZAHU	L"sazahu"



#endif
