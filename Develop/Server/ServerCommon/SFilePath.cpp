#include "stdafx.h"
#include "SFilePath.h"
#include "SDef.h"
#include "MFileSystem.h"

wstring SFilePath::MakeFieldList( const wchar_t* szSystemPath, bool bCheckDebugFieldList )
{
	wstring strFieldListFileName = wstring(szSystemPath) + PURE_FILENAME_FIELDLIST;
	wstring strFieldListDebugFileName = wstring(szSystemPath) + PURE_FILENAME_FIELDLIST_DEBUG;

	if (bCheckDebugFieldList)
	{
		if (MIsExistFile(strFieldListDebugFileName.c_str())) strFieldListFileName = strFieldListDebugFileName;
	}

	return strFieldListFileName;
}

wstring SFilePath::MakeZoneList( const wchar_t* szSystemPath )
{
	wstring strZoneListFileName = wstring(szSystemPath) + PURE_FILENAME_ZONELIST;
	return strZoneListFileName;
}
