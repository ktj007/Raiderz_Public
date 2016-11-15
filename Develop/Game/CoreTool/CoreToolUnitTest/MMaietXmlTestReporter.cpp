#include "stdafx.h"
#include "MMaietXmlTestReporter.h"

MMaietXmlTestReporter::MMaietXmlTestReporter(std::ostream& ostream)
: m_ostream(ostream)
{
}

void MMaietXmlTestReporter::ReportSummary(int const totalTestCount, int const failedTestCount,
									int const failureCount, float const secondsElapsed, MMaietTestResultList const& results)
{
	AddXmlElement(m_ostream, NULL);

	BeginResults(m_ostream, totalTestCount, failedTestCount, failureCount, secondsElapsed);

	AddMessages(m_ostream);

	for (MMaietTestResultList::const_iterator i = results.begin(); i != results.end(); ++i)
	{
		BeginTest(m_ostream, *i);

		AddTestResult(m_ostream, *i);

		EndTest(m_ostream, *i);
	}

	EndResults(m_ostream);
}

void MMaietXmlTestReporter::AddXmlElement(ostream& os, char const* encoding)
{
	os << "<?xml version=\"1.0\"";

	if (encoding != NULL)
		os << " encoding=\"" << encoding << "\"";

	os << "?>\n";
}

void MMaietXmlTestReporter::BeginResults(std::ostream& os, int const totalTestCount, int const failedTestCount, 
								   int const failureCount, float const secondsElapsed)
{
	os << "<maiettest-results"
		<< " tests=\"" << totalTestCount << "\"" 
		<< " failedtests=\"" << failedTestCount << "\"" 
		<< " failures=\"" << failureCount << "\"" 
		<< " time=\"" << secondsElapsed << "\""
		<< ">\n";
}

void MMaietXmlTestReporter::EndResults(std::ostream& os)
{
	os << "</maiettest-results>\n";
}

void MMaietXmlTestReporter::BeginTest(std::ostream& os, MMaietTestResult const& result)
{
	os << "<maiettest"
		<< " name=\"" << result.testName << "\""
		<< " time=\"" << result.timeElapsed << "\"";
}

void MMaietXmlTestReporter::EndTest(std::ostream& os, MMaietTestResult const& result)
{
	if (true == result.failures.empty() && true == result.successs.empty())
		os << "/>\n";
	else
		os << "</maiettest>\n";		
}

void MMaietXmlTestReporter::AddTestResult(std::ostream& os, MMaietTestResult const& result)
{
	if (!result.failures.empty() || !result.successs.empty())
	{
		os << ">\n"; // close <test> element
	}

	for (MMaietTestResult::SuccessVec::const_iterator it = result.successs.begin(); 
		it != result.successs.end(); 
		++it)
	{
		string const message = XmlEscape(it->second);
		os << "<success" << " message=\"" << message << "\"" << "/>\n";
	}

	for (MMaietTestResult::FailureVec::const_iterator it = result.failures.begin(); 
		it != result.failures.end(); 
		++it)
	{
		string const message = XmlEscape(it->second);
		os << "<failure" << " message=\"" << message << "\"" << "/>\n";
	}	
}

void MMaietXmlTestReporter::AddMessage( string strMessage )
{
	m_vecMessages.push_back(strMessage);
}

void MMaietXmlTestReporter::AddMessages( std::ostream& os )
{
	for (size_t i = 0; i < m_vecMessages.size(); i++)
	{
		string strMsg = m_vecMessages[i];

		os << "\t<maietreport"
			<< " text=\"" << strMsg << "\""
			<< "/>\n";
	}
}

void MMaietXmlTestReporter::ReplaceChar( string& str, char const c, string const& replacement )
{
	for (size_t pos = str.find(c); pos != string::npos; pos = str.find(c, pos + 1))
		str.replace(pos, 1, replacement);
}

string MMaietXmlTestReporter::XmlEscape( string const& value )
{
	string escaped = value;

	ReplaceChar(escaped, '&', "&amp;");
	ReplaceChar(escaped, '<', "&lt;");
	ReplaceChar(escaped, '>', "&gt;");
	ReplaceChar(escaped, '\'', "&apos;");
	ReplaceChar(escaped, '\"', "&quot;");

	return escaped;
}