#ifndef _M_MAIET_XMLTESTREPORTER_H
#define _M_MAIET_XMLTESTREPORTER_H

#include <vector>
#include "MMaietTestResult.h"
using namespace std;

class MMaietXmlTestReporter
{
private:
	vector<string>		m_vecMessages;
	void AddXmlElement(std::ostream& os, char const* encoding);
	void BeginResults(std::ostream& os, int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed);
	void EndResults(std::ostream& os);
	void BeginTest(std::ostream& os, MMaietTestResult const& result);
	void AddTestResult(std::ostream& os, MMaietTestResult const& result);
	void EndTest(std::ostream& os, MMaietTestResult const& result);
	void AddMessages(std::ostream& os);

	std::ostream& m_ostream;	

	void ReplaceChar(string& str, char const c, string const& replacement);
	string XmlEscape(string const& value);
public:
	MMaietXmlTestReporter(std::ostream& ostream);
	void ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed, MMaietTestResultList const& results);
	void AddMessage(string strMessage);
};

#endif
