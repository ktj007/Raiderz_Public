#pragma once


#define UNITTEST_CONFIG_FILTER_ITEM_COUNT	4

/// 서버의 설정값
class MUnitTestConfig
{
private:
	bool GetPrivateProfileBool(const char* szAppName, const char* szKeyName, bool bDefault, const char* szFileName);
	void GetFilter( const char* szName, string (&outString)[UNITTEST_CONFIG_FILTER_ITEM_COUNT], const char* filename );

	bool IsExistFile(const char *filename);
public:
	MUnitTestConfig();

	bool Load();

	bool m_bExecuteUnitTest;										///< 실행 파일 실행하기 전에 유닛 테스트를 수행할지 여부
	string m_strStageFilter;										///< 유닛 테스트할 스테이지 (전체이면 all)
	string m_strSuite[UNITTEST_CONFIG_FILTER_ITEM_COUNT];			///< 테스트할 Suite, 아무것도 없으면 모두 테스트한다.
	string m_strDisabledSuite[UNITTEST_CONFIG_FILTER_ITEM_COUNT];	///< 테스트하지 않을 Suite
	string m_strTest[UNITTEST_CONFIG_FILTER_ITEM_COUNT];			///< 특정 테스트만 테스트할 때 테스트 이름
	string m_strSuiteAnyWords;										///< Suite 이름중에 이 단어가 있으면 테스트한다.
	string m_strCodeCoverageFilter;				///< 코드 커버리지 체크 필터
	bool m_bBreakOnFailure;						///< 테스트 실패시
	int m_nRepeat;								///< 반복 회수. 
	bool m_bLogSuiteName;						///< 테스트 실행시 Suite 이름을 로그로 출력할지 여부
	bool m_bCheckCodeCoverage;					///< 코드 커버리지 체크를 할 지 여부
	bool m_bLogTimeElapsed;						///< 테스트별 소요시간을 출력할지 여부

	// dbtest
	string	m_strDBServer;
	string	m_strDBName;
	string	m_strDBUserName;
	string	m_strDBPassword;
	bool	m_bExecuteDB;
	string	m_strOdbcDriver;
	
	bool CheckStageFilter(const char* szStageName);
};





