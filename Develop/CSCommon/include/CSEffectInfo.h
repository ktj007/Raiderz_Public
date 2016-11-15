#pragma once

#include "CSCommonLib.h"

class CSCOMMON_API CSEffectInfo
{
public:
	enum POINT
	{
		POINT_NONE=0,

		POINT_SELF,				// 자기를 중심으로
		POINT_TARGET,			// 타겟을 중심으로
		POINT_CASTER,			// 효과를 적용시킨 사람 중심
		POINT_HITCAPSULE,		// 판정구를 중심으로
		POINT_AREAPOINT,		// 특정 좌표를 중심으로

		POINT_MAX
	};

	enum RELATION
	{
		RELATION_NONE=0,

		RELATION_ALL,			// 모두
		RELATION_ENEMY,			// 적대적인자
		RELATION_ALLIED,		// 우호적인자
		RELATION_PARTY,			// 파티원들
		RELATION_ALLIED_DEAD,	// 우호적이면서 죽어 있는자
		RELATION_SELF,			// 자기자신

		RELATION_MAX
	};

public:
	POINT		m_nPoint;		///< 누구를 중심으로
	float		m_fRadius;		///< 일정 범위만큼
	RELATION	m_nRelation;	///< 자기와 어떤 관계에 있는 대상중
	int			m_nLimit;		///< 몇명 만큼에게 효과를 준다.

public:
	CSEffectInfo();
	CSEffectInfo(POINT nPoint, float fRadius, RELATION nRelation, int nLimit=INT_MAX);
};
