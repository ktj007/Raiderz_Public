
#include "stdafx.h"
#include "XAntiHack.h"



// XAntiHack
XAntiHack::XAntiHack()
{
	m_hWnd = NULL;
	m_pAntiHackModule = NULL;
}



// ~XAntiHack
XAntiHack::~XAntiHack()
{
	if ( m_pAntiHackModule != NULL)
	{
		if ( m_pAntiHackModule->OnDestroy() == true)
		{
			delete m_pAntiHackModule;
			m_pAntiHackModule = NULL;
		}
	}
}


// CreateModule
bool XAntiHack::CreateModule()
{
	if ( m_pAntiHackModule != NULL)		return false;


	XAntiHackModule* pModule = NULL;

#ifdef XANTIHACK
#ifdef _KOREA

	pModule = new XAntiHackModuleGameGuardKor;

#endif

	if ( pModule == NULL)				return false;
	m_pAntiHackModule = pModule;

	if ( m_pAntiHackModule->OnCreate() == false)
		return false;

#endif

	return true;
}


// RemoveModule
bool XAntiHack::RemoveModule()
{
	if ( m_pAntiHackModule == NULL)		return false;

	if ( m_pAntiHackModule->OnDestroy() == false)
		return false;

	delete m_pAntiHackModule;
	m_pAntiHackModule = NULL;

	return true;
}


// Get singleton instance
XAntiHack* XAntiHack::GetInstance()
{
	static XAntiHack instance;
	return &instance;
}


// ShowErrorMessageBox
void XAntiHack::ShowErrorMessageBox()
{
	if ( m_pAntiHackModule == NULL)		return;

	m_pAntiHackModule->OnShowErrorMessageBox();
}


// CreatedWindow
void XAntiHack::CreatedWindow( HWND hWnd)
{
	m_hWnd = hWnd;

	if ( m_pAntiHackModule == NULL)		return;
	m_pAntiHackModule->OnCreatedWindow( hWnd);
}


// DestroyedWindow
void XAntiHack::DestroyedWindow()
{
	m_hWnd = NULL;

	if ( m_pAntiHackModule == NULL)		return;
	m_pAntiHackModule->OnDestroyedWindow();
}


// PlayerLogin
void XAntiHack::PlayerLogin( const wchar_t* szID)
{
	if ( m_pAntiHackModule == NULL)		return;

	bool bRetVal = m_pAntiHackModule->OnPlayerLogin( szID);
	if ( bRetVal == false)
	{
		// To do...
	}
}


// PlayerLogout
void XAntiHack::PlayerLogout( const wchar_t* szID)
{
	if ( m_pAntiHackModule == NULL)		return;

	bool bRetVal = m_pAntiHackModule->OnPlayerLogout( szID);
	if ( bRetVal == false)
	{
		// To do...
	}
}


// CheckAuthority
bool XAntiHack::CheckAuthority( XAntiHackAuthInfo* pInfo)
{
	if ( m_pAntiHackModule == NULL  ||  pInfo == NULL)
		return false;

	bool bRetVal = m_pAntiHackModule->OnCheckAuthority( pInfo);
	if ( bRetVal == false)
	{
		// To do...

		return false;
	}

	return true;
}


// CheckedAuthority
bool XAntiHack::CheckedAuthority( XAntiHackAuthInfo* pInfo)
{
	if ( m_pAntiHackModule == NULL  ||  pInfo == NULL)
		return false;

	bool bRetVal = m_pAntiHackModule->OnCheckedAuthority( pInfo);
	if ( bRetVal == false)
	{
		// To do...

		return false;
	}

	return true;
}


// Update
void XAntiHack::Update()
{
#ifdef XANTIHACK

	// Check module
	if ( m_pAntiHackModule == NULL)
	{
		// An error occurred
		mlog( "The program terminates due to unknown error in the security module.\n");

		PostMessage( m_hWnd, WM_CLOSE, 0, 0);
	}


	// Update anti-hack module
	if ( m_pAntiHackModule->OnUpdate() == false)
	{
		// An error occurred
		mlog( "The program terminates due to error in the security module. (error code = %d)\n", m_pAntiHackModule->GetLastErrorCode());

		PostMessage( m_hWnd, WM_CLOSE, 0, 0);
	}

#endif
}
