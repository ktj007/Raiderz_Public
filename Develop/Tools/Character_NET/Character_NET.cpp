// Character_NET.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "Form1.h"
// 
//using namespace Character_NET;
 
[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// 주 창을 만들어 실행합니다.
	GlobalObjects::g_pGlobal	= gcnew Global();
	GlobalObjects::g_pMainForm  = gcnew Form1();

	GlobalObjects::g_pMainForm->Show();
	while(GlobalObjects::g_pMainForm->Created)
	{
		GlobalObjects::g_pMainForm->MainLoop();
		Application::DoEvents();

		if( !GlobalObjects::g_pMainForm->m_bActive )
			Sleep(1);
	}
	return 0;
}
