#pragma once

#include "VDataValidator.h"

class VItemDataValidator : public VDataValidator
{
private:
	int									m_nItemCount;

public:
	VErrorMsg							m_ItemValidatorError;

public:
	VItemDataValidator();
	virtual ~VItemDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);

	void								CheckItemModel(VMeshInfoMgr* vMeshInfo, XItemData* pItem);
	void								CheckItemTalent(VTalentInfoMgr* vTalentInfo, XItemData* pItem);

	virtual int							GetCheckCount() { return m_nItemCount; }
};
