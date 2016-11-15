#include "stdafx.h"
#include "CSFormatString.h"
#include "AString.h"
#include "MLocale.h"
#include "FSContext.h"

CStringResMgr* CSFormatString::sm_pDictionary = NULL;

tstring CSFormatString::Format(tstring formater, FSParam param)
{
	// 유니코드로 처리한다. TCHAR는 클라이언트 툴 빌드를 위해 인터페이스에만 사용한다.
	wstring strFormater = MLocale::ConvTCHARToUTF16(formater.c_str());
	wstring strParam = MLocale::ConvTCHARToUTF16(param.m_strParam.c_str());
	wstring strSeparator = MLocale::ConvTCHARToUTF16(FSP_SEPARATOR);

	
	// 외부 인자 추출	
	vector<wstring> vecParam;
	crl::AString::Split(strParam, strSeparator, vecParam);


	// 파싱
	FSContext context(vecParam, sm_pDictionary);

	for each(wchar_t token in formater)
	{
		context.Dispatch(token);
	}

	wstring strResult = context.GetResult();


	// 유니코드 결과 tstring으로 변환
	return MLocale::ConvUTF16ToTCHAR(strResult.c_str());
}

void CSFormatString::SetDictionary(CStringResMgr* pDictionary)
{
	sm_pDictionary = pDictionary;
}
