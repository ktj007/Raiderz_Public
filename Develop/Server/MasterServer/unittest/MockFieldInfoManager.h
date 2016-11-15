#pragma once

#include "ZFieldInfoManager.h"

class MockFieldInfoManager : public ZFieldInfoManager
{
public:
	MockFieldInfoManager() : ZFieldInfoManager() {}
	virtual ~MockFieldInfoManager() {}
	void AddFieldInfo(int nFieldID, const wchar_t* szFieldName, const wchar_t* szFieldFile, bool bDynamic, int nDefaultChannelCount);
	void AddFieldGroupInfo(int nFieldGroupID, int nFirstDynamicFieldID);
};