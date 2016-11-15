#include "stdafx.h"
#include "XTalentHitParam.h"
#include "XModuleCollision.h"

SUITE(HitEffectSelecter)
{
	class FModuleCollision : public XModuleCollision
	{
	public:
		FModuleCollision()
		{			
		}
		~FModuleCollision()
		{
		}

		//float TestColCapsuleToLineForNotRub(const vec3& vStart, const vec3& vEnd, const vec3& vNearestTargetPos, float fMaxLen, vec3& vOutNearest)
		//{
		//	// 비비지 않게 하는 함수 테스트
		//	//return  ColCapsuleToLineForNotRub(vStart, vEnd, vNearestTargetPos, fMaxLen, vOutNearest);
		//}
	};

	TEST_FIXTURE(FModuleCollision, Test_ColCapsuleToLineForNotRub)
	{
		vec3 vStart(1000.0f, 1000.0f, 40.0f);
		vec3 vEnd(1000.0f, 1060.0f, 40.0f);
		vec3 vNearestTargetPos(1000.0f, 980.0f, 40.0f);
		vec3 vNearest;

		//float fLen = TestColCapsuleToLineForNotRub(vStart, vEnd, vNearestTargetPos, 80.0f, vNearest);
	}

}