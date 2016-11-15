#pragma once

#include "MWLua.h"
#include "MFileSystem.h"	// for MIsExistFile
#include "GDef.h"			// for VALID_RET

inline string _MAKE_LUANAME_IMPL(const char* pszName, int nID1=-1, int nID2=-1)	
{ 
	string strName = pszName;
	char szID[16];

	if (nID1 != -1)
	{
		strName += "_";
		_itoa_s(nID1, szID, 10);
		strName += szID;
	}
	if (nID2 != -1)
	{
		strName += "_";
		_itoa_s(nID2, szID, 10);
		strName += szID;
	}	
	return strName;
}

inline bool _MAKE_LUA_TABLE_IMPL(MWLua* pLua, const char* pszName)
{
	VALID_RET(pszName, false);
	VALID_RET(strlen(pszName) > 0, false);

	if (pLua->IsExistGlobal(pszName))
	{
		pLua->SetNull(pszName);
	}
	return WRUNSTRING(pLua, (string(pszName)+" = {}").c_str());
}

inline string _MAKE_LUA_FILE_IMPL(string strPath, string strName)
{
	return strPath + strName  + ".lua";
}

inline bool _RUN_LUA_IMPL(MWLua* pLua, string strPath, string strName)
{
	VALID_RET(strName.length() > 0, false);

	string strFileName = _MAKE_LUA_FILE_IMPL(strPath, strName);
	if (!::MIsExistFileA(strFileName.c_str()))
		return false;

	return WRUNFILE(pLua, strFileName.c_str());
}

#define MAKE_LUAFUNC0(func)				(func)
#define MAKE_LUAFUNC1(func, arg1)		_MAKE_LUANAME_IMPL((func),(arg1)).c_str()
#define MAKE_LUAFUNC2(func, arg1, arg2)	_MAKE_LUANAME_IMPL((func),(arg1),(arg2)).c_str()
#define MAKE_LUAFILE_FIELD(id)			_MAKE_LUANAME_IMPL("Field", (id)).c_str()
#define MAKE_LUATABLE_FIELD(id)			_MAKE_LUANAME_IMPL("Field", (id)).c_str()
#define MAKE_LUAFILE_QUEST(id)			_MAKE_LUANAME_IMPL("Quest", (id)).c_str()
#define MAKE_LUATABLE_QUEST(id)			_MAKE_LUANAME_IMPL("Quest", (id)).c_str()
#define MAKE_LUAFILE_BUFF()				"Buff"
#define MAKE_LUATABLE_BUFF(id)			_MAKE_LUANAME_IMPL("Buff", (id)).c_str()
#define MAKE_TABLE(name)				_MAKE_LUA_TABLE_IMPL(WLUA, (name))
#define MAKE_LUAFILE(path, file)		_MAKE_LUA_FILE_IMPL((path), (file))
#define RUN_LUA(path, file)				_RUN_LUA_IMPL(WLUA, (path), (file))