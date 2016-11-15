#include "stdafx.h"
#include "MiNetUnitTestRunner.h"
#include "MiNetUnitTestStage1.h"

bool MiNetUnitTestRunner::BeDoTest()
{
	return MUnitTestRunner::BeDoTest();
}

bool MiNetUnitTestRunner::RunStage1()
{
	return Run<MiNetUnitTestStage1>();
}

MiNetUnitTestRunner::MiNetUnitTestRunner() : MUnitTestRunner()
{
	
}

MiNetUnitTestRunner::~MiNetUnitTestRunner()
{

}

