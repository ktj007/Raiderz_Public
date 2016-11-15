#pragma once

#include "XBaseApplication.h"

class XFieldInfo;

class XFieldInfoBuilder
{
protected:
	XFieldInfo* _NewFieldInfo(int nFieldID, const wchar_t* szZoneName);
	XFieldInfo* _NewFieldInfoForLocalTest(const wchar_t* szZoneName);
public:
	XFieldInfo* NewFieldInfo(_EXECUTE_MODE nAppMode, int nFieldID, const wchar_t* szZoneName);
};