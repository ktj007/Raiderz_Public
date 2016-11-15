#ifndef _XKEYMAP_TABLE_H
#define _XKEYMAP_TABLE_H

#include "XVirtualKeyDef.h"
#include "XKeyCodes.h"

class XGameInput;

#define KTJOYPAD_COUNT		32



enum XInputType
{
	IT_KEYBOARD = 0,
	IT_MOUSE,
	IT_JOYPAD
};

struct XVirtualKeyData
{
	XInputType		nInputType;
	unsigned int	nRowKey;
};




class XKeyMapTable
{
	friend class XGameInput;


public:
	const enum KeyboardKey
	{
		KTKEY_NONE = 0,
		KTKEY_CTRL,
		KTKEY_ALT,
		KTKEY_SHIFT,
		KTKEY_COUNT
	};


private:
	XVirtualKeyData				m_VirtualKeyData[ VKEY_COUNT];
	map< string, XVirtualKey>	m_VirtualKeyMap;

	XVirtualKey					m_KeyboardVirtualKey[ KEY_KEY_CODES_COUNT][ KTKEY_COUNT];
	XVirtualKey					m_JoypadVirtualKey[ KTJOYPAD_COUNT];

	map< string, int>			m_KeyboardCodeMap;
	map< string, int>			m_JoypadCodeMap;

	map< int, string>			m_KeyboardDisplayStrMap;
	map< int, string>			m_JoypadDisplayStrMap;



public:
	XKeyMapTable();

	void InitVirtualKeyString();
	void InitKeyCodeString();

	void ClearKeyMapTable();
	void ClearVirtualKey( XVirtualKey nVKey);

	bool SetVirtualKey( const string& strVKey, const string& strCode, const string& strExt);
	bool SetVirtualKey( XVirtualKey nVKey, int nCode, bool bShift, bool bCtrl, bool bAlt);
	bool SetVirtualKey( XVirtualKey nVKey, int nCode);

	XVirtualKey FindVirtualKey( int nCode, bool bShift, bool bCtrl, bool bAlt);
	XVirtualKey FindVirtualKey( int nCode);

	XVirtualKey GetVirtualKey( const string& strVKey);
	string GetVirtualKeyName( XVirtualKey nVKey);
	XVirtualKeyData GetVirtualKeyData( XVirtualKey nVKey);

	int GetCodeValue( const string& strCode);
	string GetCodeString( int nCode);

	string GetDisplayString( int nCode);
	string GetDisplayString( XVirtualKey nVKey);
};


#endif