#include "stdafx.h"
#include "AOptionSet.h"

AOptionSetBase::AOptionSetBase(wstring option, wstring description)
: m_option(option)
, m_description(description)
{
	// do nothing
}

AOptionSetBase::~AOptionSetBase()
{
	// do nothing
}

bool AOptionSetBase::Match(wstring option)			
{ 
	return m_option == option;
}

bool AOptionSetBase::Split(wstring arguments, wstring splitter)
{
	wstringstream maker;
	maker	<< " ?"									// l-trim
			<< "("
				<< "(?:(?!" << splitter << ").)+"	// an argument, 분리자 포함안함.
			<< ")"
			<< " *"									// r-trim
			<< "(?:" << splitter << ")?";

	tr1::wregex argumentPattern(maker.str());

	tr1::wsregex_token_iterator begin(arguments.begin(), arguments.end(), argumentPattern, 1), end;
	for(tr1::wsregex_token_iterator it = begin; it != end; it++)
	{
		if (!SetSplitArgument(*it))	return false;			
	}

	return true;		
}
