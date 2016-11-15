#pragma once

/// C++, Lua에서 공통으로 사용하는 함수들
class XCommonFunction
{
public:
	static void Connect(const wchar_t* szLoginID, const wchar_t* szPassword, const wchar_t* szIP);
	static const wchar_t* GetPlayerName( MUID uid);
	static const wchar_t* GetItemName(int nItemID);

};