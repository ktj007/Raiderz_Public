#pragma once

#include "MTest.h"
#include "SUnitTest.h"
#include "GServerSystems.h"
#include "GServerManagers.h"


/// UnitTest Stage4
class GUnitTestStage4 : public MBaseUnitTest
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
	GUnitTestStage4();
	virtual ~GUnitTestStage4();
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();

	virtual const char* GetName() { return UNITTEST_STAGE4_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStage4TestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage4_Results.xml"; }
};
