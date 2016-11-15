#include "stdafx.h"
#include "AString.h"
#include "MLocale.h"
#include <mbstring.h>

namespace crl {

const std::string AString::TrimBlanks( const std::string& str )
{
	std::string::size_type begin = 0;
	std::string::size_type end = str.length();

	while (begin != end && IsBlank(str[begin])) ++begin;
	while (end != begin && IsBlank(str[end - 1])) --end;

	return (std::string(str.begin() + begin, str.begin() + end));
}

const bool AString::IsBlank( const std::string::value_type c )
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

const string AString::ToLower( const string& in )
{
	string out = in;

	for (string::size_type i = 0 ; i < in.length() ; ++i)
		out[i] = tolower(in[i]);

	return (out);
}

void AString::Replace( std::string& text, const std::string& find_token, const std::string& replace_token )
{
	size_t i = 0;
	while ((i = text.find(find_token)) != string::npos)
		text.replace(i, find_token.size(), replace_token);
}

int AString::Split(wstring strString, wstring strDelimiters, vector<wstring>& vecSplitStringList)
{
	if (strString.empty())	return 0;

	
	size_t idxBegin = 0;

	for(;;)
	{
		size_t idxSeparator = strString.find_first_of(strDelimiters, idxBegin);

		if (idxSeparator == strString.npos)
		{
			wstring& strSubString = strString.substr(idxBegin, idxSeparator);
			vecSplitStringList.push_back(strSubString);
			break;
		}

		wstring& strSubString = strString.substr(idxBegin, idxSeparator - idxBegin);
		vecSplitStringList.push_back(strSubString);

		idxBegin = idxSeparator + 1;
	}

	return vecSplitStringList.size();
}



string AString::Trim(const string& strInput, const string& strDelim)
{
	return TrimLeft(TrimRight(strInput, strDelim), strDelim);
}

string AString::TrimLeft(const string& strInput, const string& strDelim)
{ 
	return AString::ToString(TrimLeft(AString::ToWstring(strInput), AString::ToWstring(strDelim)));
}

string AString::TrimRight(const string& strInput, const string& strDelim)
{
	return AString::ToString(TrimRight(AString::ToWstring(strInput), AString::ToWstring(strDelim)));
}

wstring AString::Trim(const wstring& strInput, const wstring& strDelim)
{
	return TrimLeft(TrimRight(strInput, strDelim), strDelim);
}

wstring AString::TrimLeft(const wstring& strInput, const wstring& strDelim)
{
	wstring::size_type n = strInput.find_first_not_of(strDelim); 
	return (n == wstring::npos) ? L"" : strInput.substr(n); 
}

wstring AString::TrimRight(const wstring& strInput, const wstring& strDelim)
{ 
	wstring::size_type n = strInput.find_last_not_of(strDelim); 
	return (n == wstring::npos) ? L"" : strInput.substr(0, n+1); 
}

void AString::Split(vector<string>& vecOutputToken, const string& strInput, const string& strDelim)
{
	vector<wstring> vecWstr;
	Split(vecWstr, ToWstring(strInput), ToWstring(strDelim));
	ToString(vecWstr, vecOutputToken);
}

void AString::Split(vector<wstring>& vecOutputToken, const wstring& strInput, const wstring& strDelim)
{
	wstring::size_type nStart	= 0;
	wstring::size_type nEnd		= 0;

	wstring strTrim = TrimRight(TrimLeft(strInput, strDelim), strDelim);

	while (true)
	{
		wstring::size_type nEnd = strTrim.find_first_of(strDelim, nStart);
		if (nEnd == wstring::npos)
		{
			vecOutputToken.push_back(strTrim.substr(nStart));
			break;
		}
		else
		{
			vecOutputToken.push_back(strTrim.substr(nStart, nEnd-nStart));
			nStart = strTrim.find_first_not_of(strDelim, nEnd);
		}		
	}
}

pair<string, string> AString::SplitOnce(const string& strInput, const string& strDelim)
{
	pair<wstring, wstring> ret = SplitOnce(ToWstring(strInput), ToWstring(strDelim));
	return pair<string, string>(ToString(ret.first), ToString(ret.second));
}

pair<wstring, wstring> AString::SplitOnce(const wstring& strInput, const wstring& strDelim)
{
	typedef wstring::size_type idx_t;

	idx_t pl = strInput.find_first_of(strDelim);
	if (pl == wstring::npos)
		return pair<wstring, wstring>(strInput, wstring());
	return pair<wstring, wstring>(strInput.substr(0, pl), strInput.substr(pl + 1));
}

void AString::SplitLength(vector<string>& vecOutputToken, const string& strInput, int nMaxLength)
{
	vector<wstring> vecWstr;
	SplitLength(vecWstr, ToWstring(strInput), nMaxLength);
	ToString(vecWstr, vecOutputToken);
}

void AString::SplitLength(vector<wstring>& vecOutputToken, const wstring& strInput, int nMaxLength)
{
	wstring::size_type i;
	for (i = 0;i < strInput.size();i += nMaxLength)
		vecOutputToken.push_back(strInput.substr(i, nMaxLength));
}

void AString::SplitPathAndFileName( std::string& strPath, std::string& strFileName, const std::string& strInput )
{
	wstring wstrPath, wstrFileName;
	SplitPathAndFileName(wstrPath, wstrFileName, ToWstring(strInput));
	strPath = ToString(wstrPath);
	strFileName = ToString(wstrFileName);
}

void AString::SplitPathAndFileName( std::wstring& strPath, std::wstring& strFileName, const std::wstring& strInput )
{
	wstring::size_type n = strInput.find_last_of(L"\\/");
	if (n == wstring::npos)
	{
		strPath = L"";
		strFileName = strInput;
	}
	else
	{
		strPath = strInput.substr(0, n);
		strFileName = strInput.substr(n+1);
	}
}

string AString::ToUpper(const string& strInput)
{
	return ToString(ToUpper(ToWstring(strInput)));
}

wstring AString::ToUpper(const wstring& strInput)
{
	return MapChars(strInput, towupper);
}

wstring AString::ToLower(const wstring& strInput)
{
	return MapChars(strInput, towlower);
}

string& AString::ReplaceAll(std::string& context, const std::string& from, const std::string& to)
{
	wstring wstrContext;
	ReplaceAll(wstrContext, ToWstring(from), ToWstring(to));
	return context = ToString(wstrContext);
}

wstring& AString::ReplaceAll(std::wstring& context, const std::wstring& from, const std::wstring& to)
{
	wstring::size_type lookHere = 0;
	wstring::size_type foundHere;
	while((foundHere = context.find(from, lookHere)) != wstring::npos)
	{
		context.replace(foundHere, from.size(), to);
		lookHere = foundHere + to.size();
	}
	return context;
}

wstring AString::MapChars(const wstring &strInput, wint_t (*fpMapping)(wint_t))
{
	wstring strRet = strInput;
	for (wstring::size_type i = 0;i < strRet.size();i++)
		strRet[i] = fpMapping(strRet[i]);
	return strRet;
}

string AString::Join(const vector<string>& vecInput, const string& s)
{
	if (vecInput.empty())
		return "";

	string result = vecInput[0];
	for (vector<string>::size_type i = 1;i < vecInput.size();i++)
		result += s + vecInput[i];

	return result;
}

wstring AString::Join(const vector<wstring>& vecInput, const wstring& s)
{
	if (vecInput.empty())
		return L"";

	wstring result = vecInput[0];
	for (vector<wstring>::size_type i = 1;i < vecInput.size();i++)
		result += s + vecInput[i];

	return result;
}

bool AString::EndsWith(const string& strSuper, const string& strWhetherSuffix)
{
	if (strSuper.length() < strWhetherSuffix.length())
		return false;
	return strSuper.substr(strSuper.length() - strWhetherSuffix.length()) == strWhetherSuffix;
}

bool AString::EndsWith(const wstring& strSuper, const wstring& strWhetherSuffix)
{
	if (strSuper.length() < strWhetherSuffix.length())
		return false;
	return strSuper.substr(strSuper.length() - strWhetherSuffix.length()) == strWhetherSuffix;
}

bool AString::StartsWith(const string& strSuper, const string& strWhetherPrefix)
{
	if (strSuper.length() < strWhetherPrefix.length())
		return false;
	return strSuper.substr(0, strWhetherPrefix.length()) == strWhetherPrefix;
}

bool AString::StartsWith(const wstring& strSuper, const wstring& strWhetherPrefix)
{
	if (strSuper.length() < strWhetherPrefix.length())
		return false;
	return strSuper.substr(0, strWhetherPrefix.length()) == strWhetherPrefix;
}


int AString::ToInt(const std::string &strToConvert, int nDefault)
{
	int32 v = nDefault;
	sscanf_s(strToConvert.c_str(), "%d", &v);
	return v;
}

double AString::ToDouble(const std::string &strToConvert, double fDefault)
{
	double v = fDefault;
	sscanf_s(strToConvert.c_str(), "%lf", &v);
	return v;
}

void AString::ToWstring(const vector<string> &vecString, vector<wstring> &vecResult)
{
	vecResult.clear();

	vector<string>::const_iterator it;
	for (it = vecString.begin();it != vecString.end();it++)
		vecResult.push_back(ToWstring(*it));
}

std::wstring AString::ToWstring( const std::string& strString )
{
	return MLocale::ConvAnsiToUTF16(strString.c_str());
}

void AString::ToString(const vector<wstring> &vecString, vector<string> &vecResult)
{
	vecResult.clear();

	vector<wstring>::const_iterator it;
	for (it = vecString.begin();it != vecString.end();it++)
		vecResult.push_back(ToString(*it));
}

std::string AString::ToString( const std::wstring& strString )
{
	return MLocale::ConvUTF16ToAnsi(strString.c_str());
}










} // namespace crl