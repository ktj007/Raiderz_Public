#pragma once

#include "VDataValidator.h"

class VBuffDataValidator : VDataValidator
{
private:
	int									m_nBuffCount;

public:
	VBuffDataValidator();
	virtual ~VBuffDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);


	virtual int							GetCheckCount() { return m_nBuffCount; }
};
