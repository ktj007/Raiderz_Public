#pragma once

#include "MTest.h"
#include "GServerSystems.h"
#include "GServerManagers.h"
#include "MockServer.h"
#include "SUnitTest.h"

class GUnitTestStage2 : public MBaseUnitTest
{
private:
	class GTestServerSystems2 : public GServerSystems
	{
	public:
		GTestServerSystems2() : GServerSystems() {}
		virtual ~GTestServerSystems2() {}
		virtual void Create();
		virtual void Destroy();
	};

	GTestServerSystems2		m_TestSystems;
	GServerManagers			m_TestManagers;
public:
	GUnitTestStage2() {}
	virtual ~GUnitTestStage2() {}
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();

	virtual const char* GetName() { return UNITTEST_STAGE2_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStage2TestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage2_Results.xml"; }
};
