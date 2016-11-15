#include "stdafx.h"
#include "MLocale.h"

int	MLocale::GetCharsetFromLang(LANGID langid)
{
	switch( PRIMARYLANGID(langid) )
	{
		case LANG_JAPANESE:
			return SHIFTJIS_CHARSET;
		case LANG_KOREAN:
			return HANGEUL_CHARSET;
		case LANG_CHINESE:
			switch( SUBLANGID(langid) )
			{
				case SUBLANG_CHINESE_SIMPLIFIED:
					return GB2312_CHARSET;
				case SUBLANG_CHINESE_TRADITIONAL:
					return CHINESEBIG5_CHARSET;
				default:
					return ANSI_CHARSET;
			}
		case LANG_GREEK:
			return GREEK_CHARSET;
		case LANG_TURKISH:
			return TURKISH_CHARSET;
		case LANG_HEBREW:
			return HEBREW_CHARSET;
		case LANG_ARABIC:
			return ARABIC_CHARSET;
		case LANG_ESTONIAN:
		case LANG_LATVIAN:
		case LANG_LITHUANIAN:
			return BALTIC_CHARSET;
		case LANG_THAI:
			return THAI_CHARSET;
		case LANG_CZECH:
		case LANG_HUNGARIAN:
		case LANG_POLISH:
		case LANG_PORTUGUESE:
		case LANG_CROATIAN:
		case LANG_MACEDONIAN:
		case LANG_ROMANIAN:
		case LANG_SLOVAK:
		case LANG_SLOVENIAN:
			return EASTEUROPE_CHARSET;
		case LANG_RUSSIAN:
		case LANG_BELARUSIAN:
		case LANG_BULGARIAN:
		case LANG_UKRAINIAN:
			return RUSSIAN_CHARSET;
		default:
			return ANSI_CHARSET;
	}
}

int	MLocale::GetCodePageFromCharset(int nCharset)
{
	switch (nCharset)
	{
		case HANGUL_CHARSET:
			return 949;
		case SHIFTJIS_CHARSET:
			return 932;
		case GB2312_CHARSET:
			return 936;
		case CHINESEBIG5_CHARSET:
			return 950;
		case GREEK_CHARSET:
			return 1253;
		case TURKISH_CHARSET:
			return 1254;
		case HEBREW_CHARSET:
			return 1255;
		case ARABIC_CHARSET:
			return 1256;
		case BALTIC_CHARSET:
			return 1257;
		case THAI_CHARSET:
			return 874;
		case EASTEUROPE_CHARSET:
			return 1250;
		default:
			return 1252;
	}
}

int	MLocale::GetCodePageFromLang(LANGID langid)
{
	return GetCodePageFromCharset(GetCharsetFromLang(langid));
}


std::string MLocale::TransCode(const wchar_t *pwszString, int nCodePage)
{
	std::string strRet; 
	int nReqLen = (int)WideCharToMultiByte(nCodePage,0,pwszString,-1,0,0,0,0); 
	 
	char *pszDst = new char[nReqLen + 1]; 
	 
	// get string 
	int nLen = (int)WideCharToMultiByte(nCodePage,0,pwszString,-1,pszDst,nReqLen,0,0); 
	 
	if (nLen)
	{
		pszDst[nLen] = 0; // null terminator 
		strRet = pszDst; // copy to STL string 
	} // if (nLen) 
	 
	delete[] pszDst; // delete, if allocated 
	 
	return strRet; 
}

std::string MLocale::TransCode(const char *pszString, int nOldCodePage, int nNewCodePage)
{
	std::string strRet; 
	int nReqLen = (int)MultiByteToWideChar( nOldCodePage, 0, pszString, -1, 0, 0 ); 
	 
	// allocate buffer 
	wchar_t *pszDst = new wchar_t[nReqLen + 1]; //initially, point to array 
	 
	// get string 
	int nLen = (int)MultiByteToWideChar( nOldCodePage, 0, pszString, -1, pszDst, nReqLen ); 
	 
	if (nLen)
	{ 
		pszDst[nLen] = 0; // null terminator 
		strRet = TransCode( pszDst, nNewCodePage ); // copy to STL string 
	} // if (nLen) 
	 
	delete[] pszDst; // delete, if allocated 
	return strRet; 
}

std::string	MLocale::ConvUTF16ToAnsi(std::wstring pszString, LANGID langid)
{
	return ConvUTF16ToAnsi(pszString.c_str(), langid);
}

std::string	MLocale::ConvUTF16ToAnsi(const wchar_t* pszString, LANGID langid)
{
	if (pszString == NULL) return std::string("");
	int nCodePage = GetCodePageFromLang(langid);
	return TransCode(pszString, nCodePage);
}

std::string MLocale::ConvUTF8ToAnsi(const char* pszString, LANGID langid)
{
	if (pszString == NULL) return std::string("");
	int nCodePage = GetCodePageFromLang(langid);
	return TransCode(pszString, CP_UTF8, nCodePage);
}

std::string MLocale::ConvAnsiToUTF8(const char* pszString, LANGID langid)
{
	if (pszString == NULL) return std::string("");
	int nCodePage = GetCodePageFromLang(langid);
	return TransCode(pszString, nCodePage, CP_UTF8);
}

std::wstring MLocale::ConvAnsiToUTF16(const char* pszString, LANGID langid)
{
	int nCodePage = GetCodePageFromLang(langid);

	std::wstring strRet; 
	int nReqLen = (int)MultiByteToWideChar( nCodePage, 0, pszString, -1, 0, 0 ); 

	// allocate buffer 
	wchar_t *pszDst = new wchar_t[nReqLen + 1]; //initially, point to array 

	// get string 
	int nLen = (int)MultiByteToWideChar( nCodePage, 0, pszString, -1, pszDst, nReqLen ); 

	if (nLen)
	{ 
		pszDst[nLen] = 0; // null terminator 
		strRet = pszDst;
	} // if (nLen) 

	delete[] pszDst; // delete, if allocated 
	return strRet;
}

std::wstring MLocale::ConvAnsiToUCS2( const char* pszString, LANGID langid)
{
	return ConvAnsiToUTF16(pszString, langid);
}

std::wstring MLocale::ConvUTF8ToUTF16(const char* pszString)
{
	// 필요한 길이의 버퍼를 준비한다.
	int nReqLen = MultiByteToWideChar(CP_UTF8, 0, pszString, strlen(pszString) + 1, 0, 0);

	std::auto_ptr<wchar_t> szDest(new wchar_t[nReqLen]);


	// 버퍼에 유니코드로 변환된 문자열을 넣는다.
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pszString, strlen(pszString) + 1, szDest.get(), nReqLen);


	// 실패하면 빈문자열, 성공하면 변환된 문자열을 반환한다.
	if (nLen <= 0)
	{
		return std::wstring();
	}
	else
	{
		return szDest.get();
	}	
}

std::string	MLocale::ConvUTF16ToUTF8(const wchar_t* pszString)
{
	// 필요한 길이의 버퍼를 준비한다.
	int nReqLen = WideCharToMultiByte(CP_UTF8, 0, pszString, wcslen(pszString) + 1, 0, 0, 0, 0);

	std::auto_ptr<char> szDest(new char[nReqLen]);


	// 버퍼에 유니코드로 변환된 문자열을 넣는다.
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pszString, wcslen(pszString) + 1, szDest.get(), nReqLen, 0, 0);


	// 실패하면 빈문자열, 성공하면 변환된 문자열을 반환한다.
	if (nLen <= 0)
	{
		return std::string();
	}
	else
	{
		return szDest.get();
	}	
}
