// ValidatorApp.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "Form1.h"
#include "MFileSystem.h"

using namespace ValidatorApp;


[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// File system 초기화
	MCreateFileSystem( MFILEACCESS_GENERIC);

	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// 인자값 확인
	if(args->Length > 0 && args[0] == "ccnet")
	{
		GlobalObjects::g_ValidatorCCNet = true;		
	}

	// 주 창을 만들어 실행합니다.
	GlobalObjects::g_pMainForm = gcnew Form1();
	GlobalObjects::g_pMainForm->m_bOneExecute = GlobalObjects::g_ValidatorCCNet;
	Application::Run(GlobalObjects::g_pMainForm);

	return GlobalObjects::g_ValidatorResult;
}
