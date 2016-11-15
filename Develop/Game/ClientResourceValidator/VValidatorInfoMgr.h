#pragma once

#include "VMeshInfoMgr.h"
#include "VEffectInfoMgr.h"
#include "VSoundInfoMgr.h"
#include "VCameraInfoMgr.h"
#include "VTalentInfoMgr.h"
#include "VBuffInfoMgr.h"
#include "VNpcInfoMgr.h"
#include "VItemInfoMgr.h"

class VValidatorInfoMgr
{
public:
	VMeshInfoMgr *					m_pMeshInfoMgr;
	VEffectInfoMgr *				m_pEffectInfoMgr;
	VSoundInfoMgr *					m_pSoundInfoMgr;
	VCameraInfoMgr *				m_pCameraInfoMgr;
	VTalentInfoMgr *				m_pTalentInfoMgr;
	VBuffInfoMgr *					m_pBuffInfoMgr;
	VNpcInfoMgr *					m_pNpcInfoMgr;
	VItemInfoMgr *					m_pItemInfoMgr;

public:
	VValidatorInfoMgr();
	virtual ~VValidatorInfoMgr();

	void							Create();
	void							Destroy();

	VErrorMsg						GetInfoErrorMsg();
};
