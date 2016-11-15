#pragma once

#include "MXml.h"

class ZLoginServerInfoManager;

class ZLoginServerInfoLoader
{
private:
	ZLoginServerInfoManager* m_pManager;

	bool Parse(MXmlElement* pElement);

public:
	bool Load(ZLoginServerInfoManager* mgr, const wchar_t* szFileName);
	bool LoadStream(ZLoginServerInfoManager* mgr, const char* szStream);
};
