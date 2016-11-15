#include "stdafx.h"
#include "XFieldInfoBuilder.h"
#include "XGlobal.h"
#include "XGlobalVar.h"
#include "XBaseApplication.h"
#include "XFieldInfo.h"

XFieldInfo* XFieldInfoBuilder::NewFieldInfo( _EXECUTE_MODE nAppMode, int nFieldID, const wchar_t* szZoneName )
{
	switch (global.app->GetMode())
	{
	case EM_LOCAL_TEST:
		{
			return _NewFieldInfoForLocalTest(szZoneName);
		}
		break;
	case EM_GAME:
	case EM_REPLAY:
	case EM_ENGINE_DEBUG:
		{
			return _NewFieldInfo(nFieldID, szZoneName);
		}
		break;
	}

	return NULL;
}

XFieldInfo* XFieldInfoBuilder::_NewFieldInfo( int nFieldID, const wchar_t* szZoneName )
{
	XFieldInfo* pNewFieldInfo = NULL;

	if (nFieldID > 0)
	{
		pNewFieldInfo = info.field->NewFieldInfo(nFieldID);
	}
	else if (szZoneName != NULL && wcslen(szZoneName) > 0)
	{
		pNewFieldInfo = info.field->NewFieldInfoFromZone(szZoneName);
	}

	return pNewFieldInfo;
}

XFieldInfo* XFieldInfoBuilder::_NewFieldInfoForLocalTest( const wchar_t* szZoneName )
{
	XFieldInfo* pNewFieldInfo = NULL;

	string strPureName("Ritz");	// 기본값은 ritz
	if (wcslen(gvar.Network.szStartZone) > 0) strPureName = MLocale::ConvUTF16ToAnsi(gvar.Network.szStartZone);

	wstring strPureZoneName( MLocale::ConvAnsiToUTF16(MGetPureFileName(strPureName.c_str()).c_str()) );	// .zone 를 제거하기 위함

	pNewFieldInfo = info.field->NewFieldInfoFromZone(strPureZoneName.c_str());

	return pNewFieldInfo;

}