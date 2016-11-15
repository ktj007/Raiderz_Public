#pragma once

#include "XBaseValidator.h"

class XNPCInfo;

/// 탤런트 정보 검증
class XNPCInfoValidator : public XBaseValidator
{
private:
	bool CheckTitle(XNPCInfo* pNPCInfo);
public:
	virtual bool Check();
};
