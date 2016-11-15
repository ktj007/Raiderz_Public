#include "stdafx.h"
#include "XKeyMapTable.h"
#include "XConst.h"


#define VKEY_STR(_key,_str)				m_VirtualKeyMap.insert(map<string,XVirtualKey>::value_type(_str,_key));
#define KEYCODE_STR(_key,_str,_text)	{ m_KeyboardCodeMap.insert(map<string,int>::value_type(_str,_key));	m_KeyboardDisplayStrMap.insert(map<int,string>::value_type(_key,_text));	}
#define JOYPAD_STR(_key,_str,_text)		{ m_JoypadCodeMap.insert(map<string,int>::value_type(_str,_key));	m_JoypadDisplayStrMap.insert(map<int,string>::value_type(_key,_text));	}



XKeyMapTable::XKeyMapTable()
{
	ClearKeyMapTable();

	InitVirtualKeyString();
	InitKeyCodeString();
}


void XKeyMapTable::InitVirtualKeyString()
{
	VKEY_STR( VKEY_FORWARD,				"FORWARD");
	VKEY_STR( VKEY_BACKWARD,			"BACKWARD");
	VKEY_STR( VKEY_LEFT,				"LEFT");
	VKEY_STR( VKEY_RIGHT,				"RIGHT");
	VKEY_STR( VKEY_JUMP,				"JUMP");
	VKEY_STR( VKEY_ACTION,				"ACTION");
	VKEY_STR( VKEY_ACTION2,				"ALTACTION");
	VKEY_STR( VKEY_CHANGE_STANCE,		"CHANGESTANCE");
	VKEY_STR( VKEY_AUTO_RUN,			"AUTORUN");
	VKEY_STR( VKEY_CHANGE_WEAPON,		"CHANGEWEAPON");
	VKEY_STR( VKEY_NPCINTERACTION,		"NPCINTERACTION");
	VKEY_STR( VKEY_PCINTERACTION,		"PCINTERACTION");
	VKEY_STR( VKEY_LOCKTARGET,			"LOCKTARGET");
	VKEY_STR( VKEY_TOGGLE_MOUSE,		"TOGGLEMOUSE");
	VKEY_STR( VKEY_SIT,					"SIT");
	VKEY_STR( VKEY_UI_MAINMENU,			"MAINMENU");
	VKEY_STR( VKEY_UI_CHARACTER,		"CHARACTER");
	VKEY_STR( VKEY_UI_INVENTORY,		"INVENTORY");
	VKEY_STR( VKEY_UI_TALENT,			"TALENT");
	VKEY_STR( VKEY_UI_JOURNAL,			"JOURNAL");
	VKEY_STR( VKEY_UI_SOCIAL,			"SOCIAL");
	VKEY_STR( VKEY_UI_MAP,				"MAP");
	VKEY_STR( VKEY_UI_CONTROLSHELP1,	"CONTROLSHELP1");
	VKEY_STR( VKEY_UI_CONTROLSHELP2,	"CONTROLSHELP2");
	VKEY_STR( VKEY_TOGGLE_FREECAMERA,	"TOGGLE_FREECAMERA");
	VKEY_STR( VKEY_GUARD,				"GUARD");
	VKEY_STR( VKEY_DODGE,				"DODGE");
	VKEY_STR( VKEY_UI_PALETTE0,			"PALETTE0");
	VKEY_STR( VKEY_UI_PALETTE1,			"PALETTE1");
	VKEY_STR( VKEY_UI_PALETTE2,			"PALETTE2");
	VKEY_STR( VKEY_UI_PALETTE3,			"PALETTE3");
	VKEY_STR( VKEY_UI_PALETTE4,			"PALETTE4");
	VKEY_STR( VKEY_UI_PALETTE5,			"PALETTE5");
	VKEY_STR( VKEY_UI_PALETTE6,			"PALETTE6");
	VKEY_STR( VKEY_UI_PALETTE7,			"PALETTE7");
	VKEY_STR( VKEY_UI_PALETTE8,			"PALETTE8");
	VKEY_STR( VKEY_UI_PALETTE9,			"PALETTE9");
	VKEY_STR( VKEY_UI_PALETTE_SET0,		"PALETTESET0");
	VKEY_STR( VKEY_UI_PALETTE_SET1,		"PALETTESET1");
	VKEY_STR( VKEY_UI_PALETTE_SET2,		"PALETTESET2");
	VKEY_STR( VKEY_UI_PALETTE_SET3,		"PALETTESET3");
	VKEY_STR( VKEY_UI_PALETTE_SET4,		"PALETTESET4");
}

void XKeyMapTable::InitKeyCodeString()
{
	KEYCODE_STR( KEY_BACK,		"KEY_BACK",			"Backspace");
	KEYCODE_STR( KEY_TAB,		"KEY_TAB",			"Tab");
	KEYCODE_STR( KEY_RETURN,	"KEY_RETURN",		"Return");
	KEYCODE_STR( KEY_SHIFT,		"KEY_SHIFT",		"Shift");
	KEYCODE_STR( KEY_CONTROL,	"KEY_CONTROL",		"Ctrl");
	KEYCODE_STR( KEY_ALTER,		"KEY_ALTER",		"Alt");
	KEYCODE_STR( KEY_ESCAPE,	"KEY_ESCAPE",		"Escape");
	KEYCODE_STR( KEY_SPACE,		"KEY_SPACE",		"Space");
	KEYCODE_STR( KEY_PAGEUP,	"KEY_PAGEUP",		"Page Up");
	KEYCODE_STR( KEY_PAGEDOWN,	"KEY_PAGEDOWN",		"Page Down");
	KEYCODE_STR( KEY_END,		"KEY_END",			"End");
	KEYCODE_STR( KEY_HOME,		"KEY_HOME",			"Home");
	KEYCODE_STR( KEY_LEFT,		"KEY_LEFT",			"Left Arrow");
	KEYCODE_STR( KEY_UP,		"KEY_UP",			"Up Arrow");
	KEYCODE_STR( KEY_RIGHT,		"KEY_RIGHT",		"Right Arrow");
	KEYCODE_STR( KEY_DOWN,		"KEY_DOWN",			"Down Arrow");
	KEYCODE_STR( KEY_SELECT,	"KEY_SELECT",		"Select");
	KEYCODE_STR( KEY_PRINT,		"KEY_PRINT",		"Print");
	KEYCODE_STR( KEY_SNAPSHOT,	"KEY_SNAPSHOT",		"Print Scr.");
	KEYCODE_STR( KEY_INSERT,	"KEY_INSERT",		"Insert");
	KEYCODE_STR( KEY_DELETE,	"KEY_DELETE",		"Delete");
	KEYCODE_STR( KEY_HELP,		"KEY_HELP",			"Help");
	KEYCODE_STR( KEY_KEY_0,		"KEY_0",			"0");
	KEYCODE_STR( KEY_KEY_1,		"KEY_1",			"1");
	KEYCODE_STR( KEY_KEY_2,		"KEY_2",			"2");
	KEYCODE_STR( KEY_KEY_3,		"KEY_3",			"3");
	KEYCODE_STR( KEY_KEY_4,		"KEY_4",			"4");
	KEYCODE_STR( KEY_KEY_5,		"KEY_5",			"5");
	KEYCODE_STR( KEY_KEY_6,		"KEY_6",			"6");
	KEYCODE_STR( KEY_KEY_7,		"KEY_7",			"7");
	KEYCODE_STR( KEY_KEY_8,		"KEY_8",			"8");
	KEYCODE_STR( KEY_KEY_9,		"KEY_9",			"9");
	KEYCODE_STR( KEY_KEY_A,		"KEY_A",			"A");
	KEYCODE_STR( KEY_KEY_B,		"KEY_B",			"B");
	KEYCODE_STR( KEY_KEY_C,		"KEY_C",			"C");
	KEYCODE_STR( KEY_KEY_D,		"KEY_D",			"D");
	KEYCODE_STR( KEY_KEY_E,		"KEY_E",			"E");
	KEYCODE_STR( KEY_KEY_F,		"KEY_F",			"F");
	KEYCODE_STR( KEY_KEY_G,		"KEY_G",			"G");
	KEYCODE_STR( KEY_KEY_H,		"KEY_H",			"H");
	KEYCODE_STR( KEY_KEY_I,		"KEY_I",			"I");
	KEYCODE_STR( KEY_KEY_J,		"KEY_J",			"J");
	KEYCODE_STR( KEY_KEY_K,		"KEY_K",			"K");
	KEYCODE_STR( KEY_KEY_L,		"KEY_L",			"L");
	KEYCODE_STR( KEY_KEY_M,		"KEY_M",			"M");
	KEYCODE_STR( KEY_KEY_N,		"KEY_N",			"N");
	KEYCODE_STR( KEY_KEY_O,		"KEY_O",			"O");
	KEYCODE_STR( KEY_KEY_P,		"KEY_P",			"P");
	KEYCODE_STR( KEY_KEY_Q,		"KEY_Q",			"Q");
	KEYCODE_STR( KEY_KEY_R,		"KEY_R",			"R");
	KEYCODE_STR( KEY_KEY_S,		"KEY_S",			"S");
	KEYCODE_STR( KEY_KEY_T,		"KEY_T",			"T");
	KEYCODE_STR( KEY_KEY_U,		"KEY_U",			"U");
	KEYCODE_STR( KEY_KEY_V,		"KEY_V",			"V");
	KEYCODE_STR( KEY_KEY_W,		"KEY_W",			"W");
	KEYCODE_STR( KEY_KEY_X,		"KEY_X",			"X");
	KEYCODE_STR( KEY_KEY_Y,		"KEY_Y",			"Y");
	KEYCODE_STR( KEY_KEY_Z,		"KEY_Z",			"Z");
	KEYCODE_STR( KEY_NUM_0,		"KEY_NUM_0",		"Num Pad 0");
	KEYCODE_STR( KEY_NUM_1,		"KEY_NUM_1",		"Num Pad 1");
	KEYCODE_STR( KEY_NUM_2,		"KEY_NUM_2",		"Num Pad 2");
	KEYCODE_STR( KEY_NUM_3,		"KEY_NUM_3",		"Num Pad 3");
	KEYCODE_STR( KEY_NUM_4,		"KEY_NUM_4",		"Num Pad 4");
	KEYCODE_STR( KEY_NUM_5,		"KEY_NUM_5",		"Num Pad 5");
	KEYCODE_STR( KEY_NUM_6,		"KEY_NUM_6",		"Num Pad 6");
	KEYCODE_STR( KEY_NUM_7,		"KEY_NUM_7",		"Num Pad 7");
	KEYCODE_STR( KEY_NUM_8,		"KEY_NUM_8",		"Num Pad 8");
	KEYCODE_STR( KEY_NUM_9,		"KEY_NUM_9",		"Num Pad 9");
	KEYCODE_STR( KEY_F1,		"KEY_F1",			"F1");
	KEYCODE_STR( KEY_F2,		"KEY_F2",			"F2");
	KEYCODE_STR( KEY_F3,		"KEY_F3",			"F3");
	KEYCODE_STR( KEY_F4,		"KEY_F4",			"F4");
	KEYCODE_STR( KEY_F5,		"KEY_F5",			"F5");
	KEYCODE_STR( KEY_F6,		"KEY_F6",			"F6");
	KEYCODE_STR( KEY_F7,		"KEY_F7",			"F7");
	KEYCODE_STR( KEY_F8,		"KEY_F8",			"F8");
	KEYCODE_STR( KEY_F9,		"KEY_F9",			"F9");
	KEYCODE_STR( KEY_F10,		"KEY_F10",			"F10");
	KEYCODE_STR( KEY_F11,		"KEY_F11",			"F11");
	KEYCODE_STR( KEY_F12,		"KEY_F12",			"F12");
	KEYCODE_STR( KEY_NUMLOCK,	"KEY_NUMLOCK",		"Num Lock");
	KEYCODE_STR( KEY_COMMA,		"KEY_COMMA",		", (Comma)");
	KEYCODE_STR( KEY_PLUS,		"KEY_PLUS",			"+ (Plus)");
	KEYCODE_STR( KEY_MINUS,		"KEY_MINUS",		"- (Minus)");
	KEYCODE_STR( KEY_PERIOD,	"KEY_PERIOD",		". (Period)");
	KEYCODE_STR( KEY_GRAVE,		"KEY_GRAVE",		"` (Grave)");
	KEYCODE_STR( KEY_LBUTTON,	"KEY_LBUTTON",		"Mouse L Btn.");
	KEYCODE_STR( KEY_MBUTTON,	"KEY_MBUTTON",		"Mouse M Btn.");
	KEYCODE_STR( KEY_RBUTTON,	"KEY_RBUTTON",		"Mouse R Btn.");


	// 조이패드는 아직 안넣었음
	JOYPAD_STR( 1,				"JOYPAD_BUTTON_1",	"Joypad Btn.1");
	JOYPAD_STR( 2,				"JOYPAD_BUTTON_2",	"Joypad Btn.2");
	JOYPAD_STR( 3,				"JOYPAD_BUTTON_3",	"Joypad Btn.3");
}

void XKeyMapTable::ClearKeyMapTable()
{
	memset( m_VirtualKeyData,		0, sizeof( m_VirtualKeyData));
	memset( m_KeyboardVirtualKey,	0, sizeof( m_KeyboardVirtualKey));
	memset( m_JoypadVirtualKey,		0, sizeof( m_JoypadVirtualKey));
}


void XKeyMapTable::ClearVirtualKey( XVirtualKey nVKey)
{
	switch ( m_VirtualKeyData[ nVKey].nInputType)
	{
	case IT_KEYBOARD :
	case IT_MOUSE :
		{
			for ( int i = KTKEY_NONE;  i < KTKEY_COUNT;  i++)
			{
				if ( m_KeyboardVirtualKey[ m_VirtualKeyData[ nVKey].nRowKey][ i] == nVKey)
					m_KeyboardVirtualKey[ m_VirtualKeyData[ nVKey].nRowKey][ i] = VKEY_NA;
			}
			break;
		}

	case IT_JOYPAD :
		if ( m_JoypadVirtualKey[ m_VirtualKeyData[ nVKey].nRowKey] == nVKey)
			m_JoypadVirtualKey[ m_VirtualKeyData[ nVKey].nRowKey] = VKEY_NA;
		break;
	}


	m_VirtualKeyData[ nVKey].nInputType = IT_KEYBOARD;
	m_VirtualKeyData[ nVKey].nRowKey = 0;
}


bool XKeyMapTable::SetVirtualKey( const string& strVKey, const string& strCode, const string& strExt)
{
	if ( strVKey.empty()  ||  strCode.empty())
	{
		return false;
	}


	map< string, XVirtualKey>::iterator itrVKey = m_VirtualKeyMap.find( strVKey);
	if ( itrVKey == m_VirtualKeyMap.end())
	{
		assert( 0);
		return false;
	}

	XVirtualKey nVKey = (*itrVKey).second;
	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
	{
		assert( 0);
		return false;
	}


	// 중복키 허용으로 주석처리 - birdkr
	ClearVirtualKey( nVKey);


	// Check keyboard, mouse code
	map< string, int>::iterator itrKeyboard = m_KeyboardCodeMap.find( strCode);
	if ( itrKeyboard != m_KeyboardCodeMap.end())
	{
		int nCode = (*itrKeyboard).second;
		if ( nCode < 0  ||  nCode >= KEY_KEY_CODES_COUNT)
			return false;


		// Mouse
		if ( nCode == KEY_LBUTTON  ||  nCode == KEY_MBUTTON  ||  nCode == KEY_RBUTTON)
		{
			m_VirtualKeyData[ nVKey].nInputType = IT_MOUSE;
			m_VirtualKeyData[ nVKey].nRowKey = nCode;

			m_KeyboardVirtualKey[ nCode][ KTKEY_NONE] = nVKey;
		}

		// Keyboard
		else
		{
			m_VirtualKeyData[ nVKey].nInputType = IT_KEYBOARD;
			m_VirtualKeyData[ nVKey].nRowKey = nCode;

			if ( /*nCode != KEY_CONTROL  &&*/  /*nCode != KEY_SHIFT  &&*/  nCode != KEY_ALTER)
			{
				if ( strExt == string( "shift"))		m_KeyboardVirtualKey[ nCode][ KTKEY_SHIFT] = nVKey;
				else if ( strExt == string( "ctrl"))	m_KeyboardVirtualKey[ nCode][ KTKEY_CTRL]  = nVKey;
				else if ( strExt == string( "alt"))		m_KeyboardVirtualKey[ nCode][ KTKEY_ALT]   = nVKey;
				else									m_KeyboardVirtualKey[ nCode][ KTKEY_NONE]  = nVKey;
			}
		}
	}

	
	// Check Joypad code
	map<string, int>::iterator itrJoypad = m_JoypadCodeMap.find( strCode);
	if ( itrJoypad != m_JoypadCodeMap.end())
	{
		int nCode = (*itrJoypad).second;
		if ( nCode < 0  ||  nCode >= KTJOYPAD_COUNT)
			return false;


		m_VirtualKeyData[ nVKey].nInputType = IT_JOYPAD;
		m_VirtualKeyData[ nVKey].nRowKey = nCode;

		m_JoypadVirtualKey[ nCode] = nVKey;
	}


	return true;
}


bool XKeyMapTable::SetVirtualKey( XVirtualKey nVKey, int nCode, bool bShift, bool bCtrl, bool bAlt)
{
	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
		return false;

	if ( nCode < 0  ||  nCode >= KEY_KEY_CODES_COUNT)
		return false;


	// 중복키 허용으로 주석처리 - birdkr
	ClearVirtualKey( nVKey);


	// Joypad
	if ( 0)
	{
		m_VirtualKeyData[ nVKey].nInputType = IT_JOYPAD;
		m_VirtualKeyData[ nVKey].nRowKey = nCode;

		m_JoypadVirtualKey[ nCode] = nVKey;
	}

	// Mouse
	if ( nCode == KEY_LBUTTON  ||  nCode == KEY_MBUTTON  ||  nCode == KEY_RBUTTON)
	{
		m_VirtualKeyData[ nVKey].nInputType = IT_MOUSE;
		m_VirtualKeyData[ nVKey].nRowKey = nCode;

		m_KeyboardVirtualKey[ nCode][ KTKEY_NONE] = nVKey;
	}

	// Keyboard
	else
	{
		m_VirtualKeyData[ nVKey].nInputType = IT_KEYBOARD;
		m_VirtualKeyData[ nVKey].nRowKey = nCode;

		if ( nCode != KEY_CONTROL  &&  nCode != KEY_SHIFT  &&  nCode != KEY_ALTER)
		{
			if ( bCtrl == true)			m_KeyboardVirtualKey[ nCode][ KTKEY_CTRL]	= nVKey;
			else if ( bAlt == true)		m_KeyboardVirtualKey[ nCode][ KTKEY_ALT]	= nVKey;
			else if ( bShift == true)	m_KeyboardVirtualKey[ nCode][ KTKEY_SHIFT]	= nVKey;
			else						m_KeyboardVirtualKey[ nCode][ KTKEY_NONE]	= nVKey;
		}
	}


	return true;
}


bool XKeyMapTable::SetVirtualKey( XVirtualKey nVKey, int nCode)
{
	return SetVirtualKey( nVKey, nCode, false, false, false);
}


XVirtualKey XKeyMapTable::FindVirtualKey( int nCode, bool bShift, bool bCtrl, bool bAlt)
{
	if ( nCode < 0  ||  nCode >= KEY_KEY_CODES_COUNT)
		return VKEY_NA;


	// Joypad
	if ( 0)
	{
		// to do...

		return VKEY_NA;
	}

	// Mouse
	else if ( nCode == KEY_LBUTTON  ||  nCode == KEY_MBUTTON  ||  nCode == KEY_RBUTTON)
	{
		return m_KeyboardVirtualKey[ nCode][ KTKEY_NONE];
	}

	// Keyboard
	else
	{
		if      ( bShift == true  &&  m_KeyboardVirtualKey[ nCode][ KTKEY_SHIFT] != VKEY_NA)	return m_KeyboardVirtualKey[ nCode][ KTKEY_SHIFT];
		else if ( bCtrl == true   &&  m_KeyboardVirtualKey[ nCode][ KTKEY_CTRL] != VKEY_NA)		return m_KeyboardVirtualKey[ nCode][ KTKEY_CTRL];
		else if ( bAlt == true    &&  m_KeyboardVirtualKey[ nCode][ KTKEY_ALT] != VKEY_NA)		return m_KeyboardVirtualKey[ nCode][ KTKEY_ALT];
		else if (                     m_KeyboardVirtualKey[ nCode][ KTKEY_NONE]	!= VKEY_NA)		return m_KeyboardVirtualKey[ nCode][ KTKEY_NONE];
	}


	return VKEY_NA;
}


XVirtualKey XKeyMapTable::FindVirtualKey( int nCode)
{
	return FindVirtualKey( nCode, false, false, false);
}


XVirtualKey XKeyMapTable::GetVirtualKey( const string& strVKey)
{
	map< string, XVirtualKey>::iterator itr = m_VirtualKeyMap.find( strVKey);
	if ( itr == m_VirtualKeyMap.end())
		return VKEY_NA;

	return (*itr).second;
}


string XKeyMapTable::GetVirtualKeyName( XVirtualKey nVKey)
{
	string strName;

	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
		return strName;


	for ( map< string, XVirtualKey>::iterator itr = m_VirtualKeyMap.begin();  itr != m_VirtualKeyMap.end();  itr++)
	{
		if ( (*itr).second == nVKey)
		{
			strName = (*itr).first;
			break;
		}
	}

	return strName;
}


XVirtualKeyData XKeyMapTable::GetVirtualKeyData( XVirtualKey nVKey)
{
	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
	{
		XVirtualKeyData data;
		data.nInputType = IT_KEYBOARD;
		data.nRowKey = 0;

		return data;
	}


	return m_VirtualKeyData[ nVKey];
}


int XKeyMapTable::GetCodeValue( const string& strCode)
{
	if ( strCode.empty())
		return 0;

	map< string, int>::iterator itr = m_KeyboardCodeMap.find( strCode);
	if ( itr == m_KeyboardCodeMap.end())
		return 0;

	return (*itr).second;
}


string XKeyMapTable::GetCodeString( int nCode)
{
	string str;

	if ( nCode < 0  ||  nCode >= KEY_KEY_CODES_COUNT)
		return str;

	for ( map< string, int>::iterator itr = m_KeyboardCodeMap.begin();  itr != m_KeyboardCodeMap.end();  itr++)
	{
		if ( (*itr).second == nCode)
		{
			str = (*itr).first;
			break;
		}
	}

	return str;
}


string XKeyMapTable::GetDisplayString( int nCode)
{
	string str;

	map< int, string>::iterator itr = m_KeyboardDisplayStrMap.find( nCode);
	if ( itr != m_KeyboardDisplayStrMap.end())
		str = (*itr).second;

	return str;
}


string XKeyMapTable::GetDisplayString( XVirtualKey nVKey)
{
	string strDisplay;

	if ( nVKey <= VKEY_NA  ||  nVKey >= VKEY_COUNT)
		return strDisplay;


	int nCode = m_VirtualKeyData[ nVKey].nRowKey;
	if ( nCode <= 0  ||  nCode >= KEY_KEY_CODES_COUNT)
		return strDisplay;


	switch ( m_VirtualKeyData[ nVKey].nInputType)
	{
	case IT_KEYBOARD :
	case IT_MOUSE :
		{
			if ( nCode != KEY_SHIFT  &&  nCode != KEY_CONTROL  &&  nCode != KEY_ALTER)
			{
				if ( m_KeyboardVirtualKey[ nCode][ KTKEY_SHIFT] == nVKey)	strDisplay += m_KeyboardDisplayStrMap.find( KEY_SHIFT)->second + "+";
				else if ( m_KeyboardVirtualKey[ nCode][ KTKEY_CTRL] == nVKey)	strDisplay += m_KeyboardDisplayStrMap.find( KEY_CONTROL)->second + "+";
				else if ( m_KeyboardVirtualKey[ nCode][ KTKEY_ALT] == nVKey)	strDisplay += m_KeyboardDisplayStrMap.find( KEY_ALTER)->second + "+";
			}


			map< int, string>::iterator itr = m_KeyboardDisplayStrMap.find( nCode);
			if ( itr == m_KeyboardDisplayStrMap.end())
				break;

			strDisplay += (*itr).second;
			break;
		}

	case IT_JOYPAD :
		{
			map< int, string>::iterator itr = m_JoypadDisplayStrMap.find( nCode);
			if ( itr == m_KeyboardDisplayStrMap.end())
				break;

			strDisplay = (*itr).second;
			break;
		}
	}


	return strDisplay;
}

