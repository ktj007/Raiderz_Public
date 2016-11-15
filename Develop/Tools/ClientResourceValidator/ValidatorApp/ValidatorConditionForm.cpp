#include "StdAfx.h"
#include "ValidatorConditionForm.h"
#include "VDef.h"
#include "VFileName.h"
#include "XEffectInfo.h"
#include "VValidatorInfoMgr.h"

#include "VMeshInfoLoader.h"
#include "VTalentInfoLoader.h"
#include "VBuffInfoLoader.h"
#include "VEffectInfoLoader.h"
#include "VSoundInfoLoader.h"
#include "VCameraInfoLoader.h"
#include "VNpcInfoLoader.h"
#include "VItemInfoLoader.h"

#include "VReportMgr.h"
#include "VMeshDataReport.h"
#include "VTalentDataReport.h"
#include "VBuffDataReport.h"
#include "VEffectDataReport.h"
#include "VSoundDataReport.h"
#include "VNpcDataReport.h"
#include "VItemDataReport.h"

#include "XStrings.h"

void ValidatorApp::ValidatorConditionForm::ValidatorCondition_ListBoxText( int nIndex, String^ strText )
{
	if(ValidatorCondition_listBox->Items->Count > nIndex)
		this->ValidatorCondition_listBox->Items->RemoveAt(nIndex);

	this->ValidatorCondition_listBox->Items->Insert(nIndex, strText);
	this->ValidatorCondition_listBox->SelectedIndex = nIndex;
}

long ValidatorApp::ValidatorConditionForm::LoadingMethod( BackgroundWorker^ worker, DoWorkEventArgs ^ e, int& nIndex )
{
	XStrings::Init();

	int nFullCount = 0;
	int nCurrCount = 0;

	VMeshInfoLoader meshInfoLoader;
	VTalentInfoLoader talentInfoLoader;
	VBuffInfoLoader buffInfoLoader;
	VEffectInfoLoader effectInfoLoader;
	VSoundInfoLoader soundInfoLoader;
	VCameraInfoLoader cameraInfoLoader;
	VNpcInfoLoader npcInfoLoader;
	VItemInfoLoader itemInfoLoader;

	vector<VInfoLoader*> vecLoaderHandlers;
	vecLoaderHandlers.push_back(&meshInfoLoader);
	vecLoaderHandlers.push_back(&talentInfoLoader);
	vecLoaderHandlers.push_back(&buffInfoLoader);
	vecLoaderHandlers.push_back(&effectInfoLoader);
	vecLoaderHandlers.push_back(&soundInfoLoader);
	vecLoaderHandlers.push_back(&cameraInfoLoader);
	vecLoaderHandlers.push_back(&npcInfoLoader);
	vecLoaderHandlers.push_back(&itemInfoLoader);


	// 준비작업
	ValidatorApp::ReportProgress(worker, nIndex, 0, String::Format("Info Loading Ready..."));

	for(vector<VInfoLoader*>::iterator itReady = vecLoaderHandlers.begin(); itReady != vecLoaderHandlers.end(); ++itReady)
	{
		(*itReady)->Init();
		nFullCount += (*itReady)->GetLoadingCount();
	}

	ValidatorApp::ReportProgress(worker, nIndex++, 0, String::Format("Info Loading Ready Complete."));

	// 정보 로딩
	for(vector<VInfoLoader*>::iterator itLoading = vecLoaderHandlers.begin(); itLoading != vecLoaderHandlers.end(); ++itLoading)
	{
		(*itLoading)->Load(worker, m_pInfoMgr, nFullCount, nCurrCount, nIndex);
	}

	return 1;
}

long ValidatorApp::ValidatorConditionForm::ValidatorConditionReportMethod( BackgroundWorker^ worker, DoWorkEventArgs ^ e, int& nIndex )
{
	XStrings::Init();

	int nFullCount = 0;
	int nCurrCount = 0;

	vector<VDataReport*> vecDataReport;
	vecDataReport.push_back(m_pReportMgr->m_pMeshDataReport);
	vecDataReport.push_back(m_pReportMgr->m_pTalentDataReport);
	vecDataReport.push_back(m_pReportMgr->m_pBuffDataReport);
	vecDataReport.push_back(m_pReportMgr->m_pEffectDataReport);
	vecDataReport.push_back(m_pReportMgr->m_pSoundDataReport);
	vecDataReport.push_back(m_pReportMgr->m_pNpcDataReport);
	vecDataReport.push_back(m_pReportMgr->m_pItemDataReport);


	ValidatorApp::ReportProgress(worker, nIndex, 0, String::Format("Data Validat Ready..."));
	
	for(vector<VDataReport*>::iterator itInit = vecDataReport.begin(); itInit != vecDataReport.end(); ++itInit)
	{
		(*itInit)->Init(m_pInfoMgr);
		nFullCount += (*itInit)->GetCheckCount();
	}

	ValidatorApp::ReportProgress(worker, nIndex++, 0, String::Format("Data Validat Ready Complete."));

	for(vector<VDataReport*>::iterator itCheck = vecDataReport.begin(); itCheck != vecDataReport.end(); ++itCheck)
	{
		(*itCheck)->Start(m_pInfoMgr, worker, nFullCount, nCurrCount, nIndex);
	}

	return 1;
}