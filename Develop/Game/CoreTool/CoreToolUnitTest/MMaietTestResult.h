#ifndef _M_MAIET_TEST_RESULT_H
#define _M_MAIET_TEST_RESULT_H

#include <string>
#include <vector>

class MMaietTestResult
{
public:
	MMaietTestResult();
	MMaietTestResult(char const* test);

	std::string testName;

	typedef std::pair< int, std::string > Result;
	typedef std::vector< Result > SuccessVec;
	typedef std::vector< Result > FailureVec;

	SuccessVec successs;
	FailureVec failures;

	float timeElapsed;
};

typedef std::vector<MMaietTestResult> MMaietTestResultList;

#endif//_CS_MAIET_TEST_RESULT_H
