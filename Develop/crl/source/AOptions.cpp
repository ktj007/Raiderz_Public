#include "stdafx.h"
#include "AOptions.h"

AOptions::AOptions()
: m_switch(L"-")
, m_separator(L" ")
, m_splitter(L" ")
{
	// do nothing
}

AOptions::~AOptions()
{
	for each(AOptionSetBase* optionSet in m_listOptions)
	{
		delete optionSet;
	}
}

bool AOptions::Parse(wstring commandLine)
{
	if (!ValidCommandLine(commandLine))
	{
		return false;
	}

	return ParseOptions(commandLine);
}

bool AOptions::SetSwitch(wstring newSwitch)
{
	// EmtpySwitch 허용 - commandLine 전체를 하나의 옵션으로 취급하게된다.
	m_switch = MakeEscape(newSwitch);
	return true;
}

bool AOptions::SetSeparator(wstring newSeparator)
{
	if (newSeparator.empty())	return false;

	m_separator = MakeEscape(newSeparator);

	return true;
}

bool AOptions::SetSplitter(wstring newSplitter)
{
	if (newSplitter.empty())	return false;

	m_splitter = MakeEscape(newSplitter);

	return true;
}

bool AOptions::ValidCommandLine(wstring commandLine)
{
	wstringstream maker;
	maker	<< "("
				<< "(^| +)"					// 각 옵션은 공백으로 구분
				<< MakeMatchOption()	
			<< ")*"
			<< " *";						// l-trim

	tr1::wregex commandLinePattern(maker.str());

	tr1::match_results<wstring::const_iterator> commandResult;
	if (!tr1::regex_match(commandLine, commandResult, commandLinePattern))
	{
		return false;
	}

	return true;
}

bool AOptions::ParseOptions(wstring commandLine)
{
	tr1::wregex optionPattern(MakeMatchOption());

	tr1::wsregex_token_iterator begin(commandLine.begin(), commandLine.end(), optionPattern), end;
	for(tr1::wsregex_token_iterator it = begin; it != end; it++)
	{
		wstring token = *it;

		tr1::match_results<wstring::const_iterator> optionResult;
		if (!tr1::regex_match(token, optionResult, optionPattern))
		{
			return false;
		}

		wstring option = optionResult[1].str();
		wstring arguments = optionResult[2].str();

		for each(AOptionSetBase* optionSet in m_listOptions)
		{
			if (!optionSet->Match(option))	continue;				
			if (!optionSet->SetArgument(arguments, m_splitter))	return false;				
		}
	}

	return true;
}

wstring AOptions::MakeEscape(wstring text)
{
	wstring escapeText = text;

	wstring::size_type pos = 0;
	for(;;)
	{			
		// default regex ordinary character (ECMAScript)
		pos = escapeText.find_first_of(L"^$\\.*+?()[]{}|", pos);	

		if (pos == wstring::npos)
		{
			break;
		}

		escapeText.insert(pos, L"\\");
		pos += 2;
	}

	return escapeText;
}

wstring AOptions::MakeNotMatch(wstring text)
{	
	if (text.empty())
	{
		return L"";
	}

	// look-around 응용
	return L"(?!" + text + L")";
}

wstring AOptions::MakeMatchOption(void)
{
	wstringstream maker;		
	maker	<< m_switch									// 옵션 스위치
			<< "("
				<< "(?:"
					<< MakeNotMatch(m_switch)
					<< MakeNotMatch(m_separator)
				<< ".)+"
			<< ")"
			<< "(?:"									// 옵션값 시작
				<< m_separator							
				<< " *"
				<< "("									// 첫 옵션값
					<< "(?:"
						<< MakeNotMatch(m_switch)
						<< MakeNotMatch(m_splitter)
					<< ".)+"
					<< "(?:"							// 나머지 옵션값들
					<< m_splitter
						<< "(?:"
							<< MakeNotMatch(m_switch)
							<< MakeNotMatch(m_splitter)
						<< ".)+"
					<< ")*"
				<< ")"
			<< ")?";

	return maker.str();
}
