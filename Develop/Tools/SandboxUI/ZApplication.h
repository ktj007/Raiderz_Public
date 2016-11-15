
#pragma once

#include <string>
#include <map>
#include "../../mdk/Mint3.2/Util/Mint4R3.h"
#include "RD3DApplication.h"
#include "RSceneManager.h"
#include "MWLua.h"
#include "ZScript.h"
using namespace rs3;


// Name of application
#define APPLICATION_NAME		"HELLOWORLD_APP"



/*	class : ZApplication
*/
class ZApplication : public RD3DApplication
{
private:
	// Member variables
	static ZApplication*		m_pInstance;			// Singleton instance
	MDrawContextR3*				m_pDC;					// Draw context
	MFontR3*					m_pFont;				// Default font
	ZScript*					m_pLuaScript;			// Lua Script
	Mint4R3						m_Mint;					// Mint


public:
	// Constructor
	ZApplication();

	// Destructor
	virtual ~ZApplication();



	// Create application
	virtual bool				Create();
	bool OnCreate();

	// Destroy application
	void OnDestroy();

	// Reload resource
	bool Reload();

	// Update application
	void OnUpdate();

	// Rendering
	void OnRender();

	// Message procedure
	bool OnWndMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pReturn);



	/** Interface functions **/

	// Get name of application
	char* GetName()				{ return APPLICATION_NAME;	}

	// Get draw context
	MDrawContextR3* GetDC()		{ return m_pDC;				}

	// Get font
	MFontR3* GetFont()			{ return m_pFont;			}

	// Singleton Instance
	static ZApplication* GetInstance();

	// Find widget
	MWidget* FindWidget( const char* pszName);

	// Show interface
	void ShowInterface( const char* pszName);

	// Get device
	RDevice* GetDevice()		{ return REngine::GetDevicePtr();	}

	// Change the screen resolution
	void ChangeResolution( int width, int height);

	// GlobalMintEventCallback
	static bool GlobalMintEventCallback( MEvent* pEvent);


protected:
	// Initialize system
	bool InitSystem();

	// Initialize interfaces
	bool InitInterface();

	// Create scene manager
	RSceneManager* CreateSceneManager();
};


/************************************************************************/
/* Inline Functions                                                     */
/************************************************************************/

/*	ZGetApplication()
	Get instance of application
*/
inline ZApplication* ZGetApplication()		{ return ZApplication::GetInstance();	}
