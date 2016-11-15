#pragma once

#include "crlLib.h"
#include <string>

using namespace std;

namespace crl {

/// 스트링 처리를 도와주는 헬퍼 클래스
class CRL_API AString
{
private:
	static std::wstring MapChars(const std::wstring& strInput, wint_t (*fpMapping)(wint_t));
	static const int c_nWcharBufSize = 4096;
public:
	static const std::string TrimBlanks( const std::string& str );
	static const bool IsBlank( const std::string::value_type c );
	static const string ToLower( const string& in );
	static void Replace(std::string& text, const std::string& find_token, const std::string& replace_token);
	static int Split(wstring strString, wstring strDelimiters, vector<wstring>& vecSplitStringList);	

	static std::string			Trim(const std::string& strInput, const std::string& strDelim);
	static std::string			TrimLeft(const std::string& strInput, const std::string& strDelim);
	static std::string			TrimRight(const std::string& strInput, const std::string& strDelim);
	static std::wstring			Trim(const std::wstring& strInput, const std::wstring& strDelim);
	static std::wstring			TrimLeft(const std::wstring& strInput, const std::wstring& strDelim);
	static std::wstring			TrimRight(const std::wstring& strInput, const std::wstring& strDelim);

	// 문자열(strInput)을 특정문자들(strDelim)을 기준으로 나눠준다.
	static void Split(std::vector<std::string>& vecOutputToken, const std::string& strInput, const std::string& strDelim);
	static void Split(std::vector<std::wstring>& vecOutputToken, const std::wstring& strInput, const std::wstring& strDelim);

	// 문자열을 특정 문자의 최초 등장 위치를 기준으로 나눈다.
	static std::pair<std::string, std::string> SplitOnce(const std::string& strInput, const std::string& strDelim);
	static std::pair<std::wstring, std::wstring> SplitOnce(const std::wstring& strInput, const std::wstring& strDelim);

	// 문자열을 최대 길이로 구분해서 나눠준다.
	static void SplitLength(std::vector<std::string>& vecOutputToken, const std::string& strInput, int nMaxLength);
	static void SplitLength(std::vector<std::wstring>& vecOutputToken, const std::wstring& strInput, int nMaxLength);

	static void SplitPathAndFileName(std::string& strPath, std::string& strFileName, const std::string& strInput);
	static void SplitPathAndFileName(std::wstring& strPath, std::wstring& strFileName, const std::wstring& strInput);

	static std::string ToUpper(const std::string& strInput);
	static std::wstring ToUpper(const std::wstring& strInput);
	static std::wstring ToLower(const std::wstring& strInput);

	// context 안에 있는 from 을 to 로 일괄변환하여 context 자신을 리턴한다
	static std::string& ReplaceAll(std::string& context, const std::string& from, const std::string& to);
	static std::wstring& ReplaceAll(std::wstring& context, const std::wstring& from, const std::wstring& to);

	static std::string Join(const std::vector<std::string>& vecInput,  const std::string& s = "");
	static std::wstring Join(const std::vector<std::wstring>& vecInput,  const std::wstring& s = L"");

	// strSuper 가 strWhetherSuffix를 suffix로 갖는지
	static bool EndsWith(const std::string& strSuper, const std::string& strWhetherSuffix);
	static bool EndsWith(const std::wstring& strSuper, const std::wstring& strWhetherSuffix);

	// strSuper 가 strWhetherPrefix를 prefix로 갖는지
	static bool StartsWith(const std::string& strSuper, const std::string& strWhetherPrefix);
	static bool StartsWith(const std::wstring& strSuper, const std::wstring& strWhetherPrefix);

	static int ToInt(const std::string &strToConvert, int nDefault = 0);
	static double ToDouble(const std::string &strToConvert, double fDefault = 0);

	static inline std::string ToString(const std::string& strString) { return strString; }
	static std::string ToString(const std::wstring& strString);
	static std::wstring ToWstring(const std::string& strString);
	static inline std::wstring ToWstring(const std::wstring& strString) { return strString; }
	static void ToString(const std::vector<std::wstring>& vecString, std::vector<std::string>& vecResult);
	static void ToWstring(const std::vector<std::string>& vecString, std::vector<std::wstring>& vecResult);

	static std::string Format(const char *format);
};


} // namespace crl