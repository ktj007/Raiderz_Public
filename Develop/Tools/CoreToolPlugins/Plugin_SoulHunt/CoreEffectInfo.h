#pragma once

#include "../../Game/GameCommon/XEffectInfo.h"


// 이벤트 정보
//////////////////////////////////////////////////////////////////////////

class CoreEffectInfoMgr : public XEffectInfoMgr
{
protected:
	virtual	void				GetCommaStringSplitter(const tstring& strValue, vector<tstring>& vecOut);

public:
	CoreEffectInfoMgr();
	virtual ~CoreEffectInfoMgr(void) {}
};
