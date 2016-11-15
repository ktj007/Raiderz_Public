#include "stdafx.h"
#include "SUnitTest.h"

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

UnitTest::TestList& GetStage4TestList()
{
	static UnitTest::TestList Stage4TestList;
	return Stage4TestList;
}

UnitTest::TestList& GetStageAccountDBTestList()
{
	static UnitTest::TestList StageAccountDBTestList;
	return StageAccountDBTestList;
}

UnitTest::TestList& GetStageGameDBTestList()
{
	static UnitTest::TestList StageGameDBTestList;
	return StageGameDBTestList;
}

UnitTest::TestList& GetStageLogDBTestList()
{
	static UnitTest::TestList StageLogDBTestList;
	return StageLogDBTestList;
}