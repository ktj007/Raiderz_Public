#include "stdafx.h"
#include "XModulePost.h"

SUITE(ModulePost)
{
	TEST(TestMovementPostChecker_Rotate)
	{
		vec3 vPos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 vDir = vec3(0.0f, 1.0f, 0.0f);

		XMovementPostChecker checker;
		checker.SetLastPostedPosition(vPos);

		XMovementPostChecker::RESULT nResult;
		XMovementPostChecker::CheckArgs args(vPos, vDir, false, false, false, false, false, true);

		// 각도가 바뀌었으므로 PostRotate
		nResult = checker.Check(POST_MOVE_TICK, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_PostRotate, nResult);

		// 각도가 바뀌지 않았다
		nResult = checker.Check(POST_MOVE_TICK, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_NONE, nResult);

		// 각도가 바뀌었다.
		args.Dir = vec3(1.0f, 0.0f, 0.0f);
		nResult = checker.Check(POST_MOVE_TICK, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_PostRotate, nResult);

		// 각도가 바뀌었지만 틱 시간이 아직 안되었다
		args.Dir = vec3(0.0f, 1.0f, 0.0f);
		nResult = checker.Check(0.001f, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_NONE, nResult);
	}

	TEST(TestMovementPostChecker_Move)
	{
		vec3 vPos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 vDir = vec3(0.0f, 1.0f, 0.0f);
		const vec3 vDeltaPos = vec3(0.0f, 50.0f, 0.0f);

		XMovementPostChecker checker;
		checker.SetLastPostedPosition(vPos);

		XMovementPostChecker::RESULT nResult;
		XMovementPostChecker::CheckArgs args(vPos, vDir, true, false, true, false, false, true);

		// 이동 시작
		args.Pos += vDeltaPos;
		nResult = checker.Check(POST_MOVE_TICK, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_PostMove, nResult);

		// 틱이 안지났고 크게 움직이지도 않았다.
		args.Pos += vec3(0.0f, 0.01f, 0.0f);
		nResult = checker.Check(0.01f, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_NONE, nResult);

		// 크게 움직이지 않았지만 틱이 지났다.
		args.Pos += vec3(0.0f, 0.01f, 0.0f);
		nResult = checker.Check(POST_MOVE_TICK, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_PostMove, nResult);

		// 이동을 멈췄음
		args.bCurrentGoing = false;
		args.Pos += vec3(0.0f, 0.01f, 0.0f);
		nResult = checker.Check(POST_MOVE_TICK, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_PostMoveStop, nResult);

	}

	TEST(TestMovementPostChecker_Warp)
	{
		vec3 vPos = vec3(1000.0f, 1000.0f, 0.0f);
		vec3 vDir = vec3(0.0f, 1.0f, 0.0f);
		const vec3 vDeltaPos = vec3(0.0f, 50.0f, 0.0f);

		XMovementPostChecker checker;
		checker.SetLastPostedPosition(vPos);

		XMovementPostChecker::RESULT nResult;
		XMovementPostChecker::CheckArgs args(vPos, vDir, false, false, false, false, false, true);

		// 갑자기 위치가 바뀌었다.
		args.Pos += vec3(0.0f, 1000.0f, 0.0f);
		nResult = checker.Check(0.01f, args);
		CHECK_EQUAL(XMovementPostChecker::RESULT_PostStop, nResult);


	}

}
