#include "stdafx.h"
#include "RMouse.h"

RMouseImpl::RMouseImpl() : m_pDevice(NULL)
{
	memset(&m_MouseState, 0, sizeof(RMouseState));
}

RMouseImpl::~RMouseImpl()
{
	Destroy();
}

bool RMouseImpl::Create(LPDIRECTINPUT8 pDirectInput, HWND hWnd)
{
	HRESULT hr;
	DWORD dwCoopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

	if( FAILED( hr = pDirectInput->CreateDevice( GUID_SysMouse, &m_pDevice, NULL ) ) ) return false;
	if( FAILED( hr = m_pDevice->SetDataFormat( &c_dfDIMouse2 ) ) ) return false;

	hr = m_pDevice->SetCooperativeLevel( hWnd, dwCoopFlags );
	if( hr == DIERR_UNSUPPORTED )
	{
		Destroy();
		return false;
	}
	if( FAILED(hr) ) return false;

	// 버퍼 생성
	{
		DIPROPDWORD dipdw;

		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		dipdw.dwData            = MOUSEDATA_BUFFER_SIZE;	// Arbitary buffer size

		if( FAILED( hr = m_pDevice->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) ) return false;
	}

	if(FAILED(hr = m_pDevice->Acquire())) return false;

	memset(m_MouseState.bMouseButtonStates, false, sizeof(m_MouseState.bMouseButtonStates));

	return true;
}

void RMouseImpl::Destroy()
{
	if (m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
}

RMouseState* RMouseImpl::GetState()
{
	if( !m_pDevice ) return NULL;

	DIMOUSESTATE2 dims2;

	HRESULT hr;
	ZeroMemory( &dims2, sizeof(DIMOUSESTATE2) );
	hr = m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
	if( FAILED(hr) ) 
	{
		hr = m_pDevice->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = m_pDevice->Acquire();
		return NULL;
	}
	m_MouseState.lX = dims2.lX;
	m_MouseState.lY = dims2.lY;
	m_MouseState.lZ = dims2.lZ;
	memcpy(m_MouseState.rgbButtons, dims2.rgbButtons, sizeof(BYTE) * 8);

	return &m_MouseState;
}

RMouseData* RMouseImpl::GetData()
{
	if( !m_pDevice ) return NULL;

	DIDEVICEOBJECTDATA didod[MOUSEDATA_BUFFER_SIZE] = {0,};  // Receives buffered data 
	DWORD              dwElements;
	HRESULT            hr;

	dwElements = MOUSEDATA_BUFFER_SIZE;
	hr = m_pDevice->GetDeviceData( sizeof(DIDEVICEOBJECTDATA),
		didod, &dwElements, 0 );
	if( FAILED(hr) )
	{
		hr = m_pDevice->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = m_pDevice->Acquire();
		return NULL;
	}
	
	memcpy(m_MouseData, didod, sizeof(RMouseData) * MOUSEDATA_BUFFER_SIZE);

	return m_MouseData;
}
