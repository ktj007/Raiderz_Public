#pragma once

class TestCallCounter
{
public:
	// 함수 호출 카운터 반환
	int GetCallCounter(wstring strFuncName);

protected:
	// 함수 호출 카운터를 증가시킴
	void IncreaseCallCounter(wstring strFuncName);

private:
	typedef map<wstring, int>		MAP_CALLCOUNTER;
	// 테스트용으로 쓰이는 함수 호출 카운터
	MAP_CALLCOUNTER		m_mapTestCallCounter;
};

#define INCREASE_CALLCOUNTER { this->IncreaseCallCounter(__FUNCTIONW__); }