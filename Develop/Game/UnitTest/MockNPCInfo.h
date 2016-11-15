#pragma once

#include "XNPCInfo.h"

class MockNPCInfoMgr : public XNPCInfoMgr
{
public:
	virtual bool CreateNPCFromString( const std::wstring& _strXmlData );
};
