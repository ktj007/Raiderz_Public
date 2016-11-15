#include "stdafx.h"
#include "GServerValidateReporter.h"
#include "MMaietXmlTestReporter.h"
#include "MMaietTestResult.h"
#include "CSStrings.h"
#include "MLocale.h"

// 웹에서 줒어왔습니다 - praptor
inline string trim_right(const string& str) 
{ 
	int n = (int)(str.find_last_not_of(" \t\v\n")); 
	return n == wstring::npos ? str : str.substr(0, n + 1); 
} 

GServerValidateReporter::GServerValidateReporter(std::ostream& ostream)
{
	m_pXmlReporter = new MMaietXmlTestReporter(ostream);
}

GServerValidateReporter::~GServerValidateReporter()
{	
	SAFE_DELETE(m_pXmlReporter);
}

void GServerValidateReporter::OnLog(const char* szLog)
{
	// XML
	string stringLog = MLocale::ConvUTF16ToAnsi(CSStrings::XmlEscape(MLocale::ConvAnsiToUTF16(szLog)).c_str());
	stringLog = trim_right(stringLog);
	stringLog = MLocale::ConvAnsiToUTF8(stringLog.c_str(), LANG_KOREAN);
	m_pXmlReporter->AddMessage(stringLog);

	// Output
	mlog3(szLog);
}

void GServerValidateReporter::Report(void)
{
	m_pXmlReporter->ReportSummary(0, 0, 0, 0, MMaietTestResultList());
}
