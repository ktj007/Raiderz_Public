#pragma once

#include "XModule.h"

class XModuleEntity;

//////////////////////////////////////////////////////////////////////////
/// 오브젝트의 데이타를 관리 - npc, player의 부모 클래스
class XModuleBaseData : public XModule
{
	DECLARE_ID(XMID_DATA);
protected:
public:
	XModuleBaseData(XObject* pOwner=NULL) {}
	virtual ~XModuleBaseData(void) {}
};

