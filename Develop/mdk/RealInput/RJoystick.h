#ifndef _RJOYSTICK_H
#define _RJOYSTICK_H

#include "RealInput.h"

class RealInput;


#define JOYSTICK_BUTTON_COUNT		32

struct RJoystickState
{
	float	fX;
	float	fY;
	float	fZ;
	float	fRx;
	float	fRy;
	float	fRz;
    LONG    rglSlider[2];							/* extra axes positions         */
    DWORD   rgdwPOV[4];								/* POV directions               */
    BYTE    rgbButtons[JOYSTICK_BUTTON_COUNT];      /* 32 buttons                   */

	unsigned int			m_nJoyButtons;		///< joystick buttons
	unsigned int			m_nJoyPovs;			///< joystick povs
	unsigned int			m_nFFAxis;			///< force feedback axis
	bool					m_bForceFeedback;
	LPDIRECTINPUTEFFECT		m_pFFEffect;		///< force feedback effect
};

class RJoystickImpl : public RJoystick
{
public:
	RJoystickImpl();
	virtual ~RJoystickImpl();

	bool Create(LPDIRECTINPUT8 pDirectInput, HWND hWnd);
	void Destroy();
	void Update() override;
	RJoystickState* GetState()		{ return &m_State; }
	bool IsButtonDown(int nIndex);
	bool IsEnabled()				{ return (m_pDevice != NULL); }

	// pov 개수 얻어오기
	unsigned int GetJoystickPovCount() override		{ return m_State.m_nJoyPovs; }
	// 버튼 개수 얻어오기
	unsigned int GetJoystickButtonCount() override	{ return m_State.m_nJoyButtons; }
	// 조이스틱 상태 얻어오기
	bool GetJoystickData(DIJOYSTATE2* pjs);

	bool SetDeviceForcesXY(int nXForce, int nYForce);

protected:
	LPDIRECTINPUT8				m_pDirectInput;
	LPDIRECTINPUTDEVICE8		m_pDevice;
	RJoystickState				m_State;
	unsigned long int			m_nLastTime;
	unsigned long int			m_nInterval;

	static BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );
	static BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
};



#endif