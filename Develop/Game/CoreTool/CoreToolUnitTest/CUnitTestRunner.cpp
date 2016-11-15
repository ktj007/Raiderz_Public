#include "stdafx.h"
#include "CUnitTestRunner.h"
#include "CUnitTestStage1.h"
//#include <fstream>
using namespace std;
//using std::ofstream;

bool CUnitTestRunner::BeDoTest()
{
	// publish이면 유닛테스트 안함.
#ifdef _PUBLISH
	return false;
#endif

	// unittest이면 유닛테스트 무조건 함
#ifdef _UNITTEST
	return true;
#endif

	return MUnitTestRunner::BeDoTest();
}

bool CUnitTestRunner::RunStage1()
{
#ifdef _PUBLISH
	return true;
#endif

	return Run<CUnitTestStage1>();
}


CUnitTestRunner::CUnitTestRunner() : MUnitTestRunner()
{
	
}

CUnitTestRunner::~CUnitTestRunner()
{

}

