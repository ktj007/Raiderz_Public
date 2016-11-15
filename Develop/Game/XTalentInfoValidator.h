#pragma once

#include "XBaseValidator.h"
#include "XDef.h"

class GTalentInfo;

/// 탤런트 정보 검증
class XTalentInfoValidator : public XBaseValidator
{
public:
	virtual bool Check();	
};
