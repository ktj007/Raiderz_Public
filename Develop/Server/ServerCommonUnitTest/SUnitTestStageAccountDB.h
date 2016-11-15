#ifndef _SUNITTEST_STAGE_ACCOUNTDB_H
#define _SUNITTEST_STAGE_ACCOUNTDB_H


#include "ServerCommonUnitTestLib.h"
#include "MTest.h"
#include "SUnitTestDBBuilder.h"


/// ACCOUNTDB UnitTest
class SCOMMON_UNITTEST_API  SUnitTestStageAccountDB : public MBaseUnitTest
{
private :
	SUnitTestDBBuilder m_DBBuilder;

public :
	virtual ~SUnitTestStageAccountDB() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestStart();
	virtual void OnTestFinish(UnitTest::TestDetails const& test, int const testTimeInMs);

	virtual const char* GetName() { return UNITTEST_STAGE_ACCDB_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStageAccountDBTestList(); }
	virtual const char* GetOutputFileName() { return "AccountDBUnitTest_Results.xml"; }
};


#endif