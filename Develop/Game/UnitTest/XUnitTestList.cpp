#include "stdafx.h"
#include "XUnitTestList.h"

UnitTest::TestList& GetStage2TestList()
{
	static UnitTest::TestList Stage2TestList;
	return Stage2TestList;
}

UnitTest::TestList& GetStage3TestList()
{
	static UnitTest::TestList Stage3TestList;
	return Stage3TestList;
}
