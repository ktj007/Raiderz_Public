#pragma once

#include "UnitTest++.h"
#include "MUnitTestConfig.h"

class MBaseUnitTest
{
private:

public:
	MBaseUnitTest() {}
	virtual ~MBaseUnitTest() {}
	virtual void Create(MUnitTestConfig& config) = 0;		///< 유닛테스트에 필요한 자원 초기화
	virtual void Destroy() = 0;		///< 유닛테스트에 필요한 자원 해제
	virtual void OnTestStart() {}		///< 각각의 테스트 항목이 시작하기 전에 호출됨
	virtual void OnTestFinish() {}		///< 각각의 테스트 항목이 끝났을 때 호출됨

	virtual const char* GetName() = 0;				///< 유닛테스트의 이름
	virtual UnitTest::TestList const& GetTestList() = 0;		///< 테스트할 리스트
	virtual const char* GetOutputFileName() = 0;	///< 출력 xml 파일 이름
};


