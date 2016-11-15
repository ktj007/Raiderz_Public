// HelloRS3.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "crtdbg.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "../sampleApp.h"
#include "d3dx9.h"
#include "RFont.h"
#include "RRenderHelper.h"

#include <string>
#include <MXml.h>

using namespace rs3;
using namespace std;

class MyApplication : public sampleApp 
{
public:
	RFont *m_pFont;

	MyApplication() 
		: sampleApp()
	{ 
		m_pFont = NULL;
	}

	char *GetName() 
	{
		return _T("Font TestCode");
	}

	bool OnCreate();
	void OnDestroy();
	bool OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);
	void OnRender();
} g_App;


bool MyApplication::OnWndMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case 'Q' :
			PostQuitMessage(0);
			break;
		};
		break;
	}
	return sampleApp::OnWndMessage(hWnd,message,wParam,lParam,pReturn);
}

bool MyApplication::OnCreate()
{
	_ASSERT(m_pCamera);
	m_pCamera->SetNearFarZ(10, 50000);

	RFont::Initialize(m_pDevice);

	m_pFont = new RFont();

	m_pFont->AddFontResourceFromFile("data/interface/font/nanum_gothic_bold.ttf");
	if ( m_pFont->Create(m_pDevice, _T("나눔고딕Bold"), 20) == false )
		SAFE_DELETE(m_pFont);

	return true;
}

void MyApplication::OnDestroy()
{
	sampleApp::OnDestroy();

	SAFE_DELETE(m_pFont);
	RFont::Finalize();
}

void MyApplication::OnRender()
{
	sampleApp::OnRender();
	
	RRenderHelper::SetRenderState();
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,20,50,50,0x80808080);
	RRenderHelper::RenderGrid(RMatrix::IDENTITY,100,10,10,0xffffffff);
	RRenderHelper::RenderAxis(RMatrix::IDENTITY,100);

	m_pFont->BeginFont();
	if(m_pCamera )
		m_pDevice->SetTransform(RST_VIEW, m_pCamera->GetViewMatrix());
// 	for ( int i = 0; i<50; ++i )
// 	{
// 		/*int x = ::rand()%m_rcWindowBounds.right;
// 		int y = ::rand()%m_rcWindowBounds.bottom;*/
// 		int x = 50;
// 		int y = i * 10;
// 
// 		m_pFont->DrawText(x, y, _T("Hello, 안녕하세요. 랄라 구헤헤헤 그라지요~"), -1, D3DCOLOR_XRGB(255, 0, 0));
// 		m_pFont->DrawText(x, y+10, _T(" , Hahahaha 나루나루 킥쑈~"), -1, D3DCOLOR_XRGB(255, 0, 0));
// 		//m_pFont->DrawText(x, y, _T("Hello, 안녕하세요. 랄라 , Hahahaha"), -1, D3DCOLOR_XRGB(255, 0, 0));
// 	}

	m_pFont->DrawTextIn3DSpace(RMatrix::IDENTITY,"Hello, 안녕하세요. 랄라 구헤헤헤 그라지요~");
	m_pFont->EndFont();

	//m_pFont->BeginFont();
	//for ( int i = 0; i<50; ++i )
	//{
	//	/*int x = ::rand()%m_rcWindowBounds.right;
	//	int y = ::rand()%m_rcWindowBounds.bottom;*/
	//	int x = 50;
	//	int y = i * 10;

	//	m_pFont->DrawText(x, y, _T("Hello, 안녕하세요. 랄라 , Hahahaha"), -1, D3DCOLOR_XRGB(255, 0, 0));
	//	//m_pFont->DrawText(x, y, _T("Hello, 안녕하세요. 랄라 , Hahahaha"), -1, D3DCOLOR_XRGB(255, 0, 0));
	//}
	//m_pFont->EndFont();
}
 
int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	return g_App.Run();
}

