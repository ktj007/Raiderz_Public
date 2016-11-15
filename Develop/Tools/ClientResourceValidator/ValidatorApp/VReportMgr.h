#pragma once

#include "VMeshDataReport.h"
#include "VTalentDataReport.h"
#include "VBuffDataReport.h"
#include "VEffectDataReport.h"
#include "VSoundDataReport.h"
#include "VNpcDataReport.h"
#include "VItemDataReport.h"

class VReportMgr
{
public:
	VMeshDataReport *				m_pMeshDataReport;
	VTalentDataReport *				m_pTalentDataReport;
	VBuffDataReport *				m_pBuffDataReport;
	VEffectDataReport *				m_pEffectDataReport;
	VSoundDataReport *				m_pSoundDataReport;
	VNpcDataReport *				m_pNpcDataReport;
	VItemDataReport *				m_pItemDataReport;

public:
	VReportMgr();
	virtual ~VReportMgr();

	void							Create();
	void							Destroy();
};
