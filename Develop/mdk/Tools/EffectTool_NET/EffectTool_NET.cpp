// EffectTool_NET.cpp : 기본 프로젝트 파일입니다.

#include "stdafx.h"
#include "Global.h"
#include "MainForm.h"
#include "MProfiler.h"

using namespace EffectTool_NET;

[STAThreadAttribute]

int main(array<System::String ^> ^args)
{
	// 렌더링 최소 간격
	static DWORD gTickDelay = 1;

	// 컨트롤이 만들어지기 전에 Windows XP 시각 효과를 활성화합니다.
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// 주 창을 만들어 실행합니다.
	GlobalObjects::g_pMainForm = gcnew MainForm();

	// tree 아이콘들 파일로 다시 저장하고싶을때
	/*
	 	for( int i=0;i<9; i++ )
	 	{
	 		String^ strFilename = "icons\\" + GlobalObjects::g_pMainForm->imageListSceneNodeType->Images->Keys[i];
	 		GlobalObjects::g_pMainForm->imageListSceneNodeType->Images->default[i]->Save( strFilename );
	 
	 	}
	//*/

	GlobalObjects::g_pMainForm->Show();

	DWORD dwStartTick = GetTickCount();
	while(GlobalObjects::g_pMainForm->Created)
	{	
		PFC_RESET;

		DWORD dwCurrentTick = GetTickCount();
		if ( dwCurrentTick - dwStartTick >= gTickDelay )
		{
			dwStartTick = dwCurrentTick;
			GlobalObjects::g_pMainForm->MainLoop();
		}

		Application::DoEvents();

		if( !GlobalObjects::g_pMainForm->m_bActive )
			Sleep(1);
	}

	PFC_FINALANALYSIS("profile_effectool.txt");
	return 0;
}
