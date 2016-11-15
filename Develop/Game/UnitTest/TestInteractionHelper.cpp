#include "stdafx.h"
#include "XInteractionHelper.h"

SUITE(InteractionHelper)
{
	TEST(TestInteractionHelper_GetInteractionSize)
	{
		RBoundingBox rbb;
		rbb.vmin = vec3::ZERO;
		rbb.vmax = vec3(100.0f, 200.0f, 20.0f);

		// 모서리 200이 제일 기니까 반지름은 100
		CHECK_CLOSE(100.0f, XInteractionHelper::GetInteractionSize(rbb), 0.001f);
	}

	TEST(TestInteractionHelper_GetInteractionSize_Min)
	{
		RBoundingBox rbb;

		float fmin = ( MINSIZE_INTERACTION_CIRCLE * 2.0f ) - 10.0f;

		rbb.vmin = vec3::ZERO;
		rbb.vmax = vec3(fmin, fmin, fmin);

		CHECK_EQUAL(MINSIZE_INTERACTION_CIRCLE, XInteractionHelper::GetInteractionSize(rbb));
	}
	TEST(TestInteractionHelper_GetInteractionSize_Max)
	{
		RBoundingBox rbb;

		float fmax = (MAXSIZE_INTERACTION_CIRCLE * 2.0f ) + 10.0f;

		rbb.vmin = vec3::ZERO;
		rbb.vmax = vec3(fmax, fmax, fmax);

		CHECK_EQUAL(MAXSIZE_INTERACTION_CIRCLE, XInteractionHelper::GetInteractionSize(rbb));
	}

}