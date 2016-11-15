#pragma once

#include <string>
#include <map>
#include <vector>
#include "RLib.h"
using namespace std;

class CML2_API MCodeCoverageChecker
{
private:
	struct MCodeCoverageItem
	{
		std::string		strName;
		std::string		strFile;
		std::string		strFunction;
		int				nRunCount;
		MCodeCoverageItem() : nRunCount(0) {}
	};

	typedef std::map<std::string, MCodeCoverageItem*> MCodeCoverageItemMap;
	MCodeCoverageItemMap	m_Items;

	std::vector<string>		m_vecCheckList;
	string					m_strFilter;

	void OutputResult();
	MCodeCoverageItem* FindItem(string strName);
	bool CheckFilter(const char* szName);
public:
	MCodeCoverageChecker(char** szCodeCoverageItemList, int nItemCount, const char* szFilter="") 
	{
		m_strFilter = szFilter;

		for (int i = 0; i < nItemCount; i++)
		{
			string str = szCodeCoverageItemList[i];
			m_vecCheckList.push_back(str);
		}
	}
	~MCodeCoverageChecker()
	{
		OutputResult();
		Clear();
	}
	void Clear();
	void Check(const char* szName, const char* szFunction, const char* szFile);
	void AddItem(const char* szName, const char* szFunction, const char* szFile);
};


extern CML2_API MCodeCoverageChecker*	g_pCodeCoverageChecker;

inline void MInitCodeCoverage(char** szCodeCoverageItemList, int nItemCount, const char* szFilter="")
{
	if (g_pCodeCoverageChecker == NULL)
	{
		g_pCodeCoverageChecker = new MCodeCoverageChecker(szCodeCoverageItemList, nItemCount, szFilter);
	}
}

inline void MFinalCodeCoverage()
{
	if (g_pCodeCoverageChecker)
	{
		delete g_pCodeCoverageChecker;
		g_pCodeCoverageChecker = NULL;
	}
}

inline void MCheckCodeCoverage(const char* szName, const char* szFunction, const char* szFile)
{
	if (g_pCodeCoverageChecker == NULL) return;

	g_pCodeCoverageChecker->Check(szName, szFunction, szFile);
}


#define _DO_USE_CODE_COVERAGE



#ifdef _DO_USE_CODE_COVERAGE

#define CC_Begin(items, filter)		MInitCodeCoverage(items, _countof(items), filter)
#define CC_End()					MFinalCodeCoverage()
#define CC_Check(e)					MCheckCodeCoverage(#e, __FUNCTION__, __FILE__ )

#else

#define CC_Begin(items)		;
#define CC_End()			;
#define CC_Check(e)			;

#endif // _DO_USE_CODE_COVERAGE

/*

 사용법

 -- CCList.inc 를 만들고

 CC_Add(test_case1)
 CC_Add(test_case2)
 CC_Add(test_case3)
 CC_Add(test_case4)

 -- 이런 식으로 리스트 추가


 char* g_CodeCoverageItems[] = {

 #define CC_Add(e) #e,

	#include "CCList.inc"
 };
 #undef CC_Add


CC_Begin(g_CodeCoverageItems);		// 코드 커버리지 체크 시작
CC_End();							// 코드 커버리지 체크 종료

체크할 함수 안에 이런 식으로 CC_Check를 넣으면 됩니다.

void foo()
{
	CC_Check(test1);
	.
	.
	.
}


*/