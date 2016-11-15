#pragma once

#include "MXml.h"

class ZAppServerInfoManager;

class ZAppServerInfoLoader
{
private:
	ZAppServerInfoManager* m_pManager;

	bool Parse(MXmlElement* pElement);

public:
	bool Load(ZAppServerInfoManager* mgr, const wchar_t* szFileName);
	bool LoadStream(ZAppServerInfoManager* mgr, const char* szStream);
};
