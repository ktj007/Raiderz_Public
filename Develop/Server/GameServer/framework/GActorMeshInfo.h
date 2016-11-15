#pragma once

#include "CSMeshInfo.h"
#include "GFrameworkLib.h"

/// 액터 엔티티의 판정 정보
class GFRAMEWORK_API GActorMeshInfo : public MTestMemPool<GActorMeshInfo>
{
private:
	CSMeshInfo		m_MeshInfo;		///< Actor의 Scale까지 적용된 값이다.
	bool			m_bSimple;			/// 판정 캡슐이 딱 하나인 판정 정보
	MCapsule		m_capSimple;
	float			m_fMaxRadius;			///< 최대 반지름 - 원점에서 몸통의 제일 앞까지의 거리(늑대처럼 앞이 더 긴 경우를 위함)
	float			m_fMinRadius;			///< 최소 반지름 - 원점에서 몸통이 차지하는 제일 작은 거리(늑대처럼 앞이 더 긴 경우를 위함)
	void SetSimpleCapsule(MCapsule cap);
	void SetupRadius();
public:
	GActorMeshInfo();
	void Init(CSMeshInfo* pMeshInfo, float fScale);
	void SetDefault(float fScale=1.0f);
	void SetForPlayer();
	void SetRadius(float fMinRadius, float fMaxRadius);

	bool IsSimple() { return m_bSimple; }
	MCapsule GetSimpleCapsule() { return m_capSimple; }
	size_t GetHitGroupCount();
	CSHitGroup& GetHitGroup(int nGroupIndex=0);
	void ReserveHitGroupForTest(int nGroupCount=0);
	float GetColRadius() const { return m_fMaxRadius; }
	float GetHitRadius() const { return m_fMinRadius; }
	float GetAnimationTime(wstring strAnimationName);
	bool HasHitGroup(int nGroupIndex);
};

