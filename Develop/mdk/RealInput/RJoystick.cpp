#include "stdafx.h"
#include "RJoystick.h"
#include "RealInput.h"
#include <cmath>

#define JOYSTICK_AXIS_MAX	1000.0f
#define JOYSTICK_POLLING_INTERVAL	(1000/16)	// 초당 60번

BOOL CALLBACK RJoystickImpl::EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext )
{
    HRESULT hr;

	RJoystickImpl* pJoystick = (RJoystickImpl*)pContext;

    // Obtain an interface to the enumerated joystick.
	hr = (pJoystick->m_pDirectInput)->CreateDevice( pdidInstance->guidInstance, 
																		&(pJoystick->m_pDevice),
																		NULL );

    // If it failed, then we can't use this joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED(hr) ) 
        return DIENUM_CONTINUE;
	//-------------------------------------------------------------------------------------
	DIDEVCAPS caps;
	caps.dwSize = sizeof(DIDEVCAPS);

	hr = pJoystick->m_pDevice->GetCapabilities(&caps);
	if(SUCCEEDED(hr))
	{
		pJoystick->m_State.m_nJoyButtons	= caps.dwButtons;
		pJoystick->m_State.m_nJoyPovs		= caps.dwPOVs;
		pJoystick->m_State.m_bForceFeedback = (caps.dwFlags & DIDC_FORCEFEEDBACK)!=0;
	}
	//-------------------------------------------------------------------------------------

    // Stop enumeration. Note: we're just taking the first joystick we get. You
    // could store all the enumerated joysticks and let the user pick.
    return DIENUM_STOP;
}

BOOL CALLBACK RJoystickImpl::EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext )
{
	RJoystickImpl* pJoystick = (RJoystickImpl*)pContext;

    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls
	//-------------------------------------------------------------------------------------
	if( (pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0 )
		pJoystick->m_State.m_nFFAxis++;
	//-------------------------------------------------------------------------------------
    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg; 
        diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        diprg.diph.dwHow        = DIPH_BYID; 
        diprg.diph.dwObj        = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin              = long(-JOYSTICK_AXIS_MAX);
        diprg.lMax              = long(JOYSTICK_AXIS_MAX); 
    
        // Set the range for the axis
        if( FAILED( pJoystick->m_pDevice->SetProperty( DIPROP_RANGE, &diprg.diph ) ) ) 
            return DIENUM_STOP;
    }

	return DIENUM_CONTINUE;
}

RJoystickImpl::RJoystickImpl() : m_pDirectInput(NULL), m_pDevice(NULL), m_nInterval(JOYSTICK_POLLING_INTERVAL)
{
	memset(&m_State, 0, sizeof(RJoystickState));
}

RJoystickImpl::~RJoystickImpl()
{
	Destroy();
}

bool RJoystickImpl::Create(LPDIRECTINPUT8 pDirectInput, HWND hWnd)
{
	m_pDirectInput = pDirectInput;

	HRESULT hr;

	if( FAILED( hr = pDirectInput->EnumDevices( DI8DEVCLASS_GAMECTRL, 
										 EnumJoysticksCallback,
										 (LPVOID)this,
                                         DIEDFL_ATTACHEDONLY ) ) )
        return false;

    // Make sure we got a joystick
    if( NULL == m_pDevice )
    {
		// 조이스틱이 연결되어 있지 않다.
		return false;
    }

    if( FAILED( hr = m_pDevice->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
        return false;
	}

	// 협조레벨 설정
    if( FAILED( hr = m_pDevice->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND ) ) )
	{
        return false;
	}

	//DIDEVCAPS caps;
	//memset(&caps, 0, sizeof(DIDEVCAPS));
	//caps.dwSize = sizeof(DIDEVCAPS);
	//if ( FAILED( hr = m_pDevice->GetCapabilities( &caps ) ) )
	//{
	//	return false;
	//}

    if( FAILED( hr = m_pDevice->EnumObjects( EnumObjectsCallback, 
                                                (VOID*)this, DIDFT_ALL ) ) )
        return false;


//-------------------------------------------------------------------------------------
//추가 부분..
	if(m_State.m_nFFAxis>2)
		m_State.m_nFFAxis = 2;

	// 포스피드백 이펙트 초기화. 실패하면 disable 시키고 그대로 진행한다
	if(m_State.m_bForceFeedback && m_State.m_nFFAxis>0)
	{
		// This application needs only one effect: Applying raw forces.
		DWORD           rgdwAxes[2]     = { DIJOFS_X, DIJOFS_Y };
		LONG            rglDirection[2] = { 0, 0 };
		DICONSTANTFORCE cf              = { 0 };

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                  = sizeof(DIEFFECT);
		eff.dwFlags                 = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.dwDuration              = INFINITE;
		eff.dwSamplePeriod          = 0;
		eff.dwGain                  = DI_FFNOMINALMAX;
		eff.dwTriggerButton         = DIEB_NOTRIGGER;
		eff.dwTriggerRepeatInterval = 0;
		eff.cAxes                   = m_State.m_nFFAxis;
		eff.rgdwAxes                = rgdwAxes;
		eff.rglDirection            = rglDirection;
		eff.lpEnvelope              = 0;
		eff.cbTypeSpecificParams    = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams   = &cf;
		eff.dwStartDelay            = 0;

		// Create the prepared effect
		if( FAILED( hr = m_pDevice->CreateEffect( GUID_ConstantForce, 
			&eff, &m_State.m_pFFEffect, NULL ) ) )
		{
			m_State.m_bForceFeedback = false;
		}

		if(m_State.m_pFFEffect)
			m_State.m_pFFEffect->Start( 1, 0 ); // Start the effect
		else
			m_State.m_bForceFeedback = false;
	}
//-------------------------------------------------------------------------------------
	return true;
}

void RJoystickImpl::Destroy()
{
	if (m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
//-------------------------------------------------------------------------------------
		SAFE_RELEASE( m_State.m_pFFEffect );
	}
}

bool RJoystickImpl::GetJoystickData(DIJOYSTATE2* pjs)
{
//	DIJOYSTATE2 js;           // DInput Joystick state 

	if( NULL == m_pDevice ) return false;

	// Poll the device to read the current state
	HRESULT hr = m_pDevice->Poll(); 
	if( FAILED(hr) )  
	{
		// DInput is telling us that the input stream has been
		// interrupted. We aren't tracking any state between polls, so
		// we don't have any special reset that needs to be done. We
		// just re-acquire and try again.
		hr = m_pDevice->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = m_pDevice->Acquire();	// 위험한디..

		// hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
		// may occur when the app is minimized or in the process of 
		// switching, so just try again later 
		return false; 
	}

	// Get the input's device state
	if( FAILED( hr = m_pDevice->GetDeviceState( sizeof(DIJOYSTATE2), pjs ) ) )
		return false; // The device should have been acquired during the Poll()

	return true;
}

bool RJoystickImpl::SetDeviceForcesXY(int nXForce, int nYForce)
{
	if(!m_State.m_bForceFeedback || NULL==m_State.m_pFFEffect) return false;

	// Modifying an effect is basically the same as creating a new one, except
	// you need only specify the parameters you are modifying
	LONG rglDirection[2] = { 0, 0 };

	DICONSTANTFORCE cf;

	if( m_State.m_nFFAxis == 1 )
	{
		// If only one force feedback axis, then apply only one direction and 
		// keep the direction at zero
		cf.lMagnitude = nXForce;
		rglDirection[0] = 0;
	}
	else
	{
		// If two force feedback axis, then apply magnitude from both directions 
		rglDirection[0] = nXForce;
		rglDirection[1] = nYForce;
		cf.lMagnitude = (DWORD)sqrt( (double)nXForce * (double)nXForce +
			(double)nYForce * (double)nYForce );
	}

	DIEFFECT eff;
	ZeroMemory( &eff, sizeof(eff) );
	eff.dwSize                = sizeof(DIEFFECT);
	eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.cAxes                 = m_State.m_nFFAxis;
	eff.rglDirection          = rglDirection;
	eff.lpEnvelope            = 0;
	eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay            = 0;

	// Now set the new parameters and start the effect immediately.
	HRESULT hr = m_State.m_pFFEffect->SetParameters( &eff, DIEP_DIRECTION |
		DIEP_TYPESPECIFICPARAMS |
		DIEP_START );

	return SUCCEEDED(hr);
}


void RJoystickImpl::Update()
{
	if (m_pDevice == NULL) return;

	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - m_nLastTime) < m_nInterval) return;
	m_nLastTime = nNowTime;


	HRESULT hr;
	DIJOYSTATE joystate;           // DInput joystick state 

    hr = m_pDevice->Poll(); 

    if( FAILED(hr) )  
    {
        hr = m_pDevice->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = m_pDevice->Acquire();

		memset(&m_State, 0, sizeof(RJoystickState));
        return ; 
    }

    if( FAILED( hr = m_pDevice->GetDeviceState( sizeof(DIJOYSTATE), &joystate ) ) ) 
	{
		memset(&m_State, 0, sizeof(RJoystickState));
		return;
	}

	m_State.fX = joystate.lX / JOYSTICK_AXIS_MAX;
	m_State.fY = joystate.lY / JOYSTICK_AXIS_MAX;
	m_State.fZ = joystate.lZ / JOYSTICK_AXIS_MAX;
	m_State.fRx = joystate.lRx / JOYSTICK_AXIS_MAX;
	m_State.fRy = joystate.lRy / JOYSTICK_AXIS_MAX;
	m_State.fRz = joystate.lRz / JOYSTICK_AXIS_MAX;
	memcpy(m_State.rglSlider, joystate.rglSlider, sizeof(LONG) * 2);
	memcpy(m_State.rgdwPOV, joystate.rgdwPOV, sizeof(DWORD) * 4);
	memcpy(m_State.rgbButtons, joystate.rgbButtons, sizeof(BYTE) * 32);
}


bool RJoystickImpl::IsButtonDown(int nIndex)
{
	if ((nIndex >= 0) && (nIndex < JOYSTICK_BUTTON_COUNT))
	{
		if ( m_State.rgbButtons[nIndex] & 0x80 )
		{
			return true;
		}
	}

	return false;
}