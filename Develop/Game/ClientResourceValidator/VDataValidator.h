#pragma once

#include "VErrorMsg.h"
#include "VValidatorInfoMgr.h"

class VDataValidator
{
public:
	VErrorMsg							m_ValidatorError;

public:
	VDataValidator();
	virtual ~VDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr) {}
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr) {}

	virtual int							GetCheckCount() { return 0; }

};
