#include "stdafx.h"
#include "XObject_Visible.h"
#include "RCameraSceneNode.h"
#include "RJoystick.h"
#include "RSceneManager.h"
#include "XEventID.h"
#include "XGlueGameEvent.h"


#define MAX_KEY_SEQUENCE_TIME	2000



struct XVirtualKeyBuff
{
	XVirtualKey		key;
	float			time;
};




XGameInput::XGameInput()
{
	m_bEnable = true;
	m_nQueueSize = 1;
	m_nLastSequenceActions = -1;

	memset( m_VirtualKeyPressedTable, 0, sizeof( m_VirtualKeyPressedTable));

	m_KeyBuffer.AddFilter( VKEY_FORWARD);
	m_KeyBuffer.AddFilter( VKEY_BACKWARD);
	m_KeyBuffer.AddFilter( VKEY_LEFT);
	m_KeyBuffer.AddFilter( VKEY_RIGHT);
	m_KeyBuffer.AddFilter( VKEY_ACTION);
	m_KeyBuffer.AddFilter( VKEY_JUMP);
	m_KeyBuffer.AddFilter( VKEY_ACTION2);
	m_KeyBuffer.AddFilter( VKEY_GUARD);
	m_KeyBuffer.AddFilter( VKEY_DODGE);
	//m_KeyBuffer.AddFilter( VKEY_TOGGLE_FREELOOK);
}


XGameInput::~XGameInput()
{
}


bool XGameInput::OnEvent( MEvent& e)
{
	XVirtualKey nVKey = VKEY_NA;
	bool bPressed = false;


	switch ( e.nMessage)
	{
	case MWM_LBUTTONDOWN:
		nVKey = FindVirtualKey( KEY_LBUTTON);
		bPressed = true;
		break;

	case MWM_LBUTTONUP:
		nVKey = FindVirtualKey( KEY_LBUTTON);
		break;

	case MWM_LBUTTONDBLCLK:
		nVKey = FindVirtualKey( KEY_LBUTTON);
		bPressed = true;
		break;

	case MWM_MBUTTONDOWN:
		nVKey = FindVirtualKey( KEY_MBUTTON);
		bPressed = true;
		break;

	case MWM_MBUTTONUP:
		nVKey = FindVirtualKey( KEY_MBUTTON);
		break;

	case MWM_MBUTTONDBLCLK:
		nVKey = FindVirtualKey( KEY_MBUTTON);
		bPressed = true;
		break;

	case MWM_RBUTTONDOWN:
		nVKey = FindVirtualKey( KEY_RBUTTON);
		bPressed = true;
		break;

	case MWM_RBUTTONUP:
		nVKey = FindVirtualKey( KEY_RBUTTON);
		break;

	case MWM_RBUTTONDBLCLK:
		nVKey = FindVirtualKey( KEY_RBUTTON);
		bPressed = true;
		break;

	case MWM_KEYDOWN:
		nVKey = FindVirtualKey( e.nKey, MEvent::GetShiftState(), e.bCtrl, MEvent::GetAltState());
		bPressed = true;
		break;

	case MWM_KEYUP:
		nVKey = FindVirtualKey( e.nKey, MEvent::GetShiftState(), e.bCtrl, MEvent::GetAltState());
		break;
	};

	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
		return false;


	if ( CheckEnableKey( nVKey) == false)
		return false;


	XEvent msg;

	if ( bPressed == true) 
	{
		bool bPrevPressed = m_VirtualKeyPressedTable[ nVKey];

		m_VirtualKeyPressedTable[ nVKey] = true;
		msg.m_nID = XEVTL_VIRTUAL_KEY_PRESSED;

		if ( m_KeyBuffer.CheckFilter( nVKey))
		{
			KEYNODE key_node( nVKey, XGetNowTime(), KEYNODE::PRESSED);

			if ( bPrevPressed == false) 
			{
				key_node.nState = KEYNODE::PRESSONCE;

				m_KeyBuffer.Push( key_node);

				XEvent msg2;
				msg2.m_nID = XEVTL_VIRTUAL_KEY_PRESSEDONCE;
				msg2.m_pData = &nVKey;

				if ( gg.omgr)
					gg.omgr->PostEvent( XGetMyUID(), msg2);
			}
			else
				m_KeyBuffer.Push( key_node);
		}
	}

	else 
	{
		m_VirtualKeyPressedTable[ nVKey] = false;
		msg.m_nID = XEVTL_VIRTUAL_KEY_RELEASED;

		if ( m_KeyBuffer.CheckFilter( nVKey))
		{
			KEYNODE key_node( nVKey, XGetNowTime(), KEYNODE::RELEASED);
			m_KeyBuffer.Push(key_node);
		}
	}

	
	// 메세지 전송
	msg.m_pData = &nVKey;
	if ( gg.omgr)
		gg.omgr->PostEvent( XGetMyUID(), msg);


	return false;
}


int XGameInput::CheckSequenceKeyCommand()
{
	unsigned int nNowTime = XGetNowTime();


	// 유효 시간이 초과한 키 입력 삭제
	while ( !m_KeyBuffer.IsEmpty())
	{
		const KEYNODE& node = m_KeyBuffer.Get( 0);
		if ( (nNowTime - node.nTime) > MAX_KEY_SEQUENCE_TIME)
			m_KeyBuffer.Pop();
		else
			break;
	}


	int nSequence = -1;

	if ( m_KeyBuffer.IsEmpty() == false)
	{
		// Make key buffer
		int nPressOnceKeySize = 0;
		XVirtualKeyBuff* KeyBuff = new XVirtualKeyBuff[ m_KeyBuffer.GetSize()];
		for ( int i = 0;  i < m_KeyBuffer.GetSize();  i++)
		{
			KEYNODE node = m_KeyBuffer.Get( i);

			if ( node.nState != KEYNODE::PRESSONCE)
				continue;

			KeyBuff[ nPressOnceKeySize].key  = node.nKeycode;
			KeyBuff[ nPressOnceKeySize].time = node.nTime;
			nPressOnceKeySize++;
		}


		if ( nPressOnceKeySize > 0)
		{
			// Check key sequence
			int nMaxCombo = 0;
			unsigned int nLastTime = 0;
			int nSequenceSize = m_ActionKeyDef.GetSize();
			for ( int seq = 0;  seq < nSequenceSize;  seq++)
			{
				XActionKey& action = m_ActionKeyDef.GetTable( seq);

				int index = nPressOnceKeySize - action.GetKeyCount();
				if ( index < 0 || index >= (int)m_KeyBuffer.GetSize())
					continue;

				if (action.nExtKey != VKEY_NA)
				{
					if (IsVirtualKeyPressedAsync(action.nExtKey) == false)
						continue;
				}

				if ( (nNowTime - KeyBuff[ index].time) > (unsigned int)( action.m_fTotalTime * 1000))
					continue;


				bool bExist = true;
				int _pos;
				for ( _pos = 0;  _pos < (int)action.GetKeyCount();  _pos++)
				{
					if ( KeyBuff[ index + _pos].key != action.sequenceKeys[ _pos])
					{
						bExist = false;
						break;
					}
				}

				if ( bExist)
				{
					// 콤보 키 수가 많을수록 우선순위가 높음
					if ( (int)action.GetKeyCount() > nMaxCombo)
					{
						nMaxCombo = action.GetKeyCount();
						nSequence = action.nSpecialActionCommand;
					}
				}
			}
		}


		delete [] KeyBuff;
	}

	if ( nSequence != -1)
	{
		m_KeyBuffer.Clear();

		m_nLastSequenceActions = nSequence;

		return nSequence;
	}


	return -1;
}


void XGameInput::SetEnabled( bool bEnable)
{
	if ( m_bEnable == bEnable)
		return;

		
	m_bEnable = bEnable;

	if ( bEnable == false)
		ClearKeyBuffer( true);
}


bool XGameInput::IsEnabled()
{
	return m_bEnable;
}


bool XGameInput::CheckEnableKey( XVirtualKey nVKey)
{
	if ( m_bEnable == false)
		return false;

	return true;
}


bool XGameInput::IsVirtualKeyPressed( XVirtualKey nVKey)
{
	if ( CheckEnableKey( nVKey) == false)
		return false;

	return m_VirtualKeyPressedTable[ nVKey];
}


bool XGameInput::IsVirtualKeyPressedAsync( XVirtualKey nVKey)
{
	if ( CheckEnableKey( nVKey) == false)
		return false;


	if ( ::GetFocus() != global.mint->GetHWND())
		return false;


	switch ( m_VirtualKeyData[ nVKey].nInputType)
	{
	case IT_KEYBOARD:
		{
			unsigned int nKeyboardKey = m_VirtualKeyData[ nVKey].nRowKey;
			if ( (GetAsyncKeyState( nKeyboardKey) & 0x8000) == 0)
			{
				return false;
			}

			if ( m_KeyboardVirtualKey[ nKeyboardKey][ KTKEY_SHIFT] == nVKey)
			{
				if ( (GetAsyncKeyState( VK_SHIFT) & 0x8000) == 0)
					return false;
			}
			else if ( m_KeyboardVirtualKey[ nKeyboardKey][ KTKEY_CTRL] == nVKey)
			{
				if ( (GetAsyncKeyState( VK_CONTROL) & 0x8000) == 0)
					return false;
			}
			else if ( m_KeyboardVirtualKey[ nKeyboardKey][ KTKEY_ALT] == nVKey)
			{
				if ( (GetAsyncKeyState( VK_MENU) & 0x8000) == 0)
					return false;
			}

			return true;
		}
		break;

	case IT_MOUSE:
		{
			int nCode = m_VirtualKeyData[ nVKey].nRowKey;

			switch ( nCode)
			{
			case KEY_LBUTTON :	return MEvent::GetLButtonState();
			case KEY_MBUTTON :	return MEvent::GetMButtonState();
			case KEY_RBUTTON :	return MEvent::GetRButtonState();
			}
		}
		break;

	case IT_JOYPAD:
		break;
	}

	return false;
}


void XGameInput::AddReservedKey( XVirtualKey nVKey)
{
	while ( m_qReservedKey.size() >= m_nQueueSize)
		m_qReservedKey.pop();

	m_qReservedKey.push( nVKey);
}


XKeyBuffer& XGameInput::GetKeyBuffer()
{
	return m_KeyBuffer;
}


queue< XVirtualKey>& XGameInput::GetReservedKey()
{
	return m_qReservedKey;
}


void XGameInput::ClearKeyBuffer( bool bPostReleasedKeyEvent)
{
	for ( int nVKey = VKEY_NA;  nVKey < VKEY_COUNT;  nVKey++)
	{
		if ( m_VirtualKeyPressedTable[ nVKey] == true)
		{
			if (bPostReleasedKeyEvent)
			{
				m_VirtualKeyPressedTable[ nVKey] = false;

				if ( m_KeyBuffer.CheckFilter( (XVirtualKey)nVKey))
				{
					KEYNODE key_node( (XVirtualKey)nVKey, XGetNowTime(), KEYNODE::RELEASED);
					m_KeyBuffer.Push( key_node);
				}


				XEvent msg( XEVTL_VIRTUAL_KEY_RELEASED, &nVKey);

				if ( gg.omgr)
					gg.omgr->PostEvent( XGetMyUID(), msg);
			}

			m_VirtualKeyPressedTable[ nVKey] = false;
		}
	}

	m_KeyBuffer.Clear();
}
