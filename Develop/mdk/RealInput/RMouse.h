#ifndef _RMOUSE_H
#define _RMOUSE_H

#include "RealInput.h"
//#include "MDebug.h"

/// Mouse Device
class RMouseImpl : public RMouse
{
private:

protected:
	friend class RealInput;
	RealInput*					m_pRealInput;
	LPDIRECTINPUTDEVICE8		m_pDevice;
	RMouseState					m_MouseState;
	RMouseData					m_MouseData[MOUSEDATA_BUFFER_SIZE];

public:
								RMouseImpl();
	virtual						~RMouseImpl();

	bool						Create(LPDIRECTINPUT8 pDirectInput, HWND hWnd);
	void						Destroy();
	bool						IsEnabled()			{ return (m_pDevice != NULL); }
	RMouseState*				GetState() override;
	RMouseData*					GetData() override;
};



#endif // _RMOUSE_H