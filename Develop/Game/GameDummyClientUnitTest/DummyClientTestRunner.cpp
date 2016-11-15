#include "stdafx.h"
#include "DummyClientTestRunner.h"
#include "DummyClientUnitTestStage1.h"

bool DummyClientTestRunner::BeDoTest()
{
	return MUnitTestRunner::BeDoTest();
}

bool DummyClientTestRunner::RunStage1()
{
	return Run<DummyClientUnitTestStage1>();
}

DummyClientTestRunner::DummyClientTestRunner() : MUnitTestRunner()
{

}

DummyClientTestRunner::~DummyClientTestRunner()
{

}

