#pragma once

#include "MBaseUnitTest.h"
#include "SUnitTest.h"
#include "LServerSystems.h"
#include "LServerManagers.h"


class LUnitTestStage1 : public MBaseUnitTest
{
private:
	class ZTestServerSystems : public LServerSystems
	{
	public:
		ZTestServerSystems() : LServerSystems() {}
		virtual ~ZTestServerSystems() {}
		virtual void Create();
		virtual void Destroy();
	};

	ZTestServerSystems		m_TestSystems;
	LServerManagers			m_TestManagers;
public:
	LUnitTestStage1() {}
	virtual ~LUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestFinish();

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTest_Results.xml"; }
};
