#pragma once

#include "MTest.h"
#include "SUnitTest.h"
#include "GServerSystems.h"
#include "GServerManagers.h"


/// UnitTest Stage3
class GUnitTestStage3 : public MBaseUnitTest
{
private:
	GServerSystems			m_TestSystems;
	GServerManagers			m_TestManagers;
public:
	GUnitTestStage3();
	virtual ~GUnitTestStage3();
	virtual void Create(MUnitTestConfig& config);
	virtual void Destroy();

	virtual const char* GetName() { return UNITTEST_STAGE3_NAME; }
	virtual UnitTest::TestList const& GetTestList() { return GetStage3TestList(); }
	virtual const char* GetOutputFileName() { return "UnitTestStage3_Results.xml"; }
};
