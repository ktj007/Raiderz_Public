#pragma once

#include "CSTalentInfo.h"
#include "MTypes.h"
#include "XTalentInfoEnum.h"
#include "XTalentInfoDef.h"
#include "CSTalentInfoHelper.h"
#include "XTalentEffectInfo.h"




class XTalentInfoMgr;

class XTalentInfo : public CSTalentInfo
{
private:
	friend class XTalentInfoMgr;
	using CSTalentInfo::m_szName;		// 문자열테이블 참조하도록 직접 접근 막습니다.
	using CSTalentInfo::m_szDesc;		// 문자열테이블 참조하도록 직접 접근 막습니다.

	// 여기 값들은 xml에서 가져오지 않고, 따로 설정하는 것들이다.
	bool					m_bExistCastingFX;
	bool					m_bExistUseFX;

	XTalentEffectDataMgr	m_EffectDataMgr;

public:
	bool					IsExistcastingFX()		{ return m_bExistCastingFX; }
	bool					IsExistUseFX()			{ return m_bExistUseFX; }
	bool					isNestedSpell()			{ return (m_nSkillType == ST_MAGIC); }

	CSProjectileInfo *		GetProjectileInfo();

	vec3					GetSwordHitDir(float fAniTime, tstring strMeshName);
	vec3					GetSwordHitPos(float fAniTime, tstring strMeshName);

	bool					IsPortalTalent();

	const TCHAR*			GetName(void);
	const TCHAR*			GetDesc(void);

public:

	// Sound --------------------------
	tstring					m_szTargetSound;		// 타겟 사운드(Not Used)
	tstring					m_szTargetLoopingSound;	// 타겟 루프되는 사운드(Not Used)

	// 공격
	MDIRECTION				m_nAttackDirection;				// 공격 방향 - MF_BEATEN에 대응되어 사용된다. (클라)

	bool					m_bFreezeFrame;					// 판정시 프레임 멈춤 여부

	bool					m_bUIIndividual;				//UI에 중첩해서 보여지는 탈렌트이다.

	tstring					m_strGrappledAni;				// 잡힐때 쓰일 플레이어 애니메이션
	tstring					m_strGrappledBone;				// 잘힐때 쓰일 플레이어 본

	bool					m_bShoveWhileMoving;			// 밀기 여부, 충돌이 없으면 밀기 안함 : TRUE 밀기, FALSE 안하기(클라)
	TALENT_SHOVE_TYPE		m_eTalentShoveType;				// 탤런트 이동 타입, 정지, 뚫다, 비빈다

	float					m_fHitBloodScale;

	XTalentInfo() : CSTalentInfo()
	{
		m_bExistCastingFX = false;
		m_bExistUseFX = false;
		
		m_nAttackDirection = MDIR_FRONT;
		m_bFreezeFrame = false;

		m_bUIIndividual			= false;

		m_bShoveWhileMoving = true;
		m_eTalentShoveType = TST_STOP;

		m_fHitBloodScale = 1.0f;
	}

	virtual void CopyAll(CSTalentInfo* pTalentInfo) override;

	XTalentEffectDataMgr* GetEffectDataMgr() { return &m_EffectDataMgr; }	
};


