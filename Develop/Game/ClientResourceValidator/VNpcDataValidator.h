#pragma once

#include "VDataValidator.h"

class VNpcDataValidator : VDataValidator
{
private:
	int									m_nNpcCount;

	typedef vector<string>	MF_ANIMATION;
	map<MF_STATE, MF_ANIMATION>			m_mapMotionFactorAnimation;

public:
	VErrorMsg							m_NpcMeshValidatorError;

private:
	void								CheckMesh(XNPCInfo* pNpcInfo, VValidatorInfoMgr* vInfoMgr, string strNpcError);
	void								CheckMotionFactor(XNPCInfo* pNpcInfo, VValidatorInfoMgr* vInfoMgr, string strNpcError);

	MF_ANIMATION *						GetMotionFactorAnimationName(MF_STATE eMF);

	bool								CheckBackSlash(string strPath);

public:
	VNpcDataValidator();
	virtual ~VNpcDataValidator();

	virtual void						Init(VValidatorInfoMgr* vInfoMgr);
	virtual	void						CheckStart(VValidatorInfoMgr* vInfoMgr);

	void								CheckNpcInfo(XNPCInfo* pNpcInfo, VValidatorInfoMgr* vInfoMgr);

	virtual int							GetCheckCount() { return m_nNpcCount; }
};
