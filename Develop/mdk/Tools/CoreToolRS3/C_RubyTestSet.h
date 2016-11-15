#pragma once

#include <vector>
#include <string>
#include <map>

namespace rs3
{
	class RSingletonSceneManager;
};
class CControlMediator;
class CRubyTestSet
{
public:
	inline static CRubyTestSet& GetReference()
	{
		static CRubyTestSet engineTestSet;
		return engineTestSet;
	}

	// Test Set Logger
	static void CustomMLogCallBack(const char* szText, MLogLevel nLevel);
	// Test Log Getter
	const char* GetLastTestLog(){ return m_strLog_.c_str(); }

	// Zone Engine Test Set
	bool TestZoneEngine(int _nTestType, int _nTestIterationNum, const char* _pSzTestParameter);
private:
	CRubyTestSet(void);
	~CRubyTestSet(void);

	// custom log setter
	// 로거는 mlog를 이용합니다.
	void InitTestSetLogger_(const char* _pSzTestName);
	void HaltTestSetLogger_();
	std::string m_strLog_;
	const char* m_pSzTestTitleHeader_;
};
