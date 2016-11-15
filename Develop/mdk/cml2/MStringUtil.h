#pragma once

#include "RLib.h"
#include <string>
#include "MTstring.h"

namespace cml2 {


inline const bool IsBlank(const std::string::value_type c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

inline const bool IsBlank(const std::wstring::value_type c)
{
	return (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n');
}

CML2_API const std::string ToLower(const std::string& in);
CML2_API const std::string ToUpper(const std::string& in);
CML2_API const std::wstring ToLower(const std::wstring& in);
CML2_API const std::wstring ToUpper(const std::wstring& in);

CML2_API const std::string TrimBlanks(const std::string& str);
CML2_API const std::wstring TrimBlanks(const std::wstring& str);
CML2_API const std::string FormatString(const char* format, ...);



CML2_API std::string WideToASCII(const std::wstring& wide);
CML2_API std::wstring ASCIIToWide(const std::string& ascii);

CML2_API std::string DataToHexStringUA(const unsigned char* pData, size_t nDataSize);
CML2_API std::string DataToHexStringLA(const unsigned char* pData, size_t nDataSize);
CML2_API std::wstring DataToHexStringUW(const unsigned char* pData, size_t nDataSize);
CML2_API std::wstring DataToHexStringLW(const unsigned char* pData, size_t nDataSize);

#ifdef _UNICODE
#define DataToHexStringU	DataToHexStringUW
#define DataToHexStringL	DataToHexStringLW
#else
#define DataToHexStringU	DataToHexStringUA
#define DataToHexStringL	DataToHexStringLA
#endif

} // namespace cml2 {
