#pragma once

#include "XEffectInfo.h"


// 이벤트 정보
//////////////////////////////////////////////////////////////////////////

class XGameEffectInfoMgr : public XEffectInfoMgr
{
protected:
	virtual	void				GetCommaStringSplitter(const tstring& strValue, vector<tstring>& vecOut);

public:
	XGameEffectInfoMgr();
	virtual ~XGameEffectInfoMgr(void) {}
};
