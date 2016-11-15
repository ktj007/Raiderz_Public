#include "stdafx.h"
#include "RealInput.h"

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

#include "RJoystick.h"
#include "RKeyboard.h"
#include "RMouse.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

LPDIRECTINPUT8			g_pDI = NULL;

RealInput::RealInput()
: m_nFlags(0)
, m_pKeyboard(NULL)
, m_pMouse(NULL)
, m_pJoystick(NULL)
{
}

RealInput::~RealInput()
{
	Destroy();
}

bool RealInput::Create(HWND hWnd, unsigned long flags)
{
    HRESULT hr;

	m_nFlags = flags;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
        return false;

	if (flags & USE_DKEYBOARD)
	{
		RKeyboardImpl* pKeyBoard = new RKeyboardImpl;
		pKeyBoard->Create(g_pDI, hWnd);
		m_pKeyboard = pKeyBoard;
	}
	if (flags & USE_DMOUSE)
	{
		RMouseImpl* pMouse = new RMouseImpl;
		pMouse->Create(g_pDI, hWnd);
		m_pMouse = pMouse;
	}
	if (flags & USE_DJOYSTICK)
	{
		RJoystickImpl* pJoystick = new RJoystickImpl;
		pJoystick->Create(g_pDI, hWnd);
		m_pJoystick = pJoystick;
	}

	return true;
}

void RealInput::Destroy()
{
	if (m_nFlags & USE_DKEYBOARD)
	{
		delete m_pKeyboard;
		m_pKeyboard = NULL;
	}
	if (m_nFlags & USE_DMOUSE)
	{
		delete m_pMouse;
		m_pMouse = NULL;
	}
	if (m_nFlags & USE_DJOYSTICK)
	{
		delete m_pJoystick;
		m_pJoystick = NULL;
	}

	if (g_pDI)
	{
		g_pDI->Release();
		g_pDI = NULL;
	}
}


void RealInput::Update()
{
	if (m_nFlags & USE_DJOYSTICK)
	{
		m_pJoystick->Update();
	}
}


