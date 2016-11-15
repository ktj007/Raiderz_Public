#pragma once

#include "CSFieldInfo.h"

class PFieldInfoManager
{
public:
	PFieldInfoManager();
	virtual ~PFieldInfoManager();

	bool Load(const wchar_t* szFieldList);
	CSFieldList& GetFieldList(void);

private:
	CSFieldList	m_fieldList;
};
