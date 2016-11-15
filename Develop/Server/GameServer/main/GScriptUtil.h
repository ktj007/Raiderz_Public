#pragma once

class GGlueNPC;
class GGlueField;

class GScriptUtil
{
public:
	bool MakeSpawnTable(MWLua::table* poutTable , int nSpawnID, GGlueNPC* pNPC, GGlueField* pField);
	bool MakeDespawnTable(MWLua::table* poutTable, int nSpawnID, GGlueNPC* pNPC, GGlueField* pField);
};

bool HandleFail(const string& strTableName, const string& strFuncName, bool bSuccess);

template<typename T>
typename T::NullType* GetNullObject(T* ptr)
{
	return T::NullType::GetNull();
}

#define TO_SAFE_LUAPTR(ptr)		((ptr)?(ptr)->GetDelegator():GetNullObject(ptr))


//////////////////////////////////////////////////////////////////////////
// 
//	Implement
//
//////////////////////////////////////////////////////////////////////////

#include "GScriptManager.h"

static bool HandleFail(const string& strTableName, const string& strFuncName, bool bSuccess)
{
	if (bSuccess)	return true;

	// 로그 출력
	mlog3("Script Error: %s:%s()\n", strTableName.c_str(), strFuncName.c_str());

	return false;
}

template<typename T>
bool LuaCallback(T* pThis, string strFuncName)
{
	VALID_RET(pThis, false);	
	string strTableName = pThis->GetLuaTableName();

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL1(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pThis, WNULL); 

	::HandleFail(strTableName, strFuncName, bSuccess);
	return bSuccess;
}

template<typename T, typename A1>
bool LuaCallback(T* pThis, string strFuncName, const A1& Arg1)
{
	VALID_RET(pThis, false);	
	string strTableName = pThis->GetLuaTableName();

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL2(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pThis, Arg1, WNULL); 

	::HandleFail(strTableName, strFuncName, bSuccess);
	return bSuccess;
}

template<typename T, typename A1, typename A2>
bool LuaCallback(T* pThis, string strFuncName, const A1& Arg1, const A2& Arg2)
{
	VALID_RET(pThis, false);	
	string strTableName = pThis->GetLuaTableName();

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL3(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pThis, Arg1, Arg2, WNULL); 

	::HandleFail(strTableName, strFuncName, bSuccess);
	return bSuccess;
}

template<typename T, typename A1, typename A2, typename A3>
bool LuaCallback(T* pThis, string strFuncName, const A1& Arg1, const A2& Arg2, const A3& Arg3)
{
	VALID_RET(pThis, false);	
	string strTableName = pThis->GetLuaTableName();

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL4(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pThis, Arg1, Arg2, Arg3, WNULL); 

	::HandleFail(strTableName, strFuncName, bSuccess);
	return bSuccess;
}

template<typename T, typename A1, typename A2, typename A3, typename A4>
bool LuaCallback(T* pThis, string strFuncName, const A1& Arg1, const A2& Arg2, const A3& Arg3, const A4& Arg4)
{
	VALID_RET(pThis, false);	
	string strTableName = pThis->GetLuaTableName();

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL5(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pThis, Arg1, Arg2, Arg3, Arg4, WNULL); 

	::HandleFail(strTableName, strFuncName, bSuccess);
	return bSuccess;
}


template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
bool LuaCallback(T* pThis, string strFuncName, const A1& Arg1, const A2& Arg2, const A3& Arg3, const A4& Arg4, const A5& Arg5)
{
	VALID_RET(pThis, false);	
	string strTableName = pThis->GetLuaTableName();

	if (!WLUA->IsExistMemberFunc(strTableName.c_str(), strFuncName.c_str()))
		return false;

	bool bSuccess = WMEMCALL6(WLUA, strTableName.c_str(), strFuncName.c_str(), 
		pThis, Arg1, Arg2, Arg3, Arg4, Arg5, WNULL); 

	::HandleFail(strTableName, strFuncName, bSuccess);
	return bSuccess;
}
