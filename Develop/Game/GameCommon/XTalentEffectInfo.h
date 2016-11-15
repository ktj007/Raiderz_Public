#pragma once

#include "CSTalentInfo.h"
#include "XTalentInfoEnum.h"

class CSTalentInfoMgr;

struct EFFECT_DEL
{
	MUID		uidEffect;
	bool		bImmediatlyDelete;

	EFFECT_DEL() : uidEffect(MUID::ZERO), bImmediatlyDelete(false) {}
	EFFECT_DEL(MUID uid, bool bImme) : uidEffect(uid), bImmediatlyDelete(bImme) {}
};

struct XTalentHitEffectPosDir
{
public:
	tstring				strMeshName;
	vec3				vHitEffectPos;				// 피격 이펙트 위치(검사용으로 쓰입니다.)
	vec3				vSwordTrailHitUp;			// 검에 의한 피격시 피격 이펙트 방향
	float				fHitTime;

	XTalentHitEffectPosDir()
	{
		vHitEffectPos = vec3::ZERO;
		vSwordTrailHitUp = vec3::AXISZ;
		fHitTime = 0.0f;
	}
};

class XTalentEffectInfo
{
public:
	TALENT_EFFECT_TYPE					m_nEffectType;

	tstring								m_strTalentEffectInfoName;				// 데이터의 이름(툴에 사용하기 위한 이름)

	tstring								m_strEffectName;
	tstring								m_strBoneName;
	tstring								m_strParameter;

	bool								m_bScale;								// 스케일 적용 하는가?

	IMPACTPOS							m_nImpactPos;
	TALENT_EFFECT_DIRECTION				m_nEffectDirection;

	vector<XTalentHitEffectPosDir>		m_vecTalentHitEffectPosDir;

	XTalentEffectInfo()
	{
		Init();
	}
	virtual ~XTalentEffectInfo() {}

	void Init()
	{
		m_nEffectType					= CAST_EFFECT;

		m_bScale						= false;

		m_nImpactPos					= MODEL_POS;
		m_nEffectDirection				= TED_FRONT_EFFECT_USER;

		m_strEffectName.clear();
		m_strBoneName.clear();
		m_strParameter.clear();
		m_vecTalentHitEffectPosDir.clear();
	}

	void copyData(XTalentEffectInfo& talentEffectInfo)
	{
		m_strTalentEffectInfoName	= talentEffectInfo.m_strTalentEffectInfoName;
		m_strEffectName				= talentEffectInfo.m_strEffectName;
		m_nEffectType				= talentEffectInfo.m_nEffectType;
		m_strBoneName				= talentEffectInfo.m_strBoneName;
		m_strParameter				= talentEffectInfo.m_strParameter;
		m_nImpactPos				= talentEffectInfo.m_nImpactPos;
		m_nEffectDirection			= talentEffectInfo.m_nEffectDirection;
		m_bScale					= talentEffectInfo.m_bScale;
		m_vecTalentHitEffectPosDir	= talentEffectInfo.m_vecTalentHitEffectPosDir;
	}
};

class XTalentEffectBase
{
public:
	TALENT_EFFECT_BASE_TYPE				m_nEffectBaseTYpe;

	tstring								m_strEffectBaseFileName;
	tstring								m_strBoneName;

	bool								m_bScale;								// 스케일 적용 하는가?

	IMPACTPOS							m_nImpactPos;
	TALENT_EFFECT_DIRECTION				m_nEffectDirection;


	XTalentEffectBase()
	{
		Init(TEBY_MELEE_ATTACK);
	}

	XTalentEffectBase(TALENT_EFFECT_BASE_TYPE nType)
	{
		Init(nType);
	}

	void Init(TALENT_EFFECT_BASE_TYPE nType)
	{
		m_nEffectBaseTYpe				= nType;

		m_strEffectBaseFileName.clear();
		m_strBoneName.clear();

		m_bScale						= false;

		m_nImpactPos					= MODEL_POS;
		m_nEffectDirection				= TED_FRONT_EFFECT_USER;
	}
};

class XTalentEffectDataMgr : public vector<XTalentEffectInfo *> 
{
public:
	int									m_nRefID;
	int									m_nRefMode;

	XTalentEffectDataMgr() : m_nRefID(0), m_nRefMode(0)
	{

	}

	virtual ~XTalentEffectDataMgr()
	{
		Clear();
	}

	int									Get(TALENT_EFFECT_TYPE nType, vector<XTalentEffectInfo *> &TalentEffectInfo);
	XTalentEffectInfo *					Get(TALENT_EFFECT_TYPE nType);						// 타입들 중에 오직 첫번째 하나만 가져온다. (주의바람)
	XTalentEffectInfo*					Get(tstring& strEffectInfoName);
	bool IsEmpty()						{ return empty(); }
	void Clear()
	{
		vector<XTalentEffectInfo *>::iterator it = begin();
		while(it != end())
		{
			SAFE_DELETE(*it);
			it++;
		}

		clear();
	}
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 전역함수
float CalParabola(float fFullLength, float fCurrLength, float fMaxHeight);
vec3 MoveParabola(vec3 vStartPos, vec3 vTargetPos, vec3 vCurPos, vec3 vVelocity, float fTickTime, float fMaxHeight);
