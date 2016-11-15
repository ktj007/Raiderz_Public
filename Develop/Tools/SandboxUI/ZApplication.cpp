

#include "stdafx.h"
#include "ZApplication.h"
#include "RConfiguration.h"
#include "MMainFrame.h"
#include "ZMintLoader.h"
#include "MResourceManager.h"
#include "MFileSystem.h"



/************************************************************************/
/* Define                                                               */
/************************************************************************/

/*	ADD_INTERFACE
	Add interface
*/
#define ADD_INTERFACE(_CLASS,_FILE,_SHEET)											\
{																					\
	_CLASS* pInterface = new _CLASS;												\
	pInterface->Create( _FILE, _SHEET);												\
	m_Interface.insert( map<string,ZInterface*>::value_type( _SHEET, pInterface));	\
}



/************************************************************************/
/* Class                                                                */
/************************************************************************/

/*	class : FPS
	Definition of FPS
*/
class FPS
{
private:
	unsigned long m_nTime;
	unsigned long m_nFrameCounter;
	unsigned long m_nCurrFrame;
	unsigned long m_nLastTime;

	unsigned long m_nTickTime;
public:
	FPS() : m_nTime(0), m_nFrameCounter(0), m_nCurrFrame(0)	{ m_nLastTime = timeGetTime();	}

	int Update()
	{
		m_nFrameCounter++;
		unsigned long nNowTime = timeGetTime();

		m_nTickTime = (nNowTime - m_nLastTime);
		m_nTime += m_nTickTime;
		m_nLastTime = nNowTime;

		if (m_nTime >= 1000)
		{
			m_nCurrFrame = m_nFrameCounter;
			m_nFrameCounter = 0;
			m_nTime = 0;
		}

		return m_nCurrFrame;
	}

	int GetTick() 
	{
		return m_nTickTime;
	}

} g_FPS;



/************************************************************************/
/* Functions                                                            */
/************************************************************************/

/*	ZApplication
	Constructor
*/
ZApplication* ZApplication::m_pInstance = NULL;

ZApplication::ZApplication()
{
	m_pInstance		= this;
	m_pDC			= NULL;
	m_pFont			= NULL;
	m_pLuaScript	= new ZScript;
}


/*	~ZApplication
	Destructor
*/
ZApplication::~ZApplication()
{
}


bool ZApplication::Create()
{
	MCreateFileSystem( MFILEACCESS_GENERIC);

	bool bReturn = RApplication::Create();

	return true;
}


/*	OnCreate
	Create app window
*/
bool ZApplication::OnCreate()
{
	MInitLog( MLOG_LEVEL_INFORMATION, MLOG_ALL, NULL, "mlog.txt", "mlog.xml", "http://maiet.net/~zelinde/mlog.xsl");


	// Initialize font device
	RFont::Initialize( GetDevice());


	// Create default font
	m_pFont = new MFontR3();
	if ( !m_pFont->Create( GetDevice(), "Default", "±¼¸²", 12))
	{
		m_pFont->Destroy();
		SAFE_DELETE( m_pFont);
	}


	// Create draw context
	m_pDC = new MDrawContextR3( (rs3::RDeviceD3D*)(GetDevice()));
	if ( !m_pDC)
	{
		assert( m_pDC);
		return false;
	}


	// Create Lua script
	if ( !m_pLuaScript->Create())
	{
		assert( 0);
		return false;
	}


	Reload();


	MCursorSystem::Set("aniCursor");

	return true;
}


/*	OnDestroy
	Destroy app window
*/
void ZApplication::OnDestroy()
{
	// Destroy default font
	SAFE_DELETE( m_pFont);


	// Destroy Mint
	m_Mint.Finalize();


	// Destroy DC
	SAFE_DELETE( m_pDC);


	// Destroy Lua script
	m_pLuaScript->Destroy();
	SAFE_DELETE( m_pLuaScript);


	// Destroy managers
	MFontManager::Destroy();
	MBitmapManager::Destroy();
	RFont::Finalize();
}


/*	OnUpdate
	Update window
*/
void ZApplication::OnUpdate()
{
	// Update Mint
	m_Mint.Run();
}


/*	Reload
	Reload resource
*/
bool ZApplication::Reload()
{
	m_Mint.Finalize();


	// Create Mint
	int width  = REngine::GetConfig().m_nWidth;
	int height = REngine::GetConfig().m_nHeight;
	m_Mint.Initialize( width, height, 1024, 768, m_pDC, m_pFont, m_pLuaScript->GetLua());
	m_Mint.SetHWND( GetWindowHandle());


	MEvent::SetIMESupport( true);


	// Set cursor
	SetCursor( LoadCursor( NULL, IDC_ARROW));
	ShowCursor( TRUE);


	// Initialize system
	if ( !InitSystem())
		return false;


	// Initialize interface
	if ( !InitInterface())
		return false;


	// Load interface
	ZMintLoader mloader;
	bool bRetVal = mloader.LoadFromFile( "Data/Interface/ui_main.xml");
	if ( bRetVal == false)
		return false;


	// Show interface
	MWidget* pInterface = Mint::GetInstance()->FindWidget( "game");
	if ( pInterface)
		pInterface->Show( true);


	return true;
}


/*	OnRender
	Rendering window screen
*/
void ZApplication::OnRender()
{
	m_pDC->BeginDraw();
	m_pDC->SetColor( 0, 0, 0);
	m_pDC->FillRectangle( 0, 0, REngine::GetConfig().m_nWidth, REngine::GetConfig().m_nHeight);
	m_pDC->EndDraw();


	// Rendering Mint
	m_Mint.Draw();


	// Draw FPS
	char text[ 256];
	sprintf_s( text, "FPS = %u", g_FPS.Update());
	MGetMint()->GetDC()->SetColor( MCOLOR( 0xFFFFFFFF));

	m_pDC->BeginDraw();
	m_pDC->SetFont( MFontManager::Get( NULL));
	m_pDC->Text( REngine::GetConfig().m_nWidth - 70, 10, text);
	m_pDC->EndDraw();
}


/*	OnWndMessage
	Message procedure
*/
bool ZApplication::OnWndMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn)
{
	// Procedure messages
	if ( Mint::GetInstance()->ProcessEvent( hWnd, message, wParam, lParam) == true)
		return true;


	return false;
}


/*	GetInstance
	Get singleton instance
*/
ZApplication* ZApplication::GetInstance()
{
	return m_pInstance;
}


/*	FindWidget
	Find widget
*/
MWidget* ZApplication::FindWidget( const char* pszName)
{
	return Mint::GetInstance()->FindWidget( pszName);
}


/*	ShowInterface
	Show interface
*/
void ZApplication::ShowInterface( const char* pszName)
{
}


/*	InitSystem
	Initialize system
*/
bool ZApplication::InitSystem()
{
	return true;
}


/*	InitInterface
	Initialize interfaces
*/
bool ZApplication::InitInterface()
{
	MCursorSystem::Set("normal");

	return true;
}


/*	ChangeResolution
	Change the screen resolution
*/
void ZApplication::ChangeResolution( int width, int height)
{
	// Reset device
	REngine::GetConfig().m_nWidth = width;
	REngine::GetConfig().m_nHeight = height;
	ResetDevice();


	// Set workspace
	Mint::GetInstance()->SetWorkspaceSize( width, height);
	Mint::GetInstance()->GetMainFrame()->SetSize( width, height);
}


/*	CreateSceneManager
	Create scene manager
*/
bool bStaticCreated = false;
RSceneManager* ZApplication::CreateSceneManager()
{
	using namespace rs3;
	class RMySceneManager : public RSceneManager
	{
	public:
		RMySceneManager() { }

		virtual bool Create(RDevice* pDevice)
		{
			// Static object
			if ( bStaticCreated == false )
			{
				if ( !RFont::Initialize( pDevice ) )
					return false;

				bStaticCreated = true;
			}
			return true;
		}
	};

	RMySceneManager* pSceneManager = new RMySceneManager;
	REngine::RegisterSceneManager(pSceneManager);
	return pSceneManager;
}
