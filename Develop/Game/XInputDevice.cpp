#include "stdafx.h"
#include "XInputDevice.h"
#include "RealInput.h"
#include "RJoystick.h"
#include "RMouse.h"

XInputDevice::XInputDevice()
{
	m_pRealInput = new RealInput();
}

XInputDevice::~XInputDevice()
{
	delete m_pRealInput;
}

bool XInputDevice::Create(HWND hWnd)
{
	// 조이스틱만 사용. 키보드와 마우스는 윈도우즈 메세지를 사용한다.
	unsigned long int flags = RealInput::USE_DJOYSTICK;

	if (!m_pRealInput->Create(hWnd)) return false;

	return true;
}

void XInputDevice::Destroy()
{
	m_pRealInput->Destroy();
}

void XInputDevice::Update(float fDelta)
{
	m_pRealInput->Update();
}

bool XInputDevice::IsKeyDown(int vKey)
{
	return ((GetAsyncKeyState(vKey) & 0x8000)!=0);
}

//bool XInputDevice::IsActionKeyDown(XActionKey vKey)
//{
//	switch (vKey)
//	{
//	case XAK_LEFT:
//		{
//			if (IsKeyDown(VK_LEFT)) return true;
//			else if (m_pRealInput->GetJoystick()->IsEnabled())
//			{
//				if (m_pRealInput->GetJoystick()->GetState()->fX <= -0.90f) return true;
//			}
//			return false;
//		};
//		break;
//	case XAK_RIGHT:
//		{
//			if (IsKeyDown(VK_RIGHT)) return true;
//			else if ((m_pRealInput->GetJoystick()->IsEnabled()) && (m_pRealInput->GetJoystick()->GetState()->fX >= 0.90f)) return true;
//			return false;
//		};
//		break;
//	case XAK_UP:
//		{
//			if (IsKeyDown(VK_UP)) return true;
//			else if ((m_pRealInput->GetJoystick()->IsEnabled()) && (m_pRealInput->GetJoystick()->GetState()->fY <= -0.90f)) return true;
//			return false;
//		};
//		break;
//	case XAK_DOWN:
//		{
//			if (IsKeyDown(VK_DOWN)) return true;
//			else if ((m_pRealInput->GetJoystick()->IsEnabled()) && (m_pRealInput->GetJoystick()->GetState()->fY >= 0.90f)) return true;
//			return false;
//		};
//		break;
//	case XAK_OK:
//		{
//			if (IsKeyDown(VK_RETURN)) return true;
////			else if ((m_pJoystick) && (m_pJoystick->GetState()->lY >= JOYSTICK_AXIS_MAX)) return true;
////			return false;
//
//			//TODO: 마우스도 넣어야 한다.
//		};
//		break;
//	case XAK_CANCEL:
//		{
//			if (IsKeyDown(VK_ESCAPE)) return true;
////			else if ((m_pJoystick) && (m_pJoystick->GetState()->lY >= JOYSTICK_AXIS_MAX)) return true;
////			return false;
//		};
//		break;
//	};
//
//	return false;
//}

bool XInputDevice::GetLButtonState(void)
{
	return MEvent::GetLButtonState();
}

bool XInputDevice::GetMButtonState(void)
{
	return MEvent::GetMButtonState();
}

bool XInputDevice::GetRButtonState(void)
{
	return MEvent::GetRButtonState();
}

RMouseState* XInputDevice::GetMouseState()
{
	//RMouseData* RData = NULL;
	RMouse* pMouse = m_pRealInput->GetMouse();
	if(pMouse)
		return pMouse->GetState();

	return NULL;
}

RMouseData* XInputDevice::GetMouseData()
{
	return m_pRealInput->GetMouse()->GetData();
}