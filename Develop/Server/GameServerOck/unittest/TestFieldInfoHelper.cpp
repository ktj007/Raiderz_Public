#include "stdafx.h"
#include "CSFieldInfoHelper.h"

SUITE(FieldInfoHelper)
{
	TEST(FieldInfoHelper_ConvertToGridPos)
	{
		// #define FIELD_ATTR_CELL_SIZE	300 // cm

		CHECK(MPoint(0, 0) == CSFieldInfoHelper::ConvertToGridPos(0.0f, 0.0f, 0.0f, 0.0f));
		CHECK(MPoint(1, 1) == CSFieldInfoHelper::ConvertToGridPos(301.0f, 599.0f, 0.0f, 0.0f));

		CHECK(MPoint(0, 0) == CSFieldInfoHelper::ConvertToGridPos(-10000.0f, -10000.0f, -10000.0f, -10000.0f));
		CHECK(MPoint(1, 1) == CSFieldInfoHelper::ConvertToGridPos(100.0f, 100.0f, -300.0f, -300.0f));
	}
}