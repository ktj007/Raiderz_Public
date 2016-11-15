#pragma once

#include "XBaseValidator.h"
#include "XItemData.h"

/// 아이템 정보 검증
class XItemValidator : public XBaseValidator
{
private:
	bool CheckFile(XItemData* pItemData);
	bool CheckVisual(XItemData* pItemData);
	bool CheckWeaponMeahFindNode( XItemData* pItemData );
public:
	virtual bool Check();	
};
