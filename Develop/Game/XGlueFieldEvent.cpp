#include "stdafx.h"
#include "XGlueFieldEvent.h"
#include "XFieldInfo.h"

#define		RVALID(x)			{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); return; }}
#define		RVALID_RET(x, r)	{ if (!(x)) { mlog3("\""#x"\" is invalid. (%s() ==> %s:%d)\n", __FUNCTION__, __FILE__, __LINE__); return (r); }}
#define		VALID(x)			{ assert(x); RVALID(x) }
#define		VALID_RET(x, r)		{ assert(x); RVALID_RET(x,r) }

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

#define WLUA global.script->GetLua()

#define MAKE_LUAFUNC0(func)				(func)
#define MAKE_LUAFUNC1(func, arg1)		_MAKE_LUANAME_IMPL((func),(arg1)).c_str()
#define MAKE_LUAFUNC2(func, arg1, arg2)	_MAKE_LUANAME_IMPL((func),(arg1),(arg2)).c_str()
#define MAKE_LUAFILE_FIELD(id)			_MAKE_LUANAME_IMPL("Field", (id)).c_str()
#define MAKE_LUATABLE_FIELD(id)			_MAKE_LUANAME_IMPL("Field", (id)).c_str()
#define MAKE_LUAFILE_QUEST(id)			_MAKE_LUANAME_IMPL("Quest", (id)).c_str()
#define MAKE_LUATABLE_QUEST(id)			_MAKE_LUANAME_IMPL("Quest", (id)).c_str()
#define MAKE_TABLE(name)				_MAKE_LUA_TABLE_IMPL(WLUA, (name))
#define MAKE_LUAFILE(path, file)		_MAKE_LUA_FILE_IMPL((path), (file))
#define RUN_LUA(path, file)				_RUN_LUA_IMPL(WLUA, (path), (file))

const char* pszLuaFieldFuncnames[]={
	"OnCreate",
	"OnDestroy",
	"OnTimeChanged",
	"OnWeatherChanged",
	"OnTimer",
	"OnSensorEnter",
	"OnSensorLeave",
	"OnSensorInteract",
	"OnSensorTalent",
	"OnSpawn",
	"OnDie",
	"OnAreaEnter",
	"OnAreaLeave",
};

#define LUA_CALLBACK_FUNCNAME_FIELD_CREATE				pszLuaFieldFuncnames[0]
#define LUA_CALLBACK_FUNCNAME_FIELD_DESTROY				pszLuaFieldFuncnames[1]
#define LUA_CALLBACK_FUNCNAME_FIELD_TIME_CHANGED		pszLuaFieldFuncnames[2]
#define LUA_CALLBACK_FUNCNAME_FIELD_WEATHER_CHANGED		pszLuaFieldFuncnames[3]
#define LUA_CALLBACK_FUNCNAME_FIELD_TIMER				pszLuaFieldFuncnames[4]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_ENTER		pszLuaFieldFuncnames[5]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_LEAVE		pszLuaFieldFuncnames[6]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_INTERACT		pszLuaFieldFuncnames[7]
#define LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_USETALENT	pszLuaFieldFuncnames[8]
#define LUA_CALLBACK_FUNCNAME_FIELD_NPC_SPAWN			pszLuaFieldFuncnames[9]
#define LUA_CALLBACK_FUNCNAME_FIELD_NPC_DESPAWN			pszLuaFieldFuncnames[10]
#define LUA_CALLBACK_FUNCNAME_FIELD_AREA_ENTER			pszLuaFieldFuncnames[11]
#define LUA_CALLBACK_FUNCNAME_FIELD_AREA_LEAVE			pszLuaFieldFuncnames[12]

#define PATH_SCRIPT					"./data/script/"

#define PATH_SCRIPT_FIELD			PATH_SCRIPT"field/"

XGlueFieldEvent::XGlueFieldEvent() : m_pMWLua(NULL)
{

}

void XGlueFieldEvent::Init( MWLua* pLua )
{
	m_pMWLua = pLua;
}

bool XGlueFieldEvent::PreCheck()
{
	return global.script->GetLua() ? true : false;
}

void XGlueFieldEvent::SetNULL( XFieldInfo* pFieldInfo )
{
	if (!PreCheck())	return;

	string strFileName = MAKE_LUAFILE_FIELD(pFieldInfo->m_nFieldID);
	string strTableName = MAKE_LUATABLE_FIELD(pFieldInfo->m_nFieldID);
	MAKE_TABLE(strTableName.c_str());
}

void XGlueFieldEvent::Load( XFieldInfo* pFieldInfo )
{
	string strFileName = MAKE_LUAFILE_FIELD(pFieldInfo->m_nFieldID);
	string strTableName = MAKE_LUATABLE_FIELD(pFieldInfo->m_nFieldID);
	MAKE_TABLE(strTableName.c_str());

	// 스크립트 전처리
	const string strScriptFilename = MAKE_LUAFILE(PATH_SCRIPT_FIELD, strFileName);

	MFile mf;
	if ( mf.Open( strScriptFilename.c_str()) == true)
	{
		size_t _size = mf.GetLength();
		char* pBuffer = new char[ _size + 1];
		memset( pBuffer, 0, _size + 1);

		mf.ReadAll( pBuffer, _size);
		mf.Close();

		if ( WRUNSTRING( m_pMWLua, pBuffer) == false)
			dlog("필드 스크립트 로딩 실패. Field:%s(%d)\n", MLocale::ConvUTF16ToAnsi(pFieldInfo->GetName().c_str()).c_str(), pFieldInfo->m_nFieldID);

		delete [] pBuffer;
	}
}

bool XGlueFieldEvent::HasSensorScript( int nFieldID, int nSensorID )
{
	return true;
}

bool XGlueFieldEvent::OnSensorEnter( XFieldInfo* pField, int nSensorID, XObject* pActor )
{
	if (!PreCheck())	return false;

	string strTableName = MAKE_LUATABLE_FIELD(pField->m_nFieldID);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_ENTER, nSensorID);

	if(!m_pMWLua->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), pField, pActor, WNULL);

	return bSuccess;
}

bool XGlueFieldEvent::OnSensorLeave( XFieldInfo* pField, int nSensorID, XObject* pActor )
{
	if (!PreCheck())	return false;

	string strTableName = MAKE_LUATABLE_FIELD(pField->m_nFieldID);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_SENSOR_LEAVE, nSensorID);

	if(!m_pMWLua->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), pField, pActor, WNULL);

	return bSuccess;
}

//bool XGlueFieldEvent::OnSensorInteract( XFieldInfo* pField, int nSensorID, XObject* pPlayer )
//{
//	return true;
//}
//
//bool XGlueFieldEvent::OnSensorTalent( XFieldInfo* pField, int nSensorID, XObject* pActor, int nTalentID )
//{
//	return true;
//}

bool XGlueFieldEvent::OnAreaEnter( XFieldInfo* pField, int nAreaID)
{
	if (!PreCheck())	return false;

	string strTableName = MAKE_LUATABLE_FIELD(pField->m_nFieldID);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_AREA_ENTER, nAreaID);

	if(!m_pMWLua->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), pField, NULL, WNULL);

	return bSuccess;
}

bool XGlueFieldEvent::OnAreaLeave( XFieldInfo* pField, int nAreaID)
{
	if (!PreCheck())	return false;

	string strTableName = MAKE_LUATABLE_FIELD(pField->m_nFieldID);
	string strFuncName = MAKE_LUAFUNC1(LUA_CALLBACK_FUNCNAME_FIELD_AREA_LEAVE, nAreaID);

	if(!m_pMWLua->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), pField, NULL, WNULL);

	return bSuccess;
}