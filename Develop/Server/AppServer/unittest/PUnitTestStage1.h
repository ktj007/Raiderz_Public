#pragma once

#include "MBaseUnitTest.h"
#include "PUnitTest.h"
#include "PServerSystems.h"
#include "PServerManagers.h"


class PUnitTestStage1 : public MBaseUnitTest
{
private:
	class ZTestServerSystems : public PServerSystems
	{
	public:
		ZTestServerSystems() : PServerSystems() {}
		virtual ~ZTestServerSystems() {}
		virtual void Create();
		virtual void Destroy();
	};

	ZTestServerSystems		m_TestSystems;
	PServerManagers			m_TestManagers;
public:
	PUnitTestStage1() {}
	virtual ~PUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestFinish();

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTest_Results.xml"; }
};
