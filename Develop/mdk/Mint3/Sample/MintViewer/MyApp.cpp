#include "StdAfx.h"
#include "shlwapi.h"
#include "MyApp.h"
#include "Mint.h"
#include "MFrame.h"
#include "../util/Mint4R3.h"
#include "MintLoader.h"
#include "MMsgBox.h"
#include "MDebug.h"

#include "RConfiguration.h"

using namespace rs3;
using namespace mint3;

Mint4R3			g_Mint;

MDrawContextR3* g_pDC		= NULL;
MFontR3*		g_pDefFont	= NULL;
MMsgBox*		g_pMsgBox	= NULL;

char g_szText[256]	= "";
char g_szText2[256]	= "";

int g_nWidth = 800, g_nHeight = 600;

void ShowMessage(const char* szArg1)
{
	//	const char* szArg1 = g_App.GetLua()->GetStringArgument(1);
	if (szArg1) strcpy(g_szText, szArg1);

	return;
}

void ShowMessage2(const char* szArg1)
{
	//	const char* szArg1 = g_App.GetLua()->GetStringArgument(1);
	if (szArg1) strcpy(g_szText2, szArg1);
	return;
}

void ShowFrame()
{
	MFrame* pFrame = (MFrame*)Mint::GetInstance()->FindWidget("MyFrame");
	if (pFrame)
	{
		pFrame->Show(!pFrame->GetVisible());
	}

	return;
}

MyApp::MyApp(void) : RD3DApplication()
{
}

MyApp::~MyApp(void)
{
}

void MyApp::OnDestroy()
{
	SAFE_DELETE(m_pFont);

	SAFE_DELETE(g_pDefFont);

	g_Mint.Finalize();

	delete g_pDC;  g_pDC = NULL;

	m_pLua->Close();
	delete m_pLua; m_pLua = NULL;

	MFontManager::Destroy();
	MBitmapManager::Destroy();

	RFont::Finalize();
}

void MyApp::ChangeDirectoryToModule()
{
	// Current Directory¸¦ ¸ÂÃá´Ù.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);
}

void MyApp::OnInit()
{
	ChangeDirectoryToModule();

	MInitLog(MLOG_LEVEL_DEBUG, MLOG_FILE|MLOG_DEBUG);

	rs3::config.SetShaderPath("Data/Shader");
	rs3::config.SetTexturePath("Data/Texture/Character;Data/Texture/Map;Data/Texture/SFX;Data/Texture/SpeedTree;Data/Texture/Tile;Data/Texture/Weapon");
	rs3::config.SetTreePath("Data/Model/SpeedTree");
	rs3::config.SetModelPath("Data/Model/MapObject");
	rs3::config.m_nWidth		= 800;
	rs3::config.m_nHeight		= 600;
	rs3::config.m_bFullScreen	= false;
}

bool MyApp::Create()
{
	if(!RD3DApplication::Create()) return false;

	m_pFont = new RFont;

	m_pLua = new MWLua();
	m_pLua->Open();
	m_pLua->Def("ShowMessage", ShowMessage);
	m_pLua->Def("ShowMessage2", ShowMessage2);
	m_pLua->Def("ShowFrame", ShowFrame);

	return true;
}

bool MyApp::OnCreate()
{
	g_pDefFont = new MFontR3();

	if( !g_pDefFont->Create(GetDevice(), "Default", "±¼¸²", 12, 1.0f) )
	{
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
	}

	g_pDC = new MDrawContextR3((RDeviceD3D*)(GetDevice()));

	g_Mint.Initialize(g_nWidth, g_nHeight, g_nWidth, g_nHeight, g_pDC, g_pDefFont, m_pLua);
	g_Mint.SetHWND(GetWindowHandle());

	SetCursor(LoadCursor(NULL, IDC_ARROW));
	ShowCursor(TRUE);

	mint3::MintLoader loader;
	loader.LoadFromFile("Data/Interface/GameMain.xml");
	
	g_Mint.FindWidget(string("game"))->Show();

	//g_pMsgBox = new MMsgBox("", Mint::GetInstance()->GetMainFrame(), NULL, MT_OK);
	//g_pMsgBox->Show(true);

	return true;
}

void MyApp::OnUpdate()
{

}

void MyApp::OnRender()
{
	int x = rs3::config.m_nWidth - 70;
	char text[256];
	sprintf(text, "FPS = %u", m_FrameCounter.Update());
	MGetMint()->GetDC()->SetColor(MCOLOR(0xFFFFFFFF));

	g_pDC->BeginFont();
	g_pDC->Text(10, 10, g_szText);
	g_pDC->Text(10, 25, g_szText2);
	g_pDC->Text(x, 10, text);
	g_pDC->EndFont();

	g_Mint.Run();
	g_Mint.Draw();
}

bool MyApp::OnWndMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn )
{
	switch (message)
	{
	case WM_CHAR:
		if(wParam==VK_ESCAPE) PostMessage(hWnd,WM_CLOSE,0,0);
		break;
	};

	if(Mint::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam)==true)
		return false;

	return false;
}
