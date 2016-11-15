#ifndef _SUNITTEST_STAGE_GAMEDB_H
#define _SUNITTEST_STAGE_GAMEDB_H


#include "ServerCommonUnitTestLib.h"
#include "MTest.h"
#include "SUnitTest.h"
#include "SUnitTestDBBuilder.h"


/// GAMEDB UnitTest
class SCOMMON_UNITTEST_API  SUnitTestStageGameDB : public MBaseUnitTest
{
private :
	SUnitTestDBBuilder m_DBBuilder;

public:
	SUnitTestStageGameDB();
	virtual ~SUnitTestStageGameDB() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestStart();
	virtual void OnTestFinish(UnitTest::TestDetails const& test, int const testTimeInMs);

	virtual const char* GetName() { return UNITTEST_STAGE_GAMEDB_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStageGameDBTestList(); }
	virtual const char* GetOutputFileName() { return "GameDBUnitTest_Results.xml"; }
};


#endif