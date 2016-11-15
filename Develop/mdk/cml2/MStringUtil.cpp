#include "stdafx.h"
#include "MStringUtil.h"

namespace cml2 {

const std::string ToLower(const std::string& in)
{
	std::string out = in;

	for (std::string::size_type i = 0 ; i < in.length() ; ++i)
		out[i] = tolower(in[i]);

	return (out);
}

const std::string ToUpper(const std::string& in)
{
	std::string out = in;

	for (std::string::size_type i = 0 ; i < in.length() ; ++i)
		out[i] = toupper(in[i]);

	return (out);
}

const std::wstring ToLower(const std::wstring& in)
{
	std::wstring out = in;

	for (std::wstring::size_type i = 0 ; i < in.length() ; ++i)
		out[i] = tolower(in[i]);

	return (out);
}

const std::wstring ToUpper(const std::wstring& in)
{
	std::wstring out = in;

	for (std::wstring::size_type i = 0 ; i < in.length() ; ++i)
		out[i] = toupper(in[i]);

	return (out);
}

const std::string TrimBlanks(const std::string& str)
{
	std::string::size_type begin = 0;
	std::string::size_type end = str.length();

	while (begin != end && IsBlank(str[begin])) ++begin;
	while (end != begin && IsBlank(str[end - 1])) --end;

	return (std::string(str.begin() + begin, str.begin() + end));
}

const std::wstring TrimBlanks(const std::wstring& str)
{
	std::wstring::size_type begin = 0;
	std::wstring::size_type end = str.length();

	while (begin != end && IsBlank(str[begin])) ++begin;
	while (end != begin && IsBlank(str[end - 1])) --end;

	return (std::wstring(str.begin() + begin, str.begin() + end));
}

std::string WideToASCII(const std::wstring& wide) 
{
	return std::string(wide.begin(), wide.end());
}

std::wstring ASCIIToWide(const std::string& ascii) 
{
	return std::wstring(ascii.begin(), ascii.end());
}

std::tstring DataToHexStringUA(const unsigned char* pData, size_t nDataSize)
{
	std::string strHexString;

	for(size_t i = 0; i < nDataSize; i++)
	{
		char temp[32];

		sprintf(temp, "%02X", pData[i]);

		strHexString += temp;
	}

	return strHexString;
}

std::tstring DataToHexStringLA(const unsigned char* pData, size_t nDataSize)
{
	std::string strHexString;

	for(size_t i = 0; i < nDataSize; i++)
	{
		char temp[32];

		sprintf(temp, "%02x", pData[i]);

		strHexString += temp;
	}

	return strHexString;
}

std::wstring DataToHexStringUW(const unsigned char* pData, size_t nDataSize)
{
	std::wstring strHexString;

	for(size_t i = 0; i < nDataSize; i++)
	{
		wchar_t temp[32];

		swprintf_s(temp, L"%02X", pData[i]);

		strHexString += temp;
	}

	return strHexString;
}

std::wstring DataToHexStringLW(const unsigned char* pData, size_t nDataSize)
{
	std::wstring strHexString;

	for(size_t i = 0; i < nDataSize; i++)
	{
		wchar_t temp[32];

		swprintf_s(temp, L"%02x", pData[i]);

		strHexString += temp;
	}

	return strHexString;
}

const std::string FormatString( const char* format, ... )
{
	char temp[1000] = {0, };
	va_list args;
	va_start(args,format);
	vsnprintf_s(temp,1000, 1000, format,args);
	va_end(args);

	return temp;
}

} // namespace cml2 {
