#include "stdafx.h"
#include "VReportMgr.h"

VReportMgr::VReportMgr()
{
	m_pMeshDataReport = NULL;
	m_pTalentDataReport = NULL;
	m_pBuffDataReport = NULL;
	m_pEffectDataReport = NULL;
	m_pSoundDataReport = NULL;
	m_pNpcDataReport = NULL;
	m_pItemDataReport = NULL;
}

VReportMgr::~VReportMgr()
{
	Destroy();
}

void VReportMgr::Create()
{
	m_pMeshDataReport	= new VMeshDataReport;
	m_pTalentDataReport	= new VTalentDataReport;
	m_pBuffDataReport	= new VBuffDataReport;
	m_pEffectDataReport = new VEffectDataReport;
	m_pSoundDataReport	= new VSoundDataReport;
	m_pNpcDataReport	= new VNpcDataReport;
	m_pItemDataReport	= new VItemDataReport;
}

void VReportMgr::Destroy()
{
	SAFE_DELETE(m_pMeshDataReport);
	SAFE_DELETE(m_pTalentDataReport);
	SAFE_DELETE(m_pBuffDataReport);
	SAFE_DELETE(m_pEffectDataReport);
	SAFE_DELETE(m_pSoundDataReport);
	SAFE_DELETE(m_pNpcDataReport);
	SAFE_DELETE(m_pItemDataReport);
}
