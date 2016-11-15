#pragma once

#include "MBaseUnitTest.h"
#include "ZUnitTest.h"
#include "ZServerSystems.h"
#include "ZServerManagers.h"


class ZUnitTestStage1 : public MBaseUnitTest
{
private:
	class ZTestServerSystems : public ZServerSystems
	{
	public:
		ZTestServerSystems() : ZServerSystems() {}
		virtual ~ZTestServerSystems() {}
		virtual void Create();
		virtual void Destroy();
	};

	ZTestServerSystems		m_TestSystems;
	ZServerManagers			m_TestManagers;
public:
	ZUnitTestStage1() {}
	virtual ~ZUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestFinish();

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTest_Results.xml"; }
};
