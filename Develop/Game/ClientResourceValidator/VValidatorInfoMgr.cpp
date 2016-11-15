#include "stdafx.h"
#include "VValidatorInfoMgr.h"

VValidatorInfoMgr::VValidatorInfoMgr()
{
	m_pMeshInfoMgr		= NULL;
	m_pEffectInfoMgr	= NULL;
	m_pSoundInfoMgr		= NULL;
	m_pCameraInfoMgr	= NULL;
	m_pTalentInfoMgr	= NULL;
	m_pBuffInfoMgr		= NULL;
	m_pNpcInfoMgr		= NULL;
	m_pItemInfoMgr		= NULL;
}

VValidatorInfoMgr::~VValidatorInfoMgr()
{
	Destroy();
}

void VValidatorInfoMgr::Create()
{
	m_pMeshInfoMgr		= new VMeshInfoMgr;
	m_pEffectInfoMgr	= new VEffectInfoMgr;
	m_pSoundInfoMgr		= new VSoundInfoMgr;
	m_pCameraInfoMgr	= new VCameraInfoMgr;
	m_pTalentInfoMgr	= new VTalentInfoMgr;
	m_pBuffInfoMgr		= new VBuffInfoMgr;
	m_pNpcInfoMgr		= new VNpcInfoMgr;
	m_pItemInfoMgr		= new VItemInfoMgr;
}

void VValidatorInfoMgr::Destroy()
{
	if(m_pMeshInfoMgr)
		m_pMeshInfoMgr->DeleteMeshInfo();

	SAFE_DELETE(m_pMeshInfoMgr);
	SAFE_DELETE(m_pEffectInfoMgr);
	SAFE_DELETE(m_pSoundInfoMgr);
	SAFE_DELETE(m_pCameraInfoMgr);
	SAFE_DELETE(m_pTalentInfoMgr);
	SAFE_DELETE(m_pBuffInfoMgr);
	SAFE_DELETE(m_pNpcInfoMgr);
	SAFE_DELETE(m_pItemInfoMgr);
}

VErrorMsg VValidatorInfoMgr::GetInfoErrorMsg()
{
	VErrorMsg errorMsg;
	errorMsg.Add(m_pMeshInfoMgr->m_ErrorMsg);
	errorMsg.Add(m_pTalentInfoMgr->m_ErrorMsg);
	errorMsg.Add(m_pBuffInfoMgr->m_ErrorMsg);
	errorMsg.Add(m_pEffectInfoMgr->m_ErrorMsg);
	errorMsg.Add(m_pSoundInfoMgr->m_ErrorMsg);
	errorMsg.Add(m_pNpcInfoMgr->m_ErrorMsg);
	errorMsg.Add(m_pItemInfoMgr->m_ErrorMsg);
	return errorMsg;
}