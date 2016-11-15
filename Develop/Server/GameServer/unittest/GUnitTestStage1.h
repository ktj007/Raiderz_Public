#pragma once

#include "SUnitTest.h"
#include "GServerSystems.h"
#include "GServerManagers.h"
#include "MockServer.h"
#include "MTest.h"

class GUnitTestStage1 : public MBaseUnitTest
{
private:
	class GTestServerSystems : public GServerSystems
	{
	public:
		GTestServerSystems() : GServerSystems() {}
		virtual ~GTestServerSystems() {}
		virtual void Create();
		virtual void Destroy();
	};

	GTestServerSystems		m_TestSystems;
	GServerManagers			m_TestManagers;
public:
	GUnitTestStage1() {}
	virtual ~GUnitTestStage1() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();
	virtual void OnTestFinish();

	virtual const char* GetName() { return UNITTEST_STAGE1_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return UnitTest::Test::GetTestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage1_Results.xml"; }
};
