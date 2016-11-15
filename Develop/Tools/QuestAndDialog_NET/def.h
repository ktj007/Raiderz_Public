#pragma once

#define FILENAME_GAMESERVER_ROOTPATH		"../GameServer/"
#define FILENAME_NPCINFO					"system/npc.xml"

#define MStringToCharPointer(stringname) \
	(const char*) System::Runtime::InteropServices::Marshal \
	::StringToHGlobalAnsi(stringname).ToPointer()


//유니코드용
namespace std
{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
