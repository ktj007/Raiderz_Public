#include "stdafx.h"
#include "GNpcXPTable.h"

SUITE(NPCXPTable)
{
	TEST(TestNPCXPTable_GetFactorGrade)
	{
		GNpcXPTable table;

		CHECK_EQUAL(0.0f, table.GetFactorGrade(0));
		CHECK_EQUAL(0.3f, table.GetFactorGrade(1));
		CHECK_EQUAL(0.6f, table.GetFactorGrade(2));
		CHECK_EQUAL(1.0f, table.GetFactorGrade(3));
		CHECK_EQUAL(5.0f, table.GetFactorGrade(4));
		CHECK_EQUAL(10.0f, table.GetFactorGrade(5));
		CHECK_EQUAL(15.0f, table.GetFactorGrade(6));
		CHECK_EQUAL(20.0f, table.GetFactorGrade(7));
		CHECK_EQUAL(25.0f, table.GetFactorGrade(8));
		CHECK_EQUAL(35.0f, table.GetFactorGrade(9));
		CHECK_EQUAL(45.0f, table.GetFactorGrade(10));
	}
}