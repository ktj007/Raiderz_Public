#pragma once

#include "XModuleBaseData.h"

//////////////////////////////////////////////////////////////////////////
/// Player 오브젝트의 데이타를 관리
class XModulePlayerData : public XModuleBaseData
{
protected:
public:
	XModulePlayerData(XObject* pOwner=NULL) : XModuleBaseData(pOwner) {}
	virtual ~XModulePlayerData(void) {}
};

