#pragma once

#include "VDataValidator.h"

class VMeshDataValidator : public VDataValidator
{
private:
	int									m_nMeshCount;

public:
	VErrorMsg							m_AnimationListValidatorError;
	VErrorMsg							m_AnimationEventValidatorError;
	VErrorMsg							m_AnimationInfoValidatorError;

public:
	VMeshDataValidator();
	virtual ~VMeshDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);

	void								CheckAnimationXMl(VMeshInfo* pMeshInfo);
	void								CheckAnimationEvent(VMeshInfo* pMeshInfo, VValidatorInfoMgr* vInfoMgr);
	void								CheckAnimationInfo(VMeshInfo* pMeshInfo);

	virtual int							GetCheckCount() { return m_nMeshCount; }
};
