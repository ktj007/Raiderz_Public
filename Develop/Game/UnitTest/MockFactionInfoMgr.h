#pragma once

#include "XFactionInfoMgr.h"

class MockFactionInfoMgr : public XFactionInfoMgr
{
public:
	//MockFactionInfoMgr(void);
	//~MockFactionInfoMgr(void);

	virtual bool CreateFactionFromString(const std::wstring& _strXmlData);	// mockFactionMgr
};