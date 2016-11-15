#include "stdafx.h"
#include "PFieldInfoManager.h"

PFieldInfoManager::PFieldInfoManager()
{
	// do nothing
}

PFieldInfoManager::~PFieldInfoManager()
{
	// do nothing
}

bool PFieldInfoManager::Load(const wchar_t* szFieldList)
{
	if (!m_fieldList.Load(szFieldList))
	{
		return false;
	}

	return true;
}

CSFieldList& PFieldInfoManager::GetFieldList(void)
{
	return m_fieldList;
}
