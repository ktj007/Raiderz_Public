#ifndef _SUNITTEST_STAE_LOGDB_H
#define _SUNITTEST_STAE_LOGDB_H


#include "ServerCommonUnitTestLib.h"
#include "MTest.h"
#include "SUnitTestDBBuilder.h"


/// ACCOUNTDB UnitTest
class SCOMMON_UNITTEST_API  SUnitTestStageLogDB : public MBaseUnitTest
{
private :
	SUnitTestDBBuilder m_DBBuilder;

public :
	virtual ~SUnitTestStageLogDB () {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestStart();
	virtual void OnTestFinish(UnitTest::TestDetails const& test, int const testTimeInMs);

	virtual const char* GetName() { return UNITTEST_STAGE_LOGDB_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStageLogDBTestList(); }
	virtual const char* GetOutputFileName() { return "LogDBUnitTest_Results.xml"; }
};


#endif