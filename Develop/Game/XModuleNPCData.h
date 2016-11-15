#pragma once

#include "XModuleBaseData.h"

//////////////////////////////////////////////////////////////////////////
/// NPC 오브젝트의 데이타를 관리
class XModuleNPCData : public XModuleBaseData
{
protected:
public:
	XModuleNPCData(XObject* pOwner=NULL) : XModuleBaseData(pOwner) {}
	virtual ~XModuleNPCData(void) {}
};

