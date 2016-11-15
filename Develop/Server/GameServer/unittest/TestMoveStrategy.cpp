#include "stdafx.h"
#include "GMoveStrategy.h"

SUITE(MoveStrategy)
{
	class TestDefiniteMoveStrategy : public GDefiniteMoveStrategy
	{
	public:
		// MoveStrategy
		using GMoveStrategy::m_pOwner;
		using GMoveStrategy::m_vGoalPos;
		using GMoveStrategy::m_vGoalDir;
		using GMoveStrategy::m_fLeastGap;
		using GMoveStrategy::m_vOldPos;

		// GDefiniteMoveStrategy
		using GDefiniteMoveStrategy::m_nMoveIndex;
		using GDefiniteMoveStrategy::m_vWaypoint;
		using GDefiniteMoveStrategy::m_vNextDir;
		using GDefiniteMoveStrategy::m_vecWaypoints;

		TestDefiniteMoveStrategy(GEntityNPC* pOwner, float fLeastGap) 
			: GDefiniteMoveStrategy(pOwner, fLeastGap)
		{

		}
		virtual ~TestDefiniteMoveStrategy() {}
	};
	class FMoveStrategy
	{
	public:
		FMoveStrategy()
		{

		}
		~FMoveStrategy()
		{

		}
	};

	//////////////////////////////////////////////////////////////////////////

	TEST_FIXTURE(FMoveStrategy, TestDefiniteMoveStrategy_Constructor)
	{
		const float fLeastGap = 50.0f;
		TestDefiniteMoveStrategy defeniteMoveStrategy(NULL, fLeastGap);

		CHECK_EQUAL(-vec3::AXISY, defeniteMoveStrategy.m_vGoalDir);
		CHECK_EQUAL(vec3::ZERO, defeniteMoveStrategy.m_vGoalPos);
		CHECK_EQUAL(vec3::ZERO, defeniteMoveStrategy.m_vOldPos);
		CHECK_EQUAL(fLeastGap, defeniteMoveStrategy.m_fLeastGap);

		CHECK_EQUAL(-vec3::AXISY, defeniteMoveStrategy.m_vNextDir);
		CHECK_EQUAL(0, defeniteMoveStrategy.m_nMoveIndex);
		CHECK_EQUAL(vec3::ZERO, defeniteMoveStrategy.m_vWaypoint);
	}
}