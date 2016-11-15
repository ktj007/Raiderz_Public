#include "stdafx.h"
#include "ClientValidator.h"
#include "ValidatorConditionForm.h"
#include "DataLoad_Validator.h"
#include "VFileName.h"
#include "VDef.h"
#include "Form1.h"

ClientValidator *g_pValidatorApp = NULL;

ClientValidator::ClientValidator()
{

}

ClientValidator::~ClientValidator()
{
	Destroy();
}

std::string ClientValidator::SetCurrentDir()
{
	// Current Directory를 맞춘다.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

	return string(szModuleFileName);
}

void ClientValidator::Init()
{
	// 생성
	Create();

	// 현재 위치 설정
	SetCurrentDir();
}

void ClientValidator::Loading_Validator()
{
	// 초기화에 대한 에러 표기
	// 에러가 있나?
	VErrorMsg errorMsg = m_ValidatorInfoMgr.GetInfoErrorMsg();
	if(errorMsg.IsError() == false)
		return;
	
	ValidatorApp::DataLoad_Validator dialogDataLoad_Validator;

	dialogDataLoad_Validator.AddErrorMsg(errorMsg);

	dialogDataLoad_Validator.ShowDialog();
}

void ClientValidator::Check_Validate()
{
	ValidatorApp::ValidatorConditionForm	dialogValidatorCondition;

	dialogValidatorCondition.SetValidatorInfoMgr(&m_ValidatorInfoMgr);
	dialogValidatorCondition.SetReportMgr(&m_ReportMgr);

	dialogValidatorCondition.ShowDialog();
	System::Windows::Forms::DialogResult res = dialogValidatorCondition.DialogResult;
	if (res == System::Windows::Forms::DialogResult::OK)
	{
		Loading_Validator();
		Result_Validator();
	}
}

void ClientValidator::Create()
{
	m_ValidatorInfoMgr.Create();
	m_ReportMgr.Create();
}

void ClientValidator::Destroy()
{
	m_ValidatorInfoMgr.Destroy();
	m_ReportMgr.Destroy();
}

void ClientValidator::Start()
{
	Destroy();
	Init();
	Check_Validate();
}

void ClientValidator::Result_Validator()
{
	ValidatorApp::GlobalObjects::g_pMainForm->SetValidatorReport(&m_ReportMgr);
}